#ifndef LIGHT_H
#define LIGHT_H

#include <sstream>
#include <string>

#include "drawable.h"
#include "object3d.h"
#include "colors.h"

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

using namespace std;

struct light_direction {
    enum dir {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    light_direction(dir d) : d(d) {
    }

    light_direction() {
        d = LEFT;
    }

    operator dir() const {
        return d;
    }

    dir d;
private:
    // prevent automatic conversion for any other
    // built-in types such as bool, int, etc
    template<typename T>
    operator T() const;
};

class light: public object3d, public drawable {

public:
    enum light_type {
        DIRECTIONAL = 0,
        POSITIONAL  = 1
    };

public:
    light() {
    }

    light(const string &name, const light_type& type = POSITIONAL, float x = 0.0f, float y = 0.0f, float z = 0.0f) :
    object3d(name),
    type(type),
    theta_x(0.0f), theta_z(0.0f),
    amb_intensity(0.8), dif_intensity(1.0) {
        light_position[0] = x;
        light_position[1] = y;
        light_position[2] = z;
        MV[object3d::Tx] = x;
        MV[object3d::Ty] = y;
        MV[object3d::Tz] = z;
        for (int i = 0; i < 3; ++i) {
            diffuse[i] = 1.0f;
            ambient[i] = 1.0f;
            specular[i] = 1.0f;
            global_ambient[i] = amb_intensity;
        }
        diffuse[3] = ambient[3] = specular[3] = light_position[3] = global_ambient[3] = type;
    }

    void set_ambient(float a[4]) {
        for (int i = 0; i < 4; ++i) {
            ambient[i] = a[i];
        }
    }

    void set_diffuse(float d[4]) {
        for (int i = 0; i < 4; ++i) {
            diffuse[i] = d[i];
        }
    }

    void set_specular(float s[4]) {
        for (int i = 0; i < 4; ++i) {
            specular[i] = s[i];
        }
    }

    string float_arry_to_string(const float ary[4]) const {
        ostringstream ostr;
        ostr << ary[0] << " " << ary[1] << " " << ary[2] << " " << ary[3];
        return ostr.str();
    }

    string get_ambient_string() const {
        return float_arry_to_string(ambient);
    }

    string get_diffuse_string() const {
        return float_arry_to_string(diffuse);
    }

    string get_specular_string() const {
        return float_arry_to_string(specular);
    }

    string get_global_ambient_string() const {
        return float_arry_to_string(global_ambient);
    }

    void turn_on() {
        setup_lighting();
        setup_material();
        // enable color tracking
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_DEPTH_TEST);	// hidden surface removal
        // glEnable(GL_CULL_FACE);	// do not calculate inside of jet
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
        // set material properties to follow glColor values
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        // extra stuff, should NOT be here?
        glFrontFace(GL_CCW);		// counter clock-wise polygons face out
        // black blue background
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }

    void draw() {
        float cm[16];
        glPushMatrix(); {
            glColor3fv(get_color(colors::cyan));
            glGetFloatv(GL_MODELVIEW_MATRIX, cm);
            glLoadIdentity();
            glRotatef(theta_x, 1.0, 0.0, 0.0);
            glRotatef(theta_z, 0.0, 0.0, 1.0);
            glTranslatef(light_position[0], light_position[1], light_position[2]);
            glGetFloatv(GL_MODELVIEW_MATRIX, OM);
            glLoadMatrixf(cm);
            glMultMatrixf(OM);
            turn_on();
            glutWireCube(500);
        } glPopMatrix();
    }

    void turn(const light_direction &light) {
        switch (light.d) {
            case light_direction::DOWN:
                theta_x += 1.0f;
                if (theta_x > 360.0f) {
                    theta_x -= 360.0f;
                }
                break;

            case light_direction::UP:
                theta_x -= 1.0f;
                if (theta_x < 0.0f) {
                    theta_x += 360.0f;
                }
                break;

            case light_direction::RIGHT:
                theta_z += 1.0f;
                if (theta_z > 360.0f) {
                    theta_z -= 360.0f;
                }
                break;

            case light_direction::LEFT:
                theta_z -= 1.0f;
                if (theta_z < 0.0f) {
                    theta_z += 360.0f;
                }
                break;
        }
    }

    void reset() {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);
        glDisable(GL_LIGHT4);
        glDisable(GL_LIGHT5);
        glDisable(GL_LIGHT6);
        glDisable(GL_LIGHT7);
    }

    string get_position_str() const {
        ostringstream ostr;
        ostr << OM[Tx] << " " << OM[Ty] << " " << OM[Tz];
        return ostr.str();
    }

    void set_diffuse_intensity(float d) {
        dif_intensity = d;
    }

    void set_ambient_intensity(float a) {
        amb_intensity = a;
    }

    float get_ambient_intensity() const {
        return amb_intensity;
    }

    float get_diffuse_intensity() const {
        return dif_intensity;
    }

    void increase_amb() {
        if (amb_intensity < 1.0f) {
            amb_intensity += 0.05;
        }
    }

    void decrease_amb() {
        if (amb_intensity > 0.0f) {
            amb_intensity -= 0.05;
        }
    }

    void increase_diff() {
        if (dif_intensity < 1.0f) {
            dif_intensity += 0.05;
        }
    }

    void decrease_diff() {
        if (dif_intensity > 0.0f) {
            dif_intensity -= 0.05;
        }
    }

private:
    void setup_material() {
        // material property vectors.
        float mat_amb[]   = { 1.0, 1.0, 0.0, 1.0 };
        float mat_diff[]  = { 1.0, 0.0, 0.0, 1.0 };
        float mat_spec[]  = { 1.0, 1.0, 1,0, 1.0 };
        float mat_shine[] = { 120.0 };
        // material properties of sphere.
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_diff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shine);
    }

    void setup_lighting() {
        glEnable(GL_LIGHTING);
        for (int i = 0; i < 3; ++i) {
            diffuse[i] = specular[i] = dif_intensity;
            global_ambient[i] = amb_intensity;
        }
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    }

private:
    light_type type;
    float diffuse[4];
    float ambient[4];
    float global_ambient[4];
    float specular[4];
    float light_position[4];
    float theta_x;
    float theta_z;
    float amb_intensity;
    float dif_intensity;
};

#endif