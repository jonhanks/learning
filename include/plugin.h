//
// Created by jonathan on 11/30/18.
//

#ifndef LEARNING_PLUGIN_H
#define LEARNING_PLUGIN_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include <string>

struct Point {
    Point(float x_, float y_): x{x_}, y{y_} {}
    Point(const Point& other) = default;
    Point& operator=(const Point& other) = default;

    float x = static_cast<float>(0.);
    float y = static_cast<float>(0.);
};


using Color = ALLEGRO_COLOR;

inline Color make_color(float r, float g, float b)
{
    return al_map_rgb_f(r, g, b);
}

inline void circle(Point center, float radius, Color col, float thickness = 1.0) {
    al_draw_circle(center.x, center.y, radius, col, thickness);
};

inline void filled_circle(Point center, float radius, Color col) {
    al_draw_filled_circle(center.x, center.y, radius, col);
};

inline void arc(Point center, float radius, float start_perc, float delta_perc, Color col, float thickness = 1.0)
{
    al_draw_arc(center.x, center.y, radius, start_perc*4, delta_perc*4, col, thickness);
}

inline void line(Point p1, Point p2, Color col, float thickness = 1.0)
{
    al_draw_line(p1.x, p1.y, p2.x, p2.y, col, thickness);
}

inline void rectangle(Point p1, Point p2, Color col, float thickness = 1.0)
{
    al_draw_rectangle(p1.x, p1.y, p2.x, p2.y, col, thickness);
}

inline void filled_rectangle(Point p1, Point p2, Color col)
{
    al_draw_filled_rectangle(p1.x, p1.y, p2.x, p2.y, col);
}

namespace lrn {
    class SimpleDrawApi {
    public:
        virtual ~SimpleDrawApi() {}

        virtual std::string name() const = 0;
        virtual void draw() = 0;
    };
};


#endif //LEARNING_PLUGIN_H
