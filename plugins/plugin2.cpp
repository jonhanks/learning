//
// Created by jonathan on 11/30/18.
//
#include "allegro.hh"
#include "plugin.h"


#include <boost/config.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

class SimplePlugin: public lrn::SimpleDrawApi
{
public:
    SimplePlugin() {}
    ~SimplePlugin() override {}
    std::string name() const { return "Simple"; }
    void draw() override {
        al_draw_filled_circle(static_cast<float>(150.0),
                              static_cast<float>(150.0),
                              static_cast<float>(15.0),
                              al_map_rgb(0, 0, 255));
    }
};

// Exporting `my_namespace::plugin` variable with alias name `plugin`
// (Has the same effect as `BOOST_DLL_ALIAS(my_namespace::plugin, plugin)`)
extern "C" BOOST_SYMBOL_EXPORT boost::shared_ptr<lrn::SimpleDrawApi> plugin_factory()
{
    return boost::make_shared<SimplePlugin>();
}

