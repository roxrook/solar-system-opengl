#ifndef __SOLAR_SYSTEM_GALAXY_H
#define __SOLAR_SYSTEM_GALAXY_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <utility>
#include <stack>
#include <deque>
#include <queue>
#include <fstream>
#include <functional>
#include <numeric>
#include <memory>
#include <unordered_map>
#include <limits>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>
#include <array>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "colors.h"
#include "sun.h"
#include "planet.h"
#include "moon.h"
#include "planet.h"
#include "camera.h"
#include "light.h"
#include "spaceship.h"
#include "image.h"
#include "particle_engine.h"
#include "object3d.h"
#include "string_util.h"
#include "missile_moon.h"
#include "shootable.h"
#include "oracle.h"
#include "torpedo.h"
#include "torus.h"
#include "special_model.h"
#import "galaxy_constants.h"

using namespace colors;

const int TIME_QUANTUM[5] = {20, 40, 100, 250, 500};

extern const int VERTICAL_TEXT_OFFSET = 20;
extern const int GRAVITY = 90000000;
extern const int GRAVITY_THRESHOLD = 1800;
extern const int oo = 1000000000;
extern const int TRACKING_FRAME = 50;

class galaxy {
public:
    friend class display;

    enum information_mode {
        INFO_INTRODUCTION = 0,
        INFO_PLANET		  = 1,
        INFO_CAMERA		  = 2,
        INFO_LIGHT		  = 3,
        INFO_SPACESHIP	  = 4,
        INFO_GAME		  = 5,
        INFO_GRAVITY	  = 6
    };

    enum viewing_mode {
        PERSPECTIVE = 0,
        ORTHO		= 1
    };

    enum camera_mode {
        STATIC = 0,
        MOVING = 1
    };

    enum default_camera_type {
        FRONT			= 0,
        TOP				= 1,
        SHIP			= 2,
        SHIP_FOLLOWING	= 3
    };

    enum planet_camera_type {
        DYANMIC_UNUM		= 0,
        DYANMIC_DUO			= 1,
        DYNAMIC_TRES		= 2,
        DYNAMIC_QUATTUOR	= 3
    };


private:
    // disable copy, too expensive!
    galaxy(const galaxy &o);
    galaxy& operator =(const galaxy &o);

public:
    /**
     * Constructor
     */
    galaxy(const unordered_map<string, unsigned>& hm):

    info_mode(INFO_INTRODUCTION),

    led(new light("flash",
            light::light_type::DIRECTIONAL,
            galaxy_constants::lighting::position[0],
            galaxy_constants::lighting::position[1],
            galaxy_constants::lighting::position[2])),

    apollo(new spaceship(
            galaxy_constants::warbird::name,
            galaxy_constants::warbird::speed,
            galaxy_constants::warbird::base,
            galaxy_constants::warbird::height,
            galaxy_constants::warbird::position[0],
            galaxy_constants::warbird::position[1],
            galaxy_constants::warbird::position[2],
            colors::red,
            colors::green)),

    camera_index(0),
    planet_camera_index(0),
    on_planet_camera(false),
    bounding_sphere_mode(false),
    camera_view_mode(STATIC),
    gravity_on(false),
    debug_on(false),
    solid(true),
    planet_index(0),
    view_angle(70.0f),
    tq_idx(0),
    fps(0) {

        setup_texture_objects(hm);

        // set up environment
        setup_planets();
        setup_oracles();
        setup_toruses();
        setup_lights();
        setup_cameras();
        setup_smart_torpedo();
        setup_spaceship_followers();
        // sound::play_background();

        g2v_star->add_affected_objects(&unum_smart_torpedo);
        g2v_star->add_affected_objects(&tres_smart_torpedo);
        g2v_star->add_affected_objects(&ship_smart_torpedo);
        g2v_star->add_spaceship(&apollo);
    }

    /**
     * Reclaim memory
     */
    ~galaxy() {
        cout << "~galaxy()\n";
        for_each(planets.begin(), planets.end(), [&](planet *&p) { delete p; });
        for_each(followers.begin(), followers.end(), [&](spaceship *&f) { delete f; });
        delete apollo;
        delete led;
        delete engine;
        delete g2v_star;
        delete unum_smart_torpedo;
        delete tres_smart_torpedo;
        delete ship_smart_torpedo;
    }

    /**
     * Compute the gravity vector and force
     * TODO: fix divide by zero bug! 11/20/2012
     */
    pair<vector3<float>, float> get_gravity_vector_and_force(vector3<float> at_pos) const {
        pair<vector3<float>, float> result;
        double distance = math3d::distance(vector3<float>(0, 0, 0), at_pos);
        double force = 0.0f;
        if (distance < GRAVITY_THRESHOLD) {
            force = 0.0f;
        } else {
            force = static_cast<double>(GRAVITY)/(distance * distance);
        }

        vector3<float> sun_vector(0, 0, 0);
        vector3<float> gravity_vector = sun_vector - at_pos;
        gravity_vector.normalize();
        return pair<vector3<float>, float>(gravity_vector * force, force);
    }

    bool is_gravity_on() const {
        return gravity_on;
    }

    /**
     * Generate all models for display list
     */
    void generate_models() {
        special_model::generate_spaceship_model(1, 100.0f);
    }

    /**
     * Set the viewing volume of a viewing mode.
     *		- Text window view uses "orthonormal"
     *		- All other views uses "perspective"
     */
    void set_viewing_volume(const viewing_mode &m, int w, int h) const {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (m == PERSPECTIVE) {
            gluPerspective(view_angle, (float)w/h, 10.0, 900000.0);
        } else {
            glOrtho(-w/2, w, -h/2, h, -1.0, 1.0);
        }
    }

    /**
     * Actual drawing routine that is called
     * from glut_display_func()
     */
    void draw() {
        fps++;
        if (!on_planet_camera) {
            draw_with_camera(camera_index);
        } else {
            draw_with_planet_camera(planet_camera_index);
        }
    }

