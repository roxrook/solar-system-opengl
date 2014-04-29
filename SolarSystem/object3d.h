#ifndef __SOLAR_SYSTEM_OBJECT3D_H
#define __SOLAR_SYSTEM_OBJECT3D_H

#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "vector3.h"
#include "math3d.h"
#include "colors.h"

using namespace std;
using namespace util;

/** 
 * This is object base class for all
 * concrete component using OpenGL
 */
class object3d {

public:
	static const int SPACING = 12;
	static const int Rx = 0;
	static const int Ry = 1;
	static const int Rz = 2;

	static const int Ux = 4;
	static const int Uy = 5;
	static const int Uz = 6;

	static const int Ax = 8;
	static const int Ay = 9;
	static const int Az = 10;

	static const int Tx = 12;
	static const int Ty = 13;
	static const int Tz = 14;

	static const int AXIS_LENGTH = 5000;

public:
	virtual ~object3d() {

	}

	object3d(const string &name = "object3d", float bd_radius = 0.0f, bool collidable = false):
		name(name), 
		bounding_sphere_radius(bd_radius), 
		collidable(collidable), 
		debug_on(false),
		solid(true),
		rotate_about(false),
		bounding_sphere(false),
		pitch(0.0f), yaw(0.0f), roll(0.0f) {

		initialize(OM);
		for (int i = 0; i < 3; ++i) {
			position[i] = 0;
		}
	}

	object3d(const string &name, float bd_radius, bool collidable, const vector3<float> &position, const vector3<float> &forward):
		name(name), 
		bounding_sphere_radius(bd_radius), 
		collidable(collidable), 
		bounding_sphere(false),
		position(position), 
		debug_on(false),
		solid(true),
		rotate_about(false),
		forward(forward),
		pitch(0.0f), yaw(0.0f), roll(0.0f) {

		initialize(OM);

		MV[Tx] = position.get_x();
		MV[Ty] = position.get_y();
		MV[Tz] = position.get_z();

		OM[Tx] = position.get_x();
		OM[Ty] = position.get_y();
		OM[Tz] = position.get_z();
	}

	/**
	 * Test collision using simple bounding sphere.
	 * More complicated object should override this method 
	 * for more accurate testing.
	 */
	virtual bool collide_with(const object3d *obj) const {
		vector3<float> my = get_position();
		vector3<float> yours = obj->get_position();
		if (util::math3d::distance(my, yours) <= (bounding_sphere_radius + obj->get_bounding_sphere_radius())) {
			return true;
		} else {
			return false;
		}
	}

	bool is_collidable() const {
		return collidable;
	}

	void set_collidable(bool flag) {
		collidable = flag; 
	}

	void set_bounding_sphere_radius(float radius) {
		bounding_sphere_radius = radius;
	}

	float get_bounding_sphere_radius() const {
		return bounding_sphere_radius;
	}

	virtual void draw_bounding_sphere() {
		glPushMatrix(); 
        glColor3fv(get_color(colors::white));
        glutWireSphere(bounding_sphere_radius, 40, 17);
        glPopMatrix();
	}

	void update_internal() {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix(); {
			// make the current identity
			glLoadIdentity();
			// perform rotation and translation
			if (rotate_about == false) { 
				glTranslatef(position.get_x(), position.get_y(), position.get_z());
				glRotatef(roll, 0, 0, 1);
				glRotatef(yaw, 0, 1, 0);
				glRotatef(pitch, 1, 0, 0);
			} else {
				glRotatef(roll, 0, 0, 1);
				glRotatef(yaw, 0, 1, 0);
				glRotatef(pitch, 1, 0, 0);
				glTranslatef(position.get_x(), position.get_y(), position.get_z());
			}
			// save the actual MV without affecting camera 
			glGetFloatv(GL_MODELVIEW_MATRIX, OM);
		} glPopMatrix();
	}

	void update_smart_movement(const vector3<float> &target) {
		vector3<float> pos = position;
		vector3<float> axis = pos.cross(target);
		axis.normalize();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix(); {
			glLoadIdentity();
			glRotatef(1.0f, axis.get_x(), axis.get_y(), axis.get_z());
			glTranslatef(position.get_x(), position.get_y(), position.get_z());
			glGetFloatv(GL_MODELVIEW_MATRIX, OM);
		} glPopMatrix();
	}

