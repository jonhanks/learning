#include "allegro.hh"
#include "embed.h"
#include "plugin.h"

#include "clara.hpp"

#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

#include <boost/dll/import.hpp>
#include <boost/filesystem.hpp>

std::string toString( clara::Opt const& opt ) {
    std::ostringstream oss;
    oss << (clara::Parser() | opt);
    return oss.str();
}
std::string toString( clara::Parser const& p ) {
    std::ostringstream oss;
    oss << p;
    return oss.str();
}

boost::filesystem::path find_latest_plugin(boost::filesystem::path lib_dir)
{
    using namespace boost::filesystem;
    path latest;
    std::time_t latest_time = -1;

    const std::string post_fix = ".so";

    for (directory_entry& entry: directory_iterator(lib_dir))
    {
        if (!is_regular_file(entry.path()))
        {
            continue;
        }
        std::string name = entry.path().string();

        if (name.size() <= post_fix.size())
        {
            continue;
        }
        if (name.substr(name.size() - post_fix.size()) != post_fix)
        {
            continue;
        }
        std::time_t cur_time = last_write_time(entry.path());
        if (cur_time > latest_time)
        {
            latest_time = cur_time;
            latest = entry.path();
        }
    }

    return latest;
}

class BasicDraw: public lrn::SimpleDrawApi
{
public:
    ~BasicDraw() override {}

    std::string name() const override { return "BasicDraw"; }
    void draw() override {
        auto center = Point{100., 150.};
        auto red = make_color(1., .3, .3);

        filled_circle(center, 15., red);
    }
};

void dump_api(const std::string& path)
{
    auto extract_resource = [](boost::optional<simple_embed::resource>& opt) -> std::string {
        simple_embed::resource r = *opt;
        return std::string{reinterpret_cast<char const *>(std::get<std::uint8_t const*>(r)), std::get<std::size_t>(r)};
    };
    
    auto alleg_opt = simple_embed::get("allegro");
    auto plug_opt = simple_embed::get("plugin");
    if (!alleg_opt || !plug_opt)
    {
        throw(std::runtime_error("Unable to find api, invalid build"));
    }
    auto allegro = extract_resource(alleg_opt);
    auto plugin = extract_resource(plug_opt);
    
    auto out_f = std::ofstream{path};
    out_f << allegro << "\n" << plugin << "\n";    
}

int main(int argc, char* argv[]) {
    std::string pluginDir;
    std::string apiFile;
    bool showHelp = false;
    auto cli = clara::Help( showHelp)
            | clara::Opt( pluginDir, "plugin directory")
            ["-p"]["--plugins"]
            ("Directory to search for plugins.")
            | clara::Opt( apiFile, "path")
            ["-a"]["--api"]
            ("Write out the plugin api to the given file and exit.");
    auto cliResult = cli.parse( clara::Args( argc, argv ));
    if (showHelp)
    {
        std::cout << toString(cli) << std::endl;
        exit(1);
    }
    if (!apiFile.empty())
    {
        dump_api(apiFile);
        return 0;
    }

    auto display = al5::create_display();
    auto event_queue = al5::create_event_queue();
    auto timer = al5::create_timer(1./10.);

    boost::filesystem::path lib_path = (pluginDir.empty() ? "plugins": pluginDir);


    al5::register_event_source_kbd(event_queue.get());
    al5::register_event_source(event_queue.get(), timer.get());
    al5::register_event_source(event_queue.get(), display.get());

    boost::shared_ptr<lrn::SimpleDrawApi> backup_draw = boost::make_shared<BasicDraw>();
    // Note: this MUST come before the draw_plugin entry, so that the
    // unloading is done in a sane manner.  Destroy the plugin and then
    // drop the library.
    boost::shared_ptr<boost::dll::shared_library> cur_loaded_lib;
    boost::shared_ptr<lrn::SimpleDrawApi> draw_plugin;

    //draw_plugin = boost::dll::import<lrn::SimpleDrawApi>(lib_path / "plugin1",
    //    "plugin",
    //    boost::dll::load_mode::append_decorations);


    bool redraw = true;
    ::al_start_timer(timer.get());

    auto black{al_map_rgb(0, 0, 0)};
    auto white{al_map_rgb(255, 255, 255)};

    boost::filesystem::path current_plugin_path;

    while (true) {
        const double SCREEN_WIDTH = static_cast<double>(al5::WINDOW_WIDTH);
        const double SCREEN_HEIGHT = static_cast<double>(al5::WINDOW_HEIGHT);

        ALLEGRO_EVENT event;
        ::al_wait_for_event(event_queue.get(), &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
            //update_logic();
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                break;
            }
            //get_user_input();
        }

        auto latest_plugin = find_latest_plugin(lib_path);
        if (latest_plugin != current_plugin_path)
        {
            draw_plugin.reset();
            cur_loaded_lib.reset();
            cur_loaded_lib = boost::make_shared<boost::dll::shared_library>(latest_plugin);
            current_plugin_path = latest_plugin;
            draw_plugin = cur_loaded_lib->get<boost::shared_ptr<lrn::SimpleDrawApi>()>("plugin_factory")();
        }

        if (redraw && ::al_is_event_queue_empty(event_queue.get())) {
            if (!draw_plugin)
            {
                draw_plugin = backup_draw;
            }

            al_clear_to_color(white);
            draw_plugin->draw();
            //al_draw_line(0.0, static_cast<float>(y), SCREEN_WIDTH, static_cast<float>(y), al_map_rgb(0, 255, 0), 2.0f);
            al_flip_display();
        }
    }
    return 0;
}