    /**
     * Draw all objects in scene
     */
    void draw_all() {
        // display game status
        draw_game_status();
        // draw galaxy with texture
        draw_galaxy_skybox(100000);
        // draw light
        led->draw();
        // draw the sun
        g2v_star->draw();
        // draw particles
        engine->draw();
        // draw moving spaceship, real!
        apollo->draw();
        // draw all ship's partners
        for_each(followers.begin(), followers.end(), [&](spaceship *sp) {
            sp->draw();
        });

        // draw smart torpedo
        unum_smart_torpedo->draw();
        tres_smart_torpedo->draw();
        ship_smart_torpedo->draw();

        for_each(planets.begin(), planets.end(), [&](planet *p) { p->draw(); });
        for_each(toruses.begin(), toruses.end(), [&](torus *t) { t->draw(); });
    }

    void draw_game_status() {
        glColor3fv(get_color(colors::white));
        glRasterPos3f(-15000.0, 15000.0, 0.0);
        if (is_player_lose()) {
            write_bitmap_string(GLUT_BITMAP_HELVETICA_18, "You lose!");
        } else {
            if (is_player_win()) {
                write_bitmap_string(GLUT_BITMAP_HELVETICA_18, "You win!");
            }
        }
    }

    bool is_player_lose() const {
        return (!apollo->is_alive() || apollo->is_run_out_of_torpedo());
    }

    bool is_player_win() const {
        bool result = true;
        for (unsigned i = 0; i < shootable_objects.size(); ++i) {
            if (shootable_objects[i]->is_alive()) {
                return false;
            }
        }
        return result;
    }

