#ifndef PLANET_H
#define PLANET_H

#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "object3d.h"
#include "colors.h"
#include "moon.h"
#include "movable.h"
#include "collidable.h"

using namespace std;
using namespace colors;

class planet : public object3d, public movable, public drawable {

public:
    planet(const string &name = "planet", float radius = 1.0f, float degree = 1.0f, float p[3] = NULL, const color_name &c = green, unsigned t_id = 0):
    object3d(name, radius, false, vector3<float>(p[0], p[1], p[2]), vector3<float>(0, 1, 0)),
    radius(radius),
    degree(degree),
    color(c) {

        texture_id = t_id;
        sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricTexture(sphere, GL_TRUE);
        gluQuadricNormals(sphere, GLU_SMOOTH);

        for (int i = 0; i < 3; ++i) {
            position[i] = p[i];
        }
        collidable = true;
        rotate_about = true;
        bounding_sphere_radius = radius + 10;
    }

    ~planet() {
        for_each(moons.begin(), moons.end(), [&](moon *&m) {
            delete m;
        });
    }

    void add(moon *m) {
        moons.push_back(m);
    }

    vector<moon *> get_moons() const {
        return moons;
    }

    void draw() {
        glPushMatrix();
        {
            // save it for moons
            glGetFloatv(GL_MODELVIEW_MATRIX, camera_view);
            // update
            glMultMatrixf(OM);
            // actual rendering
            draw_itself();
            // draw axis or bounding sphere
            object3d::draw();
            // draw all moons
            draw_children();
        }
        glPopMatrix();
    }

    void draw_itself() {
        glColor3fv(get_color(color));
        glEnable(GL_TEXTURE_2D);
        {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            gluSphere(sphere, radius, 200, 40);
            object3d::draw();
        }
        glDisable(GL_TEXTURE_2D);
    }

    void draw_children() {
        for_each(moons.begin(), moons.end(), [&](moon *m) {
            m->draw();
        });
    }

    void update_children() {
        for_each(moons.begin(), moons.end(), [&](moon *m) {
            m->set_parent_position(get_position());
            m->update();
        });
    }

    void update() {
        yaw += degree;
        if (yaw >= 360.0f) {
            yaw = 0.0f;
        }
        object3d::update_internal();
        update_children();
    }

    void toggle_moons_debug_on() {
        for_each(moons.begin(), moons.end(), [this](moon *m) {
            m->set_debug_on(this->is_debug_on());
        });
    }

    void toggle_moons_solid_wire() {
        for_each(moons.begin(), moons.end(), [this](moon *m) {
            m->set_solid(this->is_solid());
        });
    }

private:
    float radius;
    float degree;
    color_name color;
    vector<moon *> moons;
    unsigned texture_id;
    GLUquadricObj *sphere;
    float camera_view[16];
};

#endif