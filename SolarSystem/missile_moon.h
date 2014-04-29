#ifndef MISSILE_MOON_H
#define MISSILE_MOON_H

#include <string>
#include <vector>

#include "moon.h"
#include "missile.h"
#include "shootable.h"
#include "colors.h"
#include "math3d.h"
#include "object3d.h"
#include "torpedo.h"

using namespace std;
using namespace colors;

class missile_moon: public moon, public shootable {

private:
    static const int TOTAL_MISSILES = 10;

public:
    missile_moon(const string &name = "missile_moon", float radius = 1.0f, float degree = 1.0f, float pos[3] = NULL, const color_name &c = red, const int distance = 5000):
    moon(name, radius, degree, pos, c),
    count(TOTAL_MISSILES),
    torpe(NULL),
    detection_distance(distance),
    alive(true),
    explosion_radius(500) {
    }

    ~missile_moon() {
    }

    bool scan_target(const object3d *obj) {
        vector3<float> actual_position = get_parent_position() + get_position();
        if (util::math3d::distance(actual_position, obj->get_position()) < detection_distance) {
            return true;
        }
        return false;
    }

    void draw_explosion() {
        if (explosion_radius > 0) {
            glColor3fv(get_color(colors::red));
            glutWireSphere(explosion_radius, 30, 17);
            explosion_radius -= 50;
        }
    }

    void draw() {
        glPushMatrix(); {
            // update itself
            glMultMatrixf(OM);
            if (alive) {
                moon::draw_itself();
            } else {
                draw_explosion();
            }
            // draw axes or bounding sphere
            object3d::draw();
        } glPopMatrix();
    }

    /**
     * Shoot target and return a smart torpedo
     * for galaxy
     */
    torpedo* shoot_target(const vector3<float> &target) {
        count--;
        if (count > 0) {
            vector3<float> start = get_parent_position() + get_position();
            vector3<float> initial_target = start;
            start[1] += 500;
            initial_target[1] += 5000;
            if (torpe == NULL) {
                torpe = new torpedo("torpedo", start, initial_target, colors::red, torpedo_type::AIM_4_FALCON, 10, 1000);
                return torpe;
            } else {
                torpe->set_new_position(start);
                torpe->set_new_target(initial_target);
                torpe->reborn();
                return torpe;
            }
        }
        return NULL;
    }

    int torpedo_left() const {
        return count;
    }

    torpedo *get_torpedo() {
        return torpe;
    }

    bool is_alive() const {
        return alive;
    }

    void destroy() {
        alive = false;
    }

private:
    bool alive;
    int count;
    const double detection_distance;
    torpedo *torpe;
    int explosion_radius;
};

#endif