    /**
     * Draw with 4 default cameras: front, top, ship and ship-following
     */
    void draw_with_camera(int index) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix(); {
            // this is moving camera
            if (index == default_camera_type::SHIP_FOLLOWING) {
                vector3<float> offset(0, -170, -1000);
                // apply moving camera
                cameras[index]->apply_object3d_orientation(apollo, offset);
            }
            cameras[index]->capture();
            // draw all objects
            draw_all();
            // restore transformation
        } glPopMatrix();
        // draw
        glutSwapBuffers();
    }

    /**
     * Draw with camera that is above each moving planet.
     * The offset is different for each planet.
     */
    void draw_with_planet_camera(int index) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix(); {
            // apply planets' camera
            planet_cameras[index]->apply_planet_orientation(planets[index], galaxy_constants::camera_view::moving::planet_cameras[index]);
            // capture the scene
            planet_cameras[index]->capture();
            // draw all planets
            draw_all();
            // restore transformation
        } glPopMatrix();
        // draw
        glutSwapBuffers();
    }

    /**
     * Draw with camera on top
     */
    void draw_top() {
        draw_with_camera(default_camera_type::SHIP_FOLLOWING);
    }

    /**
     * Display all information
     */
    void draw_galaxy_info() {
        draw_info(-90, 20, 0);
        // reset fps
        fps = 0;
    }

    /**
     * Update all objects
     */
    void update() {
        for_each(planets.begin(), planets.end(), [&](planet *p) {
            p->update();
        });
        for_each(toruses.begin(), toruses.end(), [&](torus *t) {
            t->update();
        });

        // update non-moving objects
        engine->update();
        g2v_star->update();

        // update spaceship only if it's alive
        if (apollo->is_alive()) {
            apollo->update();
            for_each(followers.begin(), followers.end(), [&](spaceship *&sp) {
                sp->set_pitch(apollo->get_pitch());
                sp->set_yaw(apollo->get_yaw());
                sp->set_roll(apollo->get_roll());
                sp->update();
            });
        }

        update_shootable_objects();
        update_collidable_objects();

        // if gravity is on, apply for all movable objects
        if (gravity_on) {
            apply_gravity();
        }
    }

    /**
     * Update all collidable objects
     */
    void update_collidable_objects() {
        vector3<float> parent_position(0, 0, 0);
        for (unsigned i = 0; i < planets.size(); ++i) {
            handle_torpedo_collision(unum_smart_torpedo, planets[i]);
            handle_torpedo_collision(tres_smart_torpedo, planets[i]);
            handle_torpedo_collision(ship_smart_torpedo, planets[i]);

            handle_spaceship_collision(apollo, planets[i]);
            // get all moons of planets[i]
            vector<moon*> moons = planets[i]->get_moons();
            // get planets[i] position
            parent_position = planets[i]->get_position();
            // now check collision with respect to the world coordinate
            for (unsigned j = 0; j < moons.size(); ++j) {
                // update parent position
                moons[j]->set_parent_position(planets[i]->get_position());
                handle_moon_vs_torpedo(moons[j], unum_smart_torpedo);
                handle_moon_vs_torpedo(moons[j], tres_smart_torpedo);
                if (moons[j]->get_name() == "U.Missile" || moons[j]->get_name() == "T.Missile") {
                    handle_ship_smart_torpedo_collision(ship_smart_torpedo, (missile_moon*&)moons[j]);
                } else {
                    handle_moon_vs_torpedo(moons[j], ship_smart_torpedo);
                }
                handle_moon_vs_spaceship(moons[j], apollo);
            }
        }

        // spaceship vs. torpedoes
        if (unum_smart_torpedo->is_alive() && apollo->is_alive() && apollo->collide_with(unum_smart_torpedo)) {
            unum_smart_torpedo->destroy();
            apollo->destroy();
        }

        if (tres_smart_torpedo->is_alive() && apollo->is_alive() && apollo->collide_with(tres_smart_torpedo)) {
            tres_smart_torpedo->destroy();
            apollo->destroy();
        }

        handle_torpedo_collision(unum_smart_torpedo, g2v_star);
        handle_torpedo_collision(tres_smart_torpedo, g2v_star);
        handle_torpedo_collision(ship_smart_torpedo, g2v_star);
        handle_spaceship_collision(apollo, g2v_star);

        for_each(toruses.begin(), toruses.end(), [&](torus *t) {
            if (apollo->is_alive() && t->collide_with(apollo)) {
                apollo->destroy();
            }
        });
    }

    void handle_ship_smart_torpedo_collision(torpedo *&torpe, missile_moon *&m) {
        if (torpe->is_alive() && m->is_alive() && m->collide_with(torpe)) {
            m->destroy();
            torpe->destroy();
        }
    }

    void handle_moon_vs_torpedo(moon *&m, torpedo *&torpe) {
        if (m->collide_with(torpe)) {
            torpe->destroy();
        }
    }

    void handle_moon_vs_spaceship(moon *&m, spaceship *&sp) {
        if (m->collide_with(sp)) {
            sp->destroy();
        }
    }

    void handle_torpedo_collision(torpedo *&torpe, const object3d *other) {
        if (torpe->is_alive() && torpe->collide_with(other)) {
            cout << torpe->get_name() << " collides with " << other->get_name() << endl;
            torpe->destroy();
        }
    }

    void handle_spaceship_collision(spaceship *&ship, const object3d *other) {
        if (ship->is_alive() && ship->collide_with(other)) {
            cout << ship->get_name() << " collides with " << other->get_name() << endl;
            ship->destroy();
        }
    }

    /**
     * Update shootable objects: 2 missile moons
     */
    void update_shootable_objects() {
        for_each(shootable_objects.begin(), shootable_objects.end(), [&](missile_moon *m) {
            if (m->scan_target(apollo) && apollo->is_alive()) {
                if (m->get_name() == "U.Missile") {
                    if (unum_smart_torpedo->is_alive() == false) {
                        unum_smart_torpedo = m->shoot_target(apollo->get_position());
                        unum_smart_torpedo->set_lives(600);
                        // debug
                        cout << m->get_name() << " shoots " << apollo->get_name() << '\n';
                    }
                } else if (m->get_name() == "T.Missile") {
                    if (tres_smart_torpedo->is_alive() == false) {
                        tres_smart_torpedo = m->shoot_target(apollo->get_position());
                        tres_smart_torpedo->set_lives(1200);
                        // debug
                        cout << m->get_name() << " shoots " << apollo->get_name() << '\n';
                    }
                } else {
                    // other missiles
                }
            }
        });
        // update current torpedo that is chasing the ship
        if (unum_smart_torpedo->is_alive()) {
            if (unum_smart_torpedo->get_current_frame() > TRACKING_FRAME) {
                unum_smart_torpedo->track(apollo->get_position());
            }
            unum_smart_torpedo->update();
        }

        if (tres_smart_torpedo->is_alive()) {
            if (tres_smart_torpedo->get_current_frame() > TRACKING_FRAME) {
                tres_smart_torpedo->track(apollo->get_position());
            }
            tres_smart_torpedo->update();
        }

        if (ship_smart_torpedo->is_alive()) {
            int id = apollo->get_current_target_id();
            if (id != -1) {
                ship_smart_torpedo->track(shootable_objects[id]->get_parent_position() + shootable_objects[id]->get_position());
            }
            ship_smart_torpedo->update();
        }
    }

    bool is_game_over() const {
        return game_over;
    }

    /**
     * Handle user interaction from special keys
     */
    void on_special_key(int key, int x, int y) {
        if (is_player_lose() || is_player_win()) {
            return;
        }
        if (glutGetModifiers() == GLUT_ACTIVE_CTRL && key == GLUT_KEY_UP) {
            apollo->turn(spaceship::direction::UP);
        } else if (glutGetModifiers() == GLUT_ACTIVE_CTRL && key == GLUT_KEY_DOWN) {
            apollo->turn(spaceship::direction::DOWN);
        } else if (glutGetModifiers() == GLUT_ACTIVE_CTRL && key == GLUT_KEY_LEFT) {
            apollo->turn(spaceship::direction::BACKWARD);
        } else if (glutGetModifiers() == GLUT_ACTIVE_CTRL && key == GLUT_KEY_RIGHT) {
            apollo->turn(spaceship::direction::FORWARD);
        } else if (glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_UP) {
            led->turn(light_direction::UP);
        } else if (glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_DOWN) {
            led->turn(light_direction::DOWN);
        } else if (glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_LEFT) {
            led->turn(light_direction::LEFT);
        } else if (glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_RIGHT) {
            led->turn(light_direction::RIGHT);
        } else if (key == GLUT_KEY_DOWN) {
            apollo->move_backward();
            for_each(followers.begin(), followers.end(), [&](spaceship *sp) { sp->move_backward(); } );
        } else if (key == GLUT_KEY_UP) {
            apollo->move_forward();
            for_each(followers.begin(), followers.end(), [&](spaceship *sp) { sp->move_forward(); } );
        } else if (key == GLUT_KEY_RIGHT) {
            apollo->turn(spaceship::direction::RIGHT);
        } else if (key == GLUT_KEY_LEFT) {
            apollo->turn(spaceship::direction::LEFT);
        } else {
            // should not get here
        }
        glutPostRedisplay();
    }

    /**
     * Handle user interaction from keyboard
     */
    void on_keyboard(unsigned char key, int x, int y) {
        if (is_player_lose() || is_player_win()) {
            return;
        }
        switch (key) {
            case ' ':
                apollo->toggle_model_or_primitive();
                for_each(followers.begin(), followers.end(), [](spaceship *f) { f->toggle_model_or_primitive(); });
                break;

            case 'q':
                exit(0);
                break;

            case 'g':
                gravity_on = !gravity_on;
                unum_smart_torpedo->set_gravity(gravity_on);
                tres_smart_torpedo->set_gravity(gravity_on);
                ship_smart_torpedo->set_gravity(gravity_on);
                g2v_star->set_gravity_on(gravity_on);
                break;

            case 'v':
                on_planet_camera = false;
                switch_camera();
                break;

            case 'p':
                on_planet_camera = true;
                switch_planet_camera();
                break;

            case 'a':
                led->increase_amb();
                break;

            case 'A':
                led->decrease_amb();
                break;

            case 'd':
                led->increase_diff();
                break;

            case 'D':
                led->decrease_diff();
                break;

            case 'r':
                solid = !solid;
                toggle_wire_solid();
                break;

            case 'f':
                handle_fire_event();
                break;

            case 't':
                toggle_time_quantum();
                break;

            case 'b':
                toggle_bounding_sphere();
                break;

            case 's':
                g2v_star->toggle_texture_mode();
                break;

            case 'e':
                engine->toggle_texture_mode();
                break;

            case 'n':
                debug_on = !debug_on;
                toggle_debug_on();
                break;

            case 'm':
                apollo->adjust_speed();
                for_each(followers.begin(), followers.end(), [&](spaceship *sp) { sp->adjust_speed(); });
                break;

            case 'w':
                warp_ship();
                break;

            case 'z':
                zoom_in();
                break;

            case 'Z':
                zoom_out();
                break;

            case 'x':
                st_idx = (st_idx + 1) % space_textures.size();
                break;
        }
        glutPostRedisplay();
    }

    void zoom_in() {
        view_angle -= 1.0f;
        if (view_angle < 0.0f) {
            view_angle = 360.f;
        }
        set_viewing_volume(viewing_mode::PERSPECTIVE, 1, 1);
    }

    void zoom_out() {
        view_angle += 1.0f;
        if (view_angle > 360.0f) {
            view_angle = 0.0f;
        }
        set_viewing_volume(viewing_mode::PERSPECTIVE, 1, 1);
    }

    void handle_fire_event() {
        // why don't we do this when fire?
        int closest_id = -1;
        double distance_to_moon;
        double closest_distance = oo;
        for (unsigned i = 0; i < shootable_objects.size(); ++i) {
            if (shootable_objects[i]->is_alive()) {
                distance_to_moon = math3d::distance(apollo->get_position(), shootable_objects[i]->get_parent_position() + shootable_objects[i]->get_position());
                if (distance_to_moon < closest_distance) {
                    closest_distance = distance_to_moon;
                    closest_id = i;
                }
            }
        }

        cout << "spaceship shoot at :" << shootable_objects[closest_id]->get_name() << endl;
        apollo->set_current_target_id(closest_id);
        if (ship_smart_torpedo->is_alive() == false) {
            ship_smart_torpedo = apollo->fire();
        }
    }
    /**
     * Handler for menu option
     */
    void on_select_info_menu(int command) {
        switch (command) {
            case information_mode::INFO_INTRODUCTION:
                info_mode = galaxy::information_mode::INFO_INTRODUCTION;
                break;

            case information_mode::INFO_PLANET:
                info_mode = galaxy::information_mode::INFO_PLANET;
                break;

            case information_mode::INFO_CAMERA:
                info_mode = galaxy::information_mode::INFO_CAMERA;
                break;

            case information_mode::INFO_LIGHT:
                info_mode = galaxy::information_mode::INFO_LIGHT;
                break;

            case information_mode::INFO_SPACESHIP:
                info_mode = galaxy::information_mode::INFO_SPACESHIP;
                break;

            case information_mode::INFO_GAME:
                info_mode = galaxy::information_mode::INFO_GAME;
                break;

            case information_mode::INFO_GRAVITY:
                info_mode = galaxy::information_mode::INFO_GRAVITY;
                break;
        }
        glutPostRedisplay();
    }

    void increase_fps() {
        fps++;
    }

    int get_time_quantum() const {
        return TIME_QUANTUM[tq_idx];
    }

