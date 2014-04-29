#ifndef ORACLE_H
#define ORACLE_H


#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

#include "object3d.h"
#include "colors.h"
#include "moon.h"
#include "movable.h"
#include "shootable.h"
#include "math3d.h"

using namespace std;
using namespace colors;

class oracle:
public object3d,
    public movable,
    public drawable,
    public shootable {

public:
    oracle(const string &name = "oracle", float radius = 1.0f, float degree = 1.0f, float p[3] = NULL, const color_name& c = green):
    object3d(name, radius, false, vector3<float>(p[0], p[1], p[2]), vector3<float>(0, 1, 0)),
    radius(radius),
    degree(degree),
    color(c) {
        for (int i = 0; i < 3; ++i) {
            position[i] = p[i];
        }
        rotate_about = true;
    }

    ~oracle() {

    }

    bool scan_target(const object3d *obj) {
        vector3<float> actual_position = get_position();
        if (util::math3d::distance(actual_position, obj->get_position()) < 5000) {
            return true;
        }
        return false;
    }

    void shoot(const vector3<float> &target) {
        // do nothing
    }

    /**
     * Return a smart torpedo for galaxy
     */
    torpedo* shoot_target(const vector3<float> &target) {
        return NULL;
    }

    void draw() {
        glPushMatrix(); {
            // save it for moons
            glGetFloatv(GL_MODELVIEW_MATRIX, camera_view);
            // update
            glMultMatrixf(OM);
            // actual rendering
            draw_itself();
            // draw axis or bounding sphere
            object3d::draw();
            // draw all moons
        } glPopMatrix();
    }

    void draw_itself() {
        glColor3fv(get_color(color));
        if (is_solid()) {
            glutSolidSphere(radius, 40, 20);
        } else {
            glutWireSphere(radius, 40, 20);
        }
    }

    void update() {
        yaw += degree;
        if (yaw >= 360.0f) {
            yaw = 0.0f;
        }
        object3d::update_internal();
    }

private:
    float radius;
    float degree;
    color_name color;
    float camera_view[16];
};

#endif