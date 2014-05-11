#ifndef PROGRAM_H
#define PROGRAM_H

#include "galaxy.h"
#include "texture.h"

#include <map>
#include <utility>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

using namespace std;

namespace driver {
    
    namespace gui_constants {
        // window with and height
        int wnd_width = 800;
        int wnd_height = 600;

        // windows' ids
        int main_wnd_id;
        int game_wnd_id;
        int top_wnd_id;
        int info_wnd_id;

        // timer
        int idle_timer = 1;
        int timer_delay = 40;
        int frame_count = 0;
        int timer_calls = 0;
        int base = 0;
        int fps = 0;
        int sound_timer = 0;
    }

    using namespace gui_constants;

// global
    auto_ptr<galaxy> controller;
    auto_ptr<texture> texture_data;

    void compile_text_list() {
        for (int i = 0; i < 256; i++) {
            glNewList(base + i, GL_COMPILE);
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, i);
            glEndList();
        }
        glListBase(base);
    }

    void redisplay_all_wnd() {
        glutSetWindow(game_wnd_id);
        glutPostRedisplay();
        glutSetWindow(top_wnd_id);
        glutPostRedisplay();
    }

    void draw_main_window() {
        glClearColor(0.5, 0.5, 0.5, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glutSwapBuffers();
    }

    void resize_main_window(int w, int h) {
        if (w != wnd_width && h != wnd_height) {
            glutPositionWindow(50, 50);
            glutReshapeWindow(wnd_width, wnd_height);
        }
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-w/2, w, -h/2, h, -1000, 1000.0);
    }

    void game_window_key_handler(unsigned char key, int x, int y) {
        controller->on_keyboard(key, x, y);
    }

    void game_window_special_key_handler(int key, int x, int y) {
        controller->on_special_key(key, x, y);
    }

    void draw_game_window() {
        glutSetWindow(game_wnd_id);
        controller->draw();
        fps++;
    }

    void resize_game_window(int w, int h) {
        controller->set_viewing_volume(galaxy::viewing_mode::PERSPECTIVE, w, h);
    }

    void draw_top_window() {
        glutSetWindow(top_wnd_id);
        controller->draw_top();
    }

    void resize_top_window(int w, int h) {
        controller->set_viewing_volume(galaxy::viewing_mode::PERSPECTIVE, w, h);
    }

    void draw_info_window() {
        glutSetWindow(info_wnd_id);
        controller->draw_galaxy_info();
    }

    void resize_info_window(int w, int h) {
        controller->set_viewing_volume(galaxy::viewing_mode::ORTHO, w, h);
    }

    void spin() {
        controller->update();
        redisplay_all_wnd();
    }

    void interval_timer(int i) {
        glutTimerFunc(controller->get_time_quantum(), interval_timer, 1);
        // compute frames / second
        timer_calls++;
        if (timer_calls * timer_delay >= 1000) {
            glutSetWindow(info_wnd_id);
            glutPostRedisplay();
            timer_calls = fps = 0;
        }
        spin();
    }

    void select_from_info_menu(int command) {
        controller->on_select_info_menu(command);
    }

    int build_info_popup_menu() {
        int menu;
        menu = glutCreateMenu(select_from_info_menu);
        glutAddMenuEntry("How to Play", galaxy::information_mode::INFO_INTRODUCTION);
        glutAddMenuEntry("Planet Info", galaxy::information_mode::INFO_PLANET);
        glutAddMenuEntry("Camera Info", galaxy::information_mode::INFO_CAMERA);
        glutAddMenuEntry("Light Info", galaxy::information_mode::INFO_LIGHT);
        glutAddMenuEntry("Spaceship Info", galaxy::information_mode::INFO_SPACESHIP);
        glutAddMenuEntry("Game Info", galaxy::information_mode::INFO_GAME);
        glutAddMenuEntry("Gravity Info", galaxy::information_mode::INFO_GRAVITY);
        return menu;
    }

    void setup_windows() {
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowSize(wnd_width, wnd_height);
        main_wnd_id = glutCreateWindow("Romulan Imperial War College");
        glutPositionWindow(50, 50);
        glutReshapeFunc(resize_main_window);
        glutDisplayFunc(draw_main_window);

        // make game window
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        game_wnd_id = glutCreateSubWindow(main_wnd_id, 10, 10, 580, 580);
        glutReshapeFunc(resize_game_window);
        glutDisplayFunc(draw_game_window);
        glutKeyboardFunc(game_window_key_handler);
        glutSpecialFunc(game_window_special_key_handler);

        glutTimerFunc(timer_delay, interval_timer, 1);

        // make top window
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        top_wnd_id = glutCreateSubWindow(main_wnd_id, 605, 0, 200, 200);
        glutReshapeFunc(resize_top_window);
        glutDisplayFunc(draw_top_window);

        // make info window
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        info_wnd_id = glutCreateSubWindow(main_wnd_id, 605, 200, 200, 400);
        glutReshapeFunc(resize_info_window);
        glutDisplayFunc(draw_info_window);
        build_info_popup_menu();
        glutAttachMenu(GLUT_RIGHT_BUTTON);

        // must initialize here? why?
        compile_text_list();
        // we start with game window
        glutSetWindow(game_wnd_id);
    }

    unordered_map<string, unsigned> load_all_textures() {
        vector<texture::texture_type> types;
        vector<pair<string, string> > filenames;

        string sun_texture = "suntexture.bmp";
        filenames.push_back(make_pair(sun_texture, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture0 = "galaxy0.bmp";
        filenames.push_back(make_pair(space_texture0, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture1 = "galaxy1.bmp";
        filenames.push_back(make_pair(space_texture1, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture2 = "galaxy2.bmp";
        filenames.push_back(make_pair(space_texture2, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture3 = "galaxy3.bmp";
        filenames.push_back(make_pair(space_texture3, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture4 = "galaxy4.bmp";
        filenames.push_back(make_pair(space_texture4, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture5 = "galaxy5.bmp";
        filenames.push_back(make_pair(space_texture5, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture6 = "galaxy6.bmp";
        filenames.push_back(make_pair(space_texture6, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture7 = "galaxy7.bmp";
        filenames.push_back(make_pair(space_texture7, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture8 = "galaxy8.bmp";
        filenames.push_back(make_pair(space_texture8, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture9 = "galaxy9.bmp";
        filenames.push_back(make_pair(space_texture9, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture10 = "galaxy10.bmp";
        filenames.push_back(make_pair(space_texture10, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string space_texture11 = "messi.bmp";
        filenames.push_back(make_pair(space_texture11, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string planet_texture = "galaxy0.bmp";
        filenames.push_back(make_pair(planet_texture, " "));
        types.push_back(texture::texture_type::TEXTURE_REGULAR);

        string particle_circle_texture = "circle.bmp";
        string particle_circle_alpha = "circlealpha.bmp";
        filenames.push_back(make_pair(particle_circle_texture, particle_circle_alpha));
        types.push_back(texture::texture_type::TEXTURE_WITH_ALPHA);

        texture_data = auto_ptr<texture>(new texture());
        texture_data->load_all(filenames, types);
        return texture_data->get_textures_hashmap();
    }

    void run(int argc, char **argv) {
        glutInit(&argc, argv);
        setup_windows();
        controller = auto_ptr<galaxy>(new galaxy(load_all_textures()));
        controller->generate_models();
        glutMainLoop();
    }
}
#endif