private:
    /**
     * Toggle update time
     */
    void toggle_time_quantum() {
        tq_idx = (tq_idx + 1) % 5;
    }

    /**
     * Turn on debug mode, draw axis
     */
    void toggle_debug_on() {
        apollo->set_debug_on(debug_on);
        g2v_star->set_debug_on(debug_on);
        for (auto iter = planets.begin(), end = planets.end(); iter != end; ++iter) {
            (*iter)->set_debug_on(debug_on);
            (*iter)->toggle_moons_debug_on();
        }
    }

    /**
     * Move ship to planets' location
     */
    void warp_ship() {
        planet_index = (planet_index + 1) % 4;
        vector3<float> destination = planets[planet_index]->get_position();
        destination += galaxy_constants::camera_view::moving::planet_cameras[planet_index];
        apollo->teleport(destination);
        apollo->face_down();
        for_each(followers.begin(), followers.end(), [&](spaceship *&sp) {
            sp->set_pitch(apollo->get_pitch());
            sp->set_yaw(apollo->get_yaw());
            sp->set_roll(apollo->get_roll());
        });
        vector3<float> shift_position = apollo->get_absolute_position();
        followers[0]->set_position(vector3<float>(shift_position[0] - 200, shift_position[1], shift_position[2] - 500));
        followers[1]->set_position(vector3<float>(shift_position[0] + 200, shift_position[1], shift_position[2] - 500));
        followers[2]->set_position(vector3<float>(shift_position[0] + 0, shift_position[1], shift_position[2] - 500));
    }

    /**
     * Switch between solid or wire frame for drawing glut object
     */
    void toggle_wire_solid() {
        apollo->set_solid(solid);
        for (auto iter = planets.begin(), end = planets.end(); iter != end; ++iter) {
            (*iter)->set_solid(solid);
            (*iter)->toggle_moons_solid_wire();
        }
    }

    /**
     * Toggle between 4 default cameras
     */
    void switch_camera() {
        camera_index = (camera_index + 1) % 4;
        camera_view_mode = (camera_index == 3) ? MOVING : STATIC;
    }

    /**
     * Toggle between 4 planet cameras
     */
    void switch_planet_camera() {
        planet_camera_index = (planet_camera_index + 1) % 4;
    }

    /**
     * Toggle bounding sphere of all collidable objects
     */
    void toggle_bounding_sphere() {
        bounding_sphere_mode = !bounding_sphere_mode;
        g2v_star->toggle_bounding_sphere(bounding_sphere_mode);
        apollo->toggle_bounding_sphere(bounding_sphere_mode);
        for (unsigned i = 0; i < planets.size(); ++i) {
            planets[i]->toggle_bounding_sphere(bounding_sphere_mode);
            vector<moon*> moons = planets[i]->get_moons();
            for (unsigned j = 0; j < moons.size(); ++j) {
                moons[j]->toggle_bounding_sphere(bounding_sphere_mode);
            }
        }
        if (unum_smart_torpedo->is_alive()) {
            unum_smart_torpedo->toggle_bounding_sphere(bounding_sphere_mode);
        }
        if (tres_smart_torpedo->is_alive()) {
            tres_smart_torpedo->toggle_bounding_sphere(bounding_sphere_mode);
        }
        if (ship_smart_torpedo->is_alive()) {
            ship_smart_torpedo->toggle_bounding_sphere(bounding_sphere_mode);
        }

        for_each(toruses.begin(), toruses.end(), [&](torus *t) { t->toggle_bounding_sphere(bounding_sphere_mode); });
    }

    /**
     * Apply gravity to all moving objects: spaceship, torpedo
     */
    void apply_gravity() {
        pair<vector3<float>, float> gravity;
        if (apollo->is_alive()) {
            gravity = get_gravity_vector_and_force(apollo->get_absolute_position());
            apollo->apply_gravity(gravity.first);
        }
        if (unum_smart_torpedo->is_alive()) {
            gravity = get_gravity_vector_and_force(unum_smart_torpedo->get_position());
            unum_smart_torpedo->apply_gravity(gravity.first);
        }
        if (tres_smart_torpedo->is_alive()) {
            gravity = get_gravity_vector_and_force(tres_smart_torpedo->get_position());
            tres_smart_torpedo->apply_gravity(gravity.first);
        }
        if (ship_smart_torpedo->is_alive()) {
            gravity = get_gravity_vector_and_force(ship_smart_torpedo->get_position());
            ship_smart_torpedo->apply_gravity(gravity.first);
        }
    }


