#ifndef __SOLAR_SYSTEM_TRIANGLE_LOADER_H
#define __SOLAR_SYSTEM_TRIANGLE_LOADER_H

#include <math.h>
#include <stdio.h>

namespace util {

// 3 vertices of triangle
    float v[3][3];
// 3 vertices of triangle
    float normal[3][3];

    const int X = 0;
    const int Y = 1;
    const int Z = 2;

    void cross_product(float v1[3], float v2[3], float n[3]) {
        n[X] = v1[Y] * v2[Z] - v1[Z] * v2[Y];
        n[Y] = v1[Z] * v2[X] - v1[X] * v2[Z];
        n[Z] = v1[X] * v2[Y] - v1[Y] * v2[X];
    }

/**
 *	Compute 3 unit normals for the 3 vertices in v
 *		normal[0] = cross(vectorFrom v[0] to v[1], vectorFrom v[2] to v[0])
 *		normal[1] = cross(vectorFrom v[1] to v[2], vectorFrom v[0] to v[1])
 *		normal[2] = cross(vectorFrom v[2] to v[0], vectorFrom v[2] to v[0])
 *	This program only requires 1 normal per surface.
*/
    void compute_normals() {
        // 3 clockwise vectors of surface
        float vec[3][3];
        float length, sOs;

        // get 3 clockwise vectors for surface
        for (int i = 0; i < 3; i++) vec[0][i] = v[1][i] - v[0][i];
        for (int i = 0; i < 3; i++) vec[1][i] = v[2][i] - v[1][i];
        for (int i = 0; i < 3; i++) vec[2][i] = v[0][i] - v[2][i];

        // compute normals
        cross_product(vec[2], vec[0], normal[0]);
        cross_product(vec[0], vec[1], normal[1]);
        cross_product(vec[1], vec[2], normal[2]);

        // convert to unit normal
        for (int n = 0; n < 3; n++) {
            sOs = 0.0;  // sOs is sum of squares
            for (int i = 0; i < 3; i++) sOs += normal[n][i] * normal[n][i];
            length = sqrt(sOs);
            for (int i = 0; i < 3; i++) normal[n][i] = normal[n][i] / length;
        }
    }

    void run() {
        FILE *model;
        unsigned int c;
        unsigned int red, green, blue;
        bool done = false;
        model = fopen("special_model.h", "w");
        // open output file for writing and write "header"
        fprintf(model, "#ifndef SPECIAL_MODEL_H\n");
        fprintf(model, "#define SPECIAL_MODEL_H\n");
        fprintf(model, " \n");
        fprintf(model, "#include <GL/glut.h>\n");
        fprintf(model, "#include \"opengl_macro.h\"\n");
        fprintf(model, "namespace special_model {\n");
        fprintf(model, " \n");
        fprintf(model, "void generate_spaceship_model(int list_id, float scale) {\n");
        fprintf(model, "	gl_enable(GL_NORMALIZE);\n");
        fprintf(model, "	gl_new_list(list_id, GL_COMPILE);\n");
        fprintf(model, "	gl_scale_f(scale, scale, scale);\n");
        fprintf(model, "	gl_begin(GL_TRIANGLES);\n");
        //read input from stdin.
        while (!feof(stdin)) {
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++) {
                    v[i][j] = -99999;
                    if (fscanf(stdin, "%f", &v[i][j]) == EOF) {
                        // at end of file
                        done = true;
                        break;
                    }
                }
            if (!done) {  // compute values
                // read and process color values
                fscanf(stdin, "%x", &c);
                red = c;
                green = c;
                blue = c;
                red = red >> 16;
                green = green << 16;
                green = green >> 24;
                blue = blue << 24;
                blue = blue >> 24;


                fprintf(model, "	gl_color_3f(%9.4f, %9.4f, %9.4f);\n", red / 255.0, green / 255.0, blue / 255.0);
                compute_normals();  // computes 3 normals -- only use the first one.
                fprintf(model, "	gl_normal_3f(");
                fprintf(model, "    %9.4f, %9.4f, %9.4f);\n", normal[0][X], normal[0][Y], normal[0][Z]);
                for (int i = 0; i < 3; i++) {
                    fprintf(model, "	gl_vertex_3f(");
                    fprintf(model, "   %9.4f, %9.4f, %9.4f);\n", v[i][X], v[i][Y], v[i][Z]);
                }
            }
        }
        fprintf(model, "	gl_end();\n");
        fprintf(model, "	gl_end_list();\n");
        fprintf(model, "}\n");
        fprintf(model, "}\n");
        fprintf(model, "\n");
        fprintf(model, "#endif");
        fclose(model);
    }
}

#endif