	void draw_axes() {
		glLineWidth(1);
		glBegin(GL_LINES); {
			// draw x
			glColor3fv(get_color(colors::color_name::red));
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(AXIS_LENGTH, 0.0, 0.0);

			// draw y
			glColor3fv(get_color(colors::color_name::green));
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, AXIS_LENGTH, 0.0);

			// draw z
			glColor3fv(get_color(colors::color_name::blue));
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, AXIS_LENGTH);
		} glEnd();
	}

	string get_name() const {
		return name;
	}

	void set_name(const string &n) {
		name = n;	
	}

	string get_position_string() const {
		ostringstream ostr;
		ostr << OM[Tx] << " " << OM[Ty] << " " << OM[Tz];
		return ostr.str(); 
	}

	string get_actual_position_string() const {
		ostringstream ostr;
		ostr << OM[Tx] << " " << OM[Ty] << " " << OM[Tz];
		return ostr.str(); 
	}

	ostream& modelview(ostream& out) const {
		out << left;
		out << "\tMODELVIEW MATRIX of " << name << endl;
		out << "--------------------------------------------------" << endl;
		out << setw(SPACING) << "R" << setw(SPACING) << "U" << setw(SPACING) << "A" << setw(SPACING) << "T" << endl;	
		out << "--------------------------------------------------" << endl;
		out << setw(SPACING) << MV[0] << setw(SPACING) << MV[4] << setw(SPACING) << MV[8]  << setw(SPACING) << MV[12] << endl;
		out << setw(SPACING) << MV[1] << setw(SPACING) << MV[5] << setw(SPACING) << MV[9]  << setw(SPACING) << MV[13] << endl;
		out << setw(SPACING) << MV[2] << setw(SPACING) << MV[6] << setw(SPACING) << MV[10] << setw(SPACING) << MV[14] << endl;
		out << setw(SPACING) << MV[3] << setw(SPACING) << MV[7] << setw(SPACING) << MV[11] << setw(SPACING) << MV[15] << endl;
		out << "--------------------------------------------------" << endl;
		out << endl;
		return out;
	}

	void draw_vector(vector3<float> &v) {
		v.normalize();
		glBegin(GL_LINES); {
			glColor3fv(get_color(colors::color_name::feldspar));
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(10000 * v.get_x(), 10000 * v.get_y(), 10000 * v.get_z());
		} glEnd();
	}

	/** 
	 * Correct 
	 */
	vector3<float> get_forward() const {
		return vector3<float>(OM[Ax], OM[Ay], OM[Az]);
	}

	/** 
	 * Correct 
	 */
	vector3<float> get_position() const {
		return vector3<float>(OM[Tx], OM[Ty], OM[Tz]);
	}

	vector3<float> get_absolute_position() const {
		return position;
	}

	/** 
	 * Correct 
	 */
	vector3<float> get_up() const {
		return vector3<float>(OM[Ux], OM[Uy], OM[Uz]);
	}

	void set_position(const vector3<float> &p) {
		position = p;
	}

	void set_up_vector(const vector3<float> &u) {
		OM[Ux] = u.get_x();
		OM[Uy] = u.get_y();
		OM[Uz] = u.get_z();
	}

	void set_forward_vector(const vector3<float> &f) {
		OM[Ax] = f.get_x();
		OM[Ay] = f.get_y();
		OM[Az] = f.get_z();
	}

	/** 
	 * Return the actual position of object
	 */
	vector3<float> get_actual_position_vector() const {
		return vector3<float>(OM[Tx], OM[Ty], OM[Tz]);
	}

	bool is_bounding_on() const {
		return bounding_sphere;
	}

	void toggle_bounding_sphere(bool flag) {
		bounding_sphere = flag;
	}

	void draw() {
		if (collidable && bounding_sphere) {
			draw_bounding_sphere();
		}
		if (debug_on) {
			draw_axes();
		}
	}
	
	void apply_gravity(const vector3<float> &gravity) {
		vector3<float> forward = get_forward();
		position += (forward + gravity);
	}

	void set_solid(bool value) {
		solid = value;
	}

	bool is_solid() const {
		return solid;
	}

	void set_debug_on(bool value) {
		debug_on = value;
	}

	bool is_debug_on() const {
		return debug_on;
	}

	void set_pitch(float p) {
		pitch = p;
	}

	void set_yaw(float y) {
		yaw = y;
	}

	void set_roll(float r) {
		roll = r;
	}

	float get_pitch() const {
		return pitch;
	}

	float get_yaw() const {
		return yaw;
	}

	float get_roll() const {
		return roll;
	}

private:
	void initialize(float M[]) {
		for (int i = 0; i < 16; ++i) {
			M[i] = 0;
		}
		M[0] = M[5] = M[10] = M[15] = 1.0; 
	}

protected:
	string name;	
	bool collidable;
	bool bounding_sphere;
	float bounding_sphere_radius;
	bool solid;
	bool debug_on;

	float OM[16]; // orientation matrix 
	float MV[16]; // modelview matrix

	vector3<float> position;
	vector3<float> forward;

	float pitch;
	float yaw;
	float roll;
	bool rotate_about;
};

#endif