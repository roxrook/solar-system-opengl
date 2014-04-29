#ifndef __SOLAR_SYSTEM_MISSILE_H
#define __SOLAR_SYSTEM_MISSILE_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <deque>
#include <random>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "drawable.h"
#include "movable.h"
#include "object3d.h"
#include "colors.h"

using namespace std;
using namespace util;
using namespace colors;

struct missile_type  {
	enum type  {
		AGM_84_HARPOON = 0,
		AIM_4_FALCON = 1
	};

	missile_type(type t) : t(t) {
	}

	missile_type() {
		t = AIM_4_FALCON;
	}

	operator type() const { 
		return t;
	}

	type t;
private:
	// prevent automatic conversion for any other 
	// built-in types such as bool, int, etc
	template<typename T>
	operator T () const;
};

class missile:
	public object3d,
	public drawable,
	public movable {

	static const int MAX_LIVES = 5000;

public:
	missile(const vector3<float> &position, const vector3<float> &velocity):
		object3d("z-52", 2000, true, position, velocity),
		radius(50),
		alive(true), 
		counter(0), 
		speed(200),
		color(colors::red),
		velocity(velocity) {
	}

	missile(const vector3<float> &position, const vector3<float> &velocity, const color_name &color, int speed, const missile_type &type):
		object3d("z-52", 2000, true, position, velocity),
		radius(50),
		alive(true), 
		counter(0), 
		speed(speed),
		color(color),
		velocity(velocity) {

		mtype = type;
	}

	void draw() {
		glPushMatrix(); {
			// update
			glMultMatrixf(OM);
			// actual rendering
			draw_itself(mtype);
			// draw axis or bounding sphere
			object3d::draw();
		} glPopMatrix();
	}


	void draw_itself(const missile_type &missile) {
		glColor3fv(get_color(color));
		switch (missile.t) {
			case missile_type::AGM_84_HARPOON:
				glutSolidCube(radius);
				break;

			case missile_type::AIM_4_FALCON:
				glutSolidSphere(radius, 40, 20);
				break;
		}
	}

	void move(int step_size) {
		position += (velocity * step_size);
	}

	void turn(float amount) {
		yaw += amount;
		if (yaw >= 360.f) {
			yaw = 0.0f;
		}
	}

	void update() {
		move(speed);
		turn(30.0f);
		counter++;
		if (counter > MAX_LIVES) {
			alive = false;
			counter = 0;
		}
		update_internal();
	}

	bool is_alive() const {
		return alive;
	}

private:
	float radius;
	bool alive;
	color_name color;
	int speed;
	int counter;
	missile_type mtype;
	vector3<float> velocity;
};

#endif
