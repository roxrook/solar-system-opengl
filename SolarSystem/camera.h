#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <sstream>

#include "object3d.h"
#include "vector3.h"
#include "drawable.h"

using namespace std;
using namespace util;

/** 
 * The camera for the whole scene based
 * on glut_look_at()
 * There are two types of camera:
 *		1) For static camera, the "at", "look_at", "up" are
 *		set at the beginning and can't be changed.
 *		2) For following camera, these values only 
 *		be set once the location of the moving object is
 *		obtained.
 */
class camera:
	public object3d {

public:
	camera():
		object3d("camera") {
		for (int i = 0; i < 3; ++i) {
			eye_position[i] = 0.0f;
			look_at[i] = 0.0f;
			up_vector[i] = 0.0f;
		}
	}

	camera(const string &name, float p[3], float l[3], float u[3]):
		object3d(name) {
		for (int i = 0; i < 3; ++i) {
			eye_position[i] = p[i];
			look_at[i] = l[i];
			up_vector[i] = u[i];
		}
	}

	camera(const string &name, const vector3<float> &p, const vector3<float> &l, const vector3<float> &u):
		object3d(name) {
		for (int i = 0; i < 3; ++i) {
			eye_position[i] = p[i];
			look_at[i] = l[i];
			up_vector[i] = u[i];
		}
	}

	void capture() const {
		gluLookAt(
			eye_position[0], eye_position[1], eye_position[2],
			look_at[0], look_at[1], look_at[2],
			up_vector[0], up_vector[1], up_vector[2]
		);
	}

	void apply_object3d_orientation(const object3d *obj, const vector3<float> &offset) {
		glTranslatef(offset.get_x(), offset.get_y(), offset.get_z());
		eye_position = obj->get_position();
		look_at = obj->get_position() + obj->get_forward();
		up_vector = obj->get_up();
	}

	void apply_planet_orientation(const object3d *obj, const vector3<float> &offset) {
		eye_position = obj->get_position() + offset;
		look_at = obj->get_position();
		up_vector = obj->get_forward();
	}

	void set_eye_position(float x, float y, float z) {
		eye_position[0] = x;
		eye_position[1] = y;
		eye_position[2] = z;
	}

	void set_eye_position(float p[3]) {
		eye_position[0] = p[0];
		eye_position[1] = p[1]; 
		eye_position[2] = p[2];
	}

	void set_eye_position(const vector3<float> &v) {
		eye_position[0] = v.get_x();
		eye_position[1] = v.get_y();
		eye_position[2] = v.get_z();
	}

	void set_look_at(float x, float y, float z) {
		look_at[0] = x;
		look_at[1] = y;
		look_at[2] = z;
	}

	void set_look_at(const vector3<float> &v) {
		look_at[0] = v.get_x();
		look_at[1] = v.get_y();
		look_at[2] = v.get_z();
	}

	void set_look_at(float at[3]) {
		look_at[0] = at[0];
		look_at[1] = at[1];
		look_at[2] = at[2];
	}

	void set_up_vector(float up_vector[3]) {
		up_vector[0] = up_vector[0];
		up_vector[1] = up_vector[1];
		up_vector[2] = up_vector[2];
	}

	void set_up_vector(float x, float y, float z) {
		up_vector[0] = x;
		up_vector[1] = y;
		up_vector[2] = z;
	}

	void set_up_vector(const vector3<float> &v) {
		up_vector[0] = v.get_x();
		up_vector[1] = v.get_y();
		up_vector[2] = v.get_z();
	}

	string get_look_at_string() const {
		return float_to_string(look_at);
	}

	string get_up_vector_string() const {
		return float_to_string(up_vector);
	}

	string get_eye_position_string() const {
		return float_to_string(eye_position);
	}

private:
	string float_to_string(const vector3<float> &vec) const {
		ostringstream ostr;
		ostr << vec[0] << " " << vec[1] << " " << vec[2];
		return ostr.str();
	}

private:
	vector3<float> eye_position;
	vector3<float> look_at;
	vector3<float> up_vector;
};

#endif