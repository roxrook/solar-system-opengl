#ifndef PARTICLE_ENGINE_H
#define PARTICLE_ENGINE_H

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "vector3.h"
#include "drawable.h"
#include "movable.h"
#include "math3d.h"
#include "object3d.h"

using namespace std;

const int NUM_PARTICLES = 2000;

vector3<float> adjust_particle_pos(const vector3<float> &pos) {
    vector3<float> axis(1, 0, 0);
    return math3d::rotate<float>(pos, axis, -30.0f);
}

float random_float() {
    return (float)rand() / ((float)RAND_MAX + 1);
}

struct particle {
    vector3<float> position;
    vector3<float> velocity;
    vector3<float> color;
    float time_alive;		// the amount of time that this particle has been alive.
    float life_span;		// the total amount of time that this particle is to live.

public:
    bool operator <(const particle &rhs) const {
        return adjust_particle_pos(position)[2] < adjust_particle_pos(rhs.position)[2];
    }

    bool operator >(const particle &rhs) const {
        return adjust_particle_pos(position)[2] > adjust_particle_pos(rhs.position)[2];
    }
};

class particle_engine : public object3d, public drawable, public movable {

public:
    particle_engine(const string &name, unsigned t_id = 0, float scale_factor = 5000.0f, float gravity = 3.0f) :
    object3d("particle engine"),
    texture_on(false),
    step_time(0.01f),
    particle_size(0.05f),
    time_until_next_step(0.0f),
    color_time(0),
    angle(0.0f),
    texture_id(t_id),
    scale_factor(scale_factor),
    gravity(gravity) {
        for (int i = 0; i < NUM_PARTICLES; ++i) {
            create_particle(particles + i);
        }
        for (int i = 0; i < (5.0f / step_time); ++i) {
            step();
        }
    }

    /**
     * Advances the particle fountain by the specified amount of time.
     */
    void update(float dt) {
        while (dt > 0) {
            if (time_until_next_step < dt) {
                dt -= time_until_next_step;
                step();
                time_until_next_step = step_time;
            } else {
                time_until_next_step -= dt;
                dt = 0;
            }
        }
    }

    void update() {
        float dt = 40 / 1000.0f;
        while (dt > 0) {
            if (time_until_next_step < dt) {
                dt -= time_until_next_step;
                step();
                time_until_next_step = step_time;
            } else {
                time_until_next_step -= dt;
                dt = 0;
            }
        }
    }

    void draw() {
        vector<particle*> ps;
        for(int i = 0; i < NUM_PARTICLES; i++) {
            ps.push_back(particles + i);
        }
        sort(ps.begin(), ps.end());
        glPushMatrix(); {
            glScalef(scale_factor, scale_factor, scale_factor);
            if (texture_on) {
                glEnable(GL_TEXTURE_2D);
                glDisable(GL_LIGHTING);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else {
                glDisable(GL_TEXTURE_2D);
            }
            glBegin(GL_QUADS); {
                float size;
                for (unsigned int i = 0; i < ps.size(); i++) {
                    particle* p = ps[i];
                    glColor4f(p->color[0], p->color[1], p->color[2], (1 - p->time_alive / p->life_span));
                    size = particle_size / 2;
                    vector3<float> pos = adjust_particle_pos(p->position);
                    glTexCoord2f(0, 0);
                    glVertex3f(pos[0] - size, pos[1] - size, pos[2]);
                    glTexCoord2f(0, 1);
                    glVertex3f(pos[0] - size, pos[1] + size, pos[2]);
                    glTexCoord2f(1, 1);
                    glVertex3f(pos[0] + size, pos[1] + size, pos[2]);
                    glTexCoord2f(1, 0);
                    glVertex3f(pos[0] + size, pos[1] - size, pos[2]);
                }
            } glEnd();
        } glPopMatrix();
    }

    void toggle_texture_mode() {
        texture_on = !texture_on;
    }

private:
    /**
     * Returns the current color of particles produced by the fountain.
     */
    vector3<float> get_current_color() {
        vector3<float> color;
        if (color_time < 0.166667f) {
            color = vector3<float>(1.0f, color_time * 6, 0.0f);
        } else if (color_time < 0.333333f) {
            color = vector3<float>((0.333333f - color_time) * 6, 1.0f, 0.0f);
        } else if (color_time < 0.5f) {
            color = vector3<float>(0.0f, 1.0f, (color_time - 0.333333f) * 6);
        } else if (color_time < 0.666667f) {
            color = vector3<float>(0.0f, (0.666667f - color_time) * 6, 1.0f);
        } else if (color_time < 0.833333f) {
            color = vector3<float>((color_time - 0.666667f) * 6, 0.0f, 1.0f);
        } else {
            color = vector3<float>(1.0f, 0.0f, (1.0f - color_time) * 6);
        }
        // make sure each of the color's components range from 0 to 1
        for(int i = 0; i < 3; i++) {
            if (color[i] < 0) {
                color[i] = 0;
            } else if (color[i] > 1) {
                color[i] = 1;
            }
        }
        return color;
    }

    /**
     * Returns the average velocity of particles produced by the fountain.
     */
    vector3<float> get_current_velocity() {
        return vector3<float>(2 * cos(angle), 2.0f, 2 * sin(angle));
    }

    /**
     * Alters p to be a particle newly produced by the fountain.
     */
    void create_particle(particle *p) {
        p->position = vector3<float>(0.0f, 0.0f, 0.0f);
        p->velocity = get_current_velocity() +
                vector3<float>(
                        0.5f * random_float() - 0.25f,
                        0.5f * random_float() - 0.25f,
                        0.5f * random_float() - 0.25f);
        p->color = get_current_color();
        p->time_alive = 0;
        p->life_span = random_float() + 1;
    }

    /**
     * Advances the particle fountain by step_time seconds.
     */
    void step() {
        color_time += step_time / 10;
        while (color_time >= 1) {
            color_time -= 1;
        }
        angle += 0.5f * step_time;
        while (angle > 2 * util::constants::PI) {
            angle -= 2 * util::constants::PI;
        }
        for (int i = 0; i < NUM_PARTICLES; i++) {
            particle* p = particles + i;
            p->position += p->velocity * step_time;
            p->velocity += vector3<float>(0.0f, -gravity * step_time, 0.0f);
            p->time_alive += step_time;
            if (p->time_alive > p->life_span) {
                create_particle(p);
            }
        }
    }

private:
    unsigned texture_id;
    bool texture_on;
    float step_time;
    float gravity;
    float particle_size;
    float scale_factor;
    // the amount of time until the next call to step().
    float time_until_next_step;

    // the color of particles that the fountain is currently shooting.  0
    // indicates red, and when it reaches 1, it starts over at red again.  It
    // always lies between 0 and 1.
    float color_time;

    // the angle at which the fountain is shooting particles, in radians.
    float angle;
    particle particles[NUM_PARTICLES];
};

#endif