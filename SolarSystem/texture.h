#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "image.h"

using namespace std;
using namespace util;

/// TODO: Need to release memory for texture
class texture {
public:
    enum texture_type {
        TEXTURE_REGULAR,
        TEXTURE_WITH_ALPHA
    };

public:
    texture():
    ready(false) {
    }

    void load_all(const vector<pair<string, string> > &filenames, const vector<texture_type> &types) {
        assert(filenames.size() == types.size());
        if (filenames.size() == 0) {
            return;
        }

        vector<pair<image*, image*> > images;
        for (unsigned i = 0, size = types.size(); i < size; ++i) {
            if (types[i] == TEXTURE_REGULAR) {
                images.push_back(make_pair<image*, image*>(load_bmp(filenames[i].first.c_str()), NULL));
            } else {
                images.push_back(make_pair<image*, image*>(
                        load_bmp(filenames[i].first.c_str()),
                        load_bmp(filenames[i].second.c_str())
                ));
            }
        }
        load_multiple_textures_from_image(images, filenames, types);
        cleanup(images);
        ready = true;
    }

    bool is_data_ready() const {
        return ready;
    }

    unordered_map<string, unsigned> get_textures_hashmap() const {
        assert(ready == true);
        return hm;
    }

private:
    void load_multiple_textures_from_image(vector<pair<image*, image*> >& images,
            const vector<pair<string, string> > &filenames, const vector<texture_type> &types) {
        unsigned no_textures = filenames.size();
        /// TODO: must release these ids
        unsigned *texture_ids = new unsigned[no_textures];
        // generate textures for all bmp files
        glGenTextures(no_textures, texture_ids);
        for (unsigned i = 0; i < no_textures; ++i) {
            glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
            if (types[i] == TEXTURE_REGULAR) {
                glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        GL_RGB,
                        images[i].first->width, images[i].first->height,
                        0,
                        GL_RGB,
                        GL_UNSIGNED_BYTE,
                        images[i].first->pixels);
            } else {
                char *pixels = util::add_alpha_channel(images[i].first, images[i].second);
                glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        GL_RGBA,
                        images[i].first->width, images[i].second->height,
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        pixels);
                delete pixels;
            }
            // insert into hash-map for later look up
            // we choose the first file name as key
            hm.insert(make_pair(filenames[i].first, texture_ids[i]));
        }
        glDeleteTextures(no_textures, (GLuint* )&texture_ids);
        delete []texture_ids;
    }

    void cleanup(vector<pair<image*, image*> >& images) {
        for_each(images.begin(), images.end(), [&](pair<image*, image* >& p) {
            delete p.first;
            p.first = NULL;
            delete p.second;
            p.second = NULL;
        });
    }

private:
    bool ready;
    unordered_map<string, unsigned> hm;
};
#endif