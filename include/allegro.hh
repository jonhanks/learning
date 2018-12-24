#ifndef __TANKS_ALLEGRO5__
#define __TANKS_ALLEGRO5__

#include <memory>
#include <stdexcept>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

namespace al5 {

    const int WINDOW_WIDTH = 1024;
    const int WINDOW_HEIGHT = 768;

    namespace detail {
        inline void destroy_display(ALLEGRO_DISPLAY* display)
        {
            if (display) {
                ::al_destroy_display(display);
            }
        }

        inline void destroy_event_queue(ALLEGRO_EVENT_QUEUE* queue)
        {
            if (queue) {
                ::al_destroy_event_queue(queue);
            }
        }

        inline void destroy_timer(ALLEGRO_TIMER* timer)
        {
            if (timer) {
                ::al_destroy_timer(timer);
            }
        }
    }

    typedef std::unique_ptr<ALLEGRO_DISPLAY, decltype(&detail::destroy_display)> unique_display;
    typedef std::unique_ptr<ALLEGRO_EVENT_QUEUE, decltype(&detail::destroy_event_queue)> unique_event_queue;
    typedef std::unique_ptr<ALLEGRO_TIMER, decltype(&detail::destroy_timer)> unique_timer;

    inline al5::unique_display create_display()
    {
        ::al_set_new_display_flags(ALLEGRO_WINDOWED);
        auto display = ::al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT);
        if (!display) {
            throw std::runtime_error("Unable to create display");
        }
        return unique_display{display, &detail::destroy_display};
    }

    inline unique_event_queue create_event_queue()
    {
        auto queue = ::al_create_event_queue();
        if (!queue) {
            throw std::runtime_error("Unable to create event queue");
        }
        return unique_event_queue{queue, &detail::destroy_event_queue};
    }

    inline unique_timer create_timer(double period)
    {
        auto timer = ::al_create_timer(period);
        if (!timer) {
            throw std::runtime_error("Unable to create timer");
        }
        return unique_timer{timer, &detail::destroy_timer};
    }

    inline void register_event_source_kbd(ALLEGRO_EVENT_QUEUE* queue)
    {
        ::al_register_event_source(queue, ::al_get_keyboard_event_source());
    }
    inline void register_event_source(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_TIMER* timer)
    {
        ::al_register_event_source(queue, ::al_get_timer_event_source(timer));
    }
    inline void register_event_source(ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_DISPLAY* display)
    {
        ::al_register_event_source(queue, ::al_get_display_event_source(display));
    }

    /// Handle required library initialization
    class library_init {
    public:
        library_init() {
            if (!al_init()) {
                throw std::runtime_error("Unable to initialize allegro library");
            }
            if (!al_install_keyboard()) {
                throw std::runtime_error("Unable to initialize the allegro keyboard handler");
            }
            if (!al_init_primitives_addon()) {
                throw std::runtime_error("Unable to initialize primitives library");
            }
        }

        ~library_init()
        {
            al_shutdown_primitives_addon();
        }
    };

    namespace detail {
        static library_init __library_init;
    }

}
#endif // __TANKS_ALLEGRO5__