private:

    void draw_galaxy_skybox(int length) {
        float size = 1.0f;
        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, space_textures[st_idx]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glColor3fv(get_color(white));
        glBegin(GL_QUADS); {
            // Negative X
            {
                glTexCoord2f(0, 0);
                glVertex3f(-length, -length, length);
                glTexCoord2f(1, 0);
                glVertex3f(-length, -length, -length);
                glTexCoord2f(1, 1);
                glVertex3f(-length, length, -length);
                glTexCoord2f(0, 1);
                glVertex3f(-length, length, length);
            }

            //  Positive X
            {
                glTexCoord2f(0, 0);
                glVertex3f(length, -length, -length);
                glTexCoord2f(1, 0);
                glVertex3f(length, -length, length);
                glTexCoord2f(1, 1);
                glVertex3f(length, length, length);
                glTexCoord2f(0, 1);
                glVertex3f(length, length, -length);
            }

            // Negative Z
            {
                glTexCoord2f(0, 0);
                glVertex3f(-length, -length, -length);
                glTexCoord2f(1, 0);
                glVertex3f(length, -length, -length);
                glTexCoord2f(1, 1);
                glVertex3f(length, length, -length);
                glTexCoord2f(0, 1);
                glVertex3f(-length, length, -length);
            }

            // Positive Z
            {
                glTexCoord2f(0, 0);
                glVertex3f(length, -length, length);
                glTexCoord2f(1, 0);
                glVertex3f(-length, -length, length);
                glTexCoord2f(1, 1);
                glVertex3f(-length, length, length);
                glTexCoord2f(0, 1);
                glVertex3f(length, length, length);
            }

            // Positive Y
            {
                glTexCoord2f(0, 0);
                glVertex3f(-length, length, length);
                glTexCoord2f(1, 0);
                glVertex3f(-length, length, -length);
                glTexCoord2f(1, 1);
                glVertex3f(length, length, -length);
                glTexCoord2f(0, 1);
                glVertex3f(length, length, length);
            }

            // Negative Y
            {
                glTexCoord2f(0, 0);
                glVertex3f(-length, -length, -length);
                glTexCoord2f(1, 0);
                glVertex3f(-length, -length, length);
                glTexCoord2f(1, 1);
                glVertex3f(length, -length, length);
                glTexCoord2f(0, 1);
                glVertex3f(length, -length, -length);
            }
        } glEnd();
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    }

    void draw_intro_info(int x, int y, int z) const {
        glDisable(GL_LIGHTING);
        glColor3fv(get_color(black));
        int y_offset = 380;

        draw_text("How to play", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("-----------------------", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ v switch camera", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ b toggle bounding sphere", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ t next time quantum", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ g toggle gravity", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ e toggle particle texture", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ n display axis", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ p toggle planet view", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ x change texture", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ z/Z zoom in/out", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ 'space' to toggle ", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("model/primitive", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(" ", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;




        glEnable(GL_LIGHTING);
        glutSwapBuffers();
    }

    void draw_camera_info(int x, int y, int z) const {
        glDisable(GL_LIGHTING);
        glColor3fv(get_color(black));
        int y_offset = 380;
        draw_text("Camera Information", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("-----------------------", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        for_each(cameras.begin(), cameras.end(), [&](const camera *c) {
            draw_text("camera: " + c->get_name(), x, y_offset, z);
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text("+ at: " + c->get_position_string(), x, y_offset, z);
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text("+ lookat: " + c->get_look_at_string(), x, y_offset, z);
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text("+ up: " + c->get_up_vector_string(), x, y_offset, z);
            y_offset -= VERTICAL_TEXT_OFFSET;
        });
        glEnable(GL_LIGHTING);
        glutSwapBuffers();
    }

    void draw_planet_info(int x, int y, int z) const {
        glDisable(GL_LIGHTING);
        glColor3fv(get_color(black));
        int y_offset = 380;
        string fps_str("fps: ");
        fps_str += util::to_string(fps);
        draw_text(fps_str, x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(g2v_star->get_name(), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(g2v_star->get_position_string(), x, y_offset, z);
        for (int i = 0; i < planets.size(); ++i) {
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text(planets[i]->get_name(), x, y_offset, z);

            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text(planets[i]->get_position_string(), x, y_offset, z);

            vector<moon*> moons = planets[i]->get_moons();
            for (int j = 0; j < moons.size(); ++j) {
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text(moons[j]->get_name(), x, y_offset, z);
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text(moons[j]->get_actual_position_string(), x, y_offset, z);
            }
        }
        glEnable(GL_LIGHTING);
        glutSwapBuffers();
    }

    void draw_light_info(int x, int y, int z) const {
        glDisable(GL_LIGHTING);
        glColor3fv(get_color(black));
        int y_offset = 380;
        draw_text("Light Information", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("-----------------------", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("Control light source: ", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ light source = cube", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ move light source", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("shift left/right/up/down", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ a/A for inc/dec ambient", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ d/D for inc/dec diffuse", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(" ", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ ambient", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(led->get_ambient_string(), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ global ambient", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(led->get_global_ambient_string(), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ diffuse", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(led->get_diffuse_string(), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ specular", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(led->get_specular_string(), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ position", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(led->get_position_string(), x, y_offset, z);
        glEnable(GL_LIGHTING);
        glutSwapBuffers();
    }

    void draw_spaceship_info(int x, int y, int z) const {
        glDisable(GL_LIGHTING);
        glColor3fv(get_color(black));
        int y_offset = 380;
        draw_text("Spaceship Information", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("-----------------------", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ name: " + apollo->get_name(), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ pitch/yaw/roll: ", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(apollo->get_pyr_string(), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ locate at:", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(apollo->get_position_string(), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ speed: " + util::to_string(apollo->get_speed()), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ total missiles fired:", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text(util::to_string(apollo->get_shooted_torpedo()), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("Control spaceship: ", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ up/down: forward/backward", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ left/right: turn left/right", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ ctrl left: rotate left", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ ctrl right: rotate right", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ ctrl up: turn up", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ ctrl down: turn down", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ f to fire missiles", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ m toggle speed", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ w warp to planet", x, y_offset, z);
        glEnable(GL_LIGHTING);
        glutSwapBuffers();
    }

    void draw_game_info(int x, int y, int z) const {
        glDisable(GL_LIGHTING);
        glColor3fv(get_color(black));
        int y_offset = 380;
        draw_text("Game Information", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("-----------------------", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("time quantum: " + util::to_string(get_time_quantum()), x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;

        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("unum missile: ",  x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ alive: " + (unum_smart_torpedo->is_alive() ? string("true") : string("false")),  x, y_offset, z);
        if (unum_smart_torpedo->is_alive()) {
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text("+ position: " + util::to_string(unum_smart_torpedo->get_absolute_position()),  x, y_offset, z);
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text("+ lives: " + util::to_string(unum_smart_torpedo->get_lives() - unum_smart_torpedo->get_current_frame()),  x, y_offset, z);
        }

        y_offset -= VERTICAL_TEXT_OFFSET;
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("tres missile: ",  x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ alive: " + (tres_smart_torpedo->is_alive() ? string("true") : string("false")),  x, y_offset, z);
        if (tres_smart_torpedo->is_alive()) {
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text("+ position: " + util::to_string(tres_smart_torpedo->get_absolute_position()),  x, y_offset, z);
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text("+ lives: " + util::to_string(tres_smart_torpedo->get_lives() - tres_smart_torpedo->get_current_frame()),  x, y_offset, z);
        }

        y_offset -= VERTICAL_TEXT_OFFSET;
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("ship missile: ",  x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("+ alive: " + (ship_smart_torpedo->is_alive() ? string("true") : string("false")),  x, y_offset, z);
        if (ship_smart_torpedo->is_alive()) {
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text("+ position: " + util::to_string(ship_smart_torpedo->get_absolute_position()),  x, y_offset, z);
            y_offset -= VERTICAL_TEXT_OFFSET;
            draw_text("+ lives: " + util::to_string(ship_smart_torpedo->get_lives() - ship_smart_torpedo->get_current_frame()),  x, y_offset, z);
        }

        y_offset -= VERTICAL_TEXT_OFFSET;
        y_offset -= VERTICAL_TEXT_OFFSET;

        draw_text("distance from spaceship: ", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        double distance_to_moon = 0.0f;
        if (apollo->is_alive()) {
            for (int i = 0; i < shootable_objects.size(); ++i) {
                distance_to_moon = math3d::distance(apollo->get_position(), shootable_objects[i]->get_parent_position() + shootable_objects[i]->get_position());
                draw_text("+ to: " + shootable_objects[i]->get_name(), x, y_offset, z);
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text(util::to_string(distance_to_moon), x, y_offset, z);
                y_offset -= VERTICAL_TEXT_OFFSET;
            }
        }



        y_offset -= VERTICAL_TEXT_OFFSET;
        y_offset -= VERTICAL_TEXT_OFFSET;
        if (is_player_lose()) {
            draw_text("status: you lose",  x, y_offset, z);
        } else if (is_player_win()) {
            draw_text("status: you win",  x, y_offset, z);
        }

        glEnable(GL_LIGHTING);
        glutSwapBuffers();
    }

    void draw_gravity_info(int x, int y, int z) const {
        glDisable(GL_LIGHTING);
        glColor3fv(get_color(black));
        int y_offset = 380;
        vector3<float> dist;

        draw_text("Gravity Information", x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("-----------------------", x, y_offset, z);

        y_offset -= VERTICAL_TEXT_OFFSET;
        draw_text("G = " + util::to_string(GRAVITY),  x, y_offset, z);
        y_offset -= VERTICAL_TEXT_OFFSET;
        if (gravity_on) {
            pair<vector3<float>, float> gravity;
            draw_text("gravity: on",  x, y_offset, z);
            if (apollo->is_alive()) {
                y_offset -= VERTICAL_TEXT_OFFSET;
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("warbird",  x, y_offset, z);
                gravity = get_gravity_vector_and_force(apollo->get_absolute_position());
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("+ force: " + util::to_string(gravity.second),  x, y_offset, z);
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("+ d(sun): " + util::to_string(apollo->get_position().length()),  x, y_offset, z);
            }
            if (ship_smart_torpedo->is_alive()) {
                y_offset -= VERTICAL_TEXT_OFFSET;
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("ship torpedo",  x, y_offset, z);
                gravity = get_gravity_vector_and_force(ship_smart_torpedo->get_position());
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("+ force: " + util::to_string(gravity.second),  x, y_offset, z);
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("+ d(sun): " + util::to_string(ship_smart_torpedo->get_position().length()),  x, y_offset, z);
            }
            if (unum_smart_torpedo->is_alive()) {
                y_offset -= VERTICAL_TEXT_OFFSET;
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("unum torpedo",  x, y_offset, z);
                gravity = get_gravity_vector_and_force(unum_smart_torpedo->get_position());
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("+ force: " + util::to_string(gravity.second),  x, y_offset, z);
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("+ d(sun): " + util::to_string(unum_smart_torpedo->get_position().length()),  x, y_offset, z);
            }
            if (tres_smart_torpedo->is_alive()) {
                y_offset -= VERTICAL_TEXT_OFFSET;
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("tres torpedo",  x, y_offset, z);
                gravity = get_gravity_vector_and_force(tres_smart_torpedo->get_position());
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("+ force: " + util::to_string(gravity.second),  x, y_offset, z);
                y_offset -= VERTICAL_TEXT_OFFSET;
                draw_text("+ d(sun): " + util::to_string(tres_smart_torpedo->get_position().length()),  x, y_offset, z);
            }
        } else {
            draw_text("gravity: off ",  x, y_offset, z);
        }

        glEnable(GL_LIGHTING);
        glutSwapBuffers();
    }

    void draw_info(int x, int y, int z) const {
        setup_text_window(0.9f, 0.9f, 0.0f, 0.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        switch (info_mode) {
            case information_mode::INFO_INTRODUCTION:
                draw_intro_info(x, y, z);
                break;

            case information_mode::INFO_CAMERA:
                draw_camera_info(x, y, z);
                break;

            case information_mode::INFO_PLANET:
                draw_planet_info(x, y, z);
                break;

            case information_mode::INFO_LIGHT:
                draw_light_info(x, y, z);
                break;

            case information_mode::INFO_SPACESHIP:
                draw_spaceship_info(x, y, z);
                break;

            case information_mode::INFO_GAME:
                draw_game_info(x, y, z);
                break;

            case information_mode::INFO_GRAVITY:
                draw_gravity_info(x, y, z);
                break;
        }
    }

    void draw_text(const string &text, int x, int y, int z) const {
        glRasterPos3i(x, y, z);
        glCallLists(text.length(), GL_BYTE, (char *)text.c_str());
    }

    void write_bitmap_string(void *font, char *str) {
        for (char *c = str; *c != '\0'; c++) {
            glutBitmapCharacter(font, *c);
        }
    }

private:
    void setup_text_window(float r, float g, float b, float a) const {
        static float diffuse[4] = { 1.0, 0.3, 0.3, 1.0 };
        static float ambient[4] = { 0.6, 0.6, 0.6, 1.0 };
        static float position[4] = {-50.0, 50.0, -10.0, 1.0 };  // ambient light
        // clear the color and depth buffers
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        // set light materials
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, position);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
    }

    void setup_lights() {
        using namespace galaxy_constants::lighting;
        led->set_ambient(ambient);
        led->set_diffuse(diffuse);
        led->set_specular(specular);
    }

    void setup_cameras() {
        using namespace galaxy_constants;
        using namespace galaxy_constants::camera_view;
        float planet_up[3] = {1, 0, 0};

        cameras.push_back(new camera("front", front::position, front::lookat, front::up));
        cameras.push_back(new camera("top", top::position, top::lookat, top::up));
        cameras.push_back(new camera("ship", ship::position, ship::lookat, ship::up));
        cameras.push_back(new camera("following", ship::position, ship::lookat, ship::up));

        planet_cameras.push_back(new camera("unum", unum::position, unum::position, planet_up));
        planet_cameras.push_back(new camera("duo", duo::position, duo::position, planet_up));
        planet_cameras.push_back(new camera("tres", tres::position, tres::position, planet_up));
        planet_cameras.push_back(new camera("quattuor", quattuor::position, quattuor::position, planet_up));
    }

    void setup_texture_objects(const unordered_map<string, unsigned>& textures) {
        unsigned sun_tid = textures.find("suntexture.bmp")->second;
        unsigned par_tid = textures.find("circle.bmp")->second;
        g2v_star = new sun(galaxy_constants::helios::name, galaxy_constants::helios::radius, sun_tid);
        // g2v_star->add_affected_objects(apollo);

        engine = new particle_engine("firework", par_tid, 5000.0f, 4.5f);

        space_textures.push_back(textures.find("galaxy0.bmp")->second);
        space_textures.push_back(textures.find("galaxy1.bmp")->second);
        space_textures.push_back(textures.find("galaxy2.bmp")->second);
        space_textures.push_back(textures.find("galaxy3.bmp")->second);
        space_textures.push_back(textures.find("galaxy4.bmp")->second);
        space_textures.push_back(textures.find("galaxy5.bmp")->second);
        space_textures.push_back(textures.find("galaxy6.bmp")->second);
        space_textures.push_back(textures.find("galaxy7.bmp")->second);
        space_textures.push_back(textures.find("galaxy8.bmp")->second);
        space_textures.push_back(textures.find("galaxy9.bmp")->second);
        space_textures.push_back(textures.find("galaxy10.bmp")->second);
        space_textures.push_back(textures.find("messi.bmp")->second);

        st_idx = 0;
        planet_texture_id = textures.find("galaxy0.bmp")->second;
    }

    void setup_planets() {
        using namespace galaxy_constants;
        planet *unum = new planet(unum::name, unum::radius, unum::degree, unum::position, colors::white, planet_texture_id); {
            using namespace unum;
            unum->add(new moon(primun::name, primun::radius, primun::degree, primun::position, colors::brown));
            unum->add(new moon(secundo::name, secundo::radius, secundo::degree, secundo::position, colors::cornflower_blue));
            missile_moon *p = new missile_moon(u_missile::name, u_missile::radius, u_missile::degree, u_missile::position, colors::white, 3000);
            unum->add(p);
            shootable_objects.push_back(p);
        }

        planet *duo = new planet(duo::name, duo::radius, duo::degree, duo::position, colors::white, planet_texture_id); {
            using namespace duo;
        }

        planet *tres = new planet(tres::name, tres::radius, tres::degree, tres::position, colors::white, planet_texture_id); {
            using namespace tres;
            tres->add(new moon(primun::name, primun::radius, primun::degree, primun::position, blue));
            tres->add(new moon(secundo::name, secundo::radius, secundo::degree, secundo::position, colors::magenta));
            tres->add(new moon(tertia::name, tertia::radius, tertia::degree, tertia::position, colors::orange));
            tres->add(new moon(quartum::name, quartum::radius, quartum::degree, quartum::position, colors::cyan));
            // tres->add(new moon(t_missile::name, t_missile::radius, t_missile::degree, t_missile::position, blue));
            // add shooting moon
            missile_moon *p = new missile_moon(t_missile::name, t_missile::radius, t_missile::degree, t_missile::position, colors::green, 5000);
            tres->add(p);
            shootable_objects.push_back(p);
        }

        planet *quattuor = new planet(quattuor::name, quattuor::radius, quattuor::degree, quattuor::position, colors::white, planet_texture_id); {
            using namespace quattuor;
            quattuor->add(new moon(primun::name, primun::radius, primun::degree, primun::position, colors::green));
        }

        planets.push_back(unum);
        planets.push_back(duo);
        planets.push_back(tres);
        planets.push_back(quattuor);
    }

    void setup_oracles() {
        using namespace galaxy_constants;
        oracles.push_back(new oracle(helion::name, helion::radius, helion::degree, helion::position, colors::fire_brick));
    }

    void setup_toruses() {
        using namespace galaxy_constants;
        toruses.push_back(new torus(obstacles::one::name, obstacles::one::radius, 1.0f, obstacles::one::position, colors::cadet_blue));
        toruses.push_back(new torus(obstacles::two::name, obstacles::two::radius, 1.0f, obstacles::two::position, colors::aquamarine));
        toruses.push_back(new torus(obstacles::three::name, obstacles::three::radius, 1.0f, obstacles::three::position, colors::light_wood));
        toruses.push_back(new torus(obstacles::four::name, obstacles::four::radius, 1.0f, obstacles::four::position, colors::magenta));
    }

    void setup_smart_torpedo() {
        // create a current missile
        unum_smart_torpedo = new torpedo();
        tres_smart_torpedo = new torpedo();
        ship_smart_torpedo = new torpedo();
    }

    void setup_spaceship_followers() {
        followers.push_back(
                new spaceship(
                        galaxy_constants::warbird::name,
                        galaxy_constants::warbird::speed,
                        galaxy_constants::warbird::base,
                        galaxy_constants::warbird::height,
                        galaxy_constants::warbird::position[0] + 200,
                        galaxy_constants::warbird::position[1],
                        galaxy_constants::warbird::position[2] - 500,
                        colors::dark_orchid,
                        colors::aquamarine));

        followers.push_back(
                new spaceship(
                        galaxy_constants::warbird::name,
                        galaxy_constants::warbird::speed,
                        galaxy_constants::warbird::base,
                        galaxy_constants::warbird::height,
                        galaxy_constants::warbird::position[0] - 200,
                        galaxy_constants::warbird::position[1],
                        galaxy_constants::warbird::position[2] - 500,
                        colors::dark_orchid,
                        colors::aquamarine));

        followers.push_back(
                new spaceship(
                        galaxy_constants::warbird::name,
                        galaxy_constants::warbird::speed,
                        galaxy_constants::warbird::base,
                        galaxy_constants::warbird::height,
                        galaxy_constants::warbird::position[0],
                        galaxy_constants::warbird::position[1],
                        galaxy_constants::warbird::position[2] - 500,
                        colors::dark_orchid,
                        colors::aquamarine));
    }

private:
    bool game_over;

    /* frame per second */
    int fps;

    /* current display information mode */
    information_mode info_mode;

    /* current camera view mode */
    camera_mode camera_view_mode;

    /* for toggle bounding sphere on/off */
    bool bounding_sphere_mode;

    /* for toggle gravity on/off */
    bool gravity_on;

    /* display axis of each of each if debug is on */
    bool debug_on;

    /* draw wire frame or solid */
    bool solid;

    /* four default camera: front, top, ship, following */
    vector<camera*> cameras;

    /* for toggling between cameras */
    int planet_camera_index;

    /* camera for each planet */
    vector<camera*> planet_cameras;
    /* for toggling between planet's cameras */
    int camera_index;

    /* is camera currently on planet? */
    bool on_planet_camera;

    /* light source for entire scene */
    light *led;

    /* particle engine inside the sun */
    particle_engine *engine;

    /* the sun is located at (0, 0, 0) */
    sun *g2v_star;

    /* spaceship that is controlled by user */
    spaceship* apollo;
    vector<spaceship*> followers;

    /* current missile of both missile sites */
    torpedo *unum_smart_torpedo;
    torpedo *tres_smart_torpedo;
    torpedo *ship_smart_torpedo;

    /* four planets */
    vector<planet*> planets;

    int planet_index;

    /* shootable objects: two moons */
    vector<missile_moon*> shootable_objects;

    /* all torpedo of ship */
    vector<torpedo*> ship_torpedos;

    /* convenient for draw/update */
    vector<object3d*> objects;

    /* special shooting target */
    vector<oracle*> oracles;

    /* experiment different bounding sphere */
    vector<torus*> toruses;

    /* space texture index for toggling */
    vector<unsigned> space_textures;
    int st_idx;

    unsigned planet_texture_id;

    float view_angle;
    /* time quantum index for toggle between different speed */
    int tq_idx;
};

#endif