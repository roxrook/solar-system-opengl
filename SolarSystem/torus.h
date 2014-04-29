#ifndef TORUS_H
#define TORUS_H

#include <vector>
#include <string>
#include <algorithm>

#include "object3d.h"
#include "colors.h"
#include "movable.h"
#include "collidable.h"
#include "math3d.h"
#include "vector3.h"
#import "drawable.h"

using namespace std;
using namespace colors;
using namespace util;

class torus:	
	public object3d, 
	public movable,
	public drawable {

public:
	static const int OFFSET = 1000;

public:
	torus(const string &name = "torus", float radius = 1.0f, float degree = 1.0f, float p[3] = NULL, const color_name& c = green): 
		object3d(name, radius, false, vector3<float>(p[0], p[1], p[2]), vector3<float>(0, 1, 0)),  
		radius(radius), 
		degree(degree),  
		color(c) {
		for (int i = 0; i < 3; ++i) {
			position[i] = p[i];
		}
		bounding_sphere_radius = radius + 10;
	}

	~torus() {
	}

	void draw_bounding_sphere() {
		double center = OFFSET + radius;
		double diamater = (OFFSET + radius)*2;
		int no_sphere = (diamater/radius) * 4;	
		double angle = 0.0;
		for (int i = 0; i < no_sphere; ++i) {
			glPushMatrix(); {
				glColor3fv(get_color(colors::white));
				glTranslatef(center*cos(angle), center*sin(angle), 0);
				glutWireSphere(bounding_sphere_radius, 40, 20);
			} glPopMatrix();
			angle += (2 * util::constants::PI) / no_sphere;
		}
	}

	bool collide_with(const object3d *other) const {
		double outer_radius = OFFSET + radius;
		double diamater = (OFFSET + radius)*2;
		int no_sphere = (diamater/radius) * 4;	
		double angle = 0.0;
		vector3<float> center;
		for (int i = 0; i < no_sphere; ++i) {
			center = get_position() + vector3<float>(outer_radius*cos(angle), outer_radius*sin(angle), 0.0f);
			if (math3d::distance(center, other->get_position()) < (bounding_sphere_radius + other->get_bounding_sphere_radius())) {
				return true;
			}
			angle += (2 * util::constants::PI) / no_sphere;
		}
		return false;
	}

	void draw() {
		glPushMatrix(); {
			// update
			glMultMatrixf(OM);
			// actual rendering	
			draw_itself();
			// draw bounding sphere
			if (bounding_sphere) {
				draw_bounding_sphere();
			}
			// draw axis or bounding sphere
			object3d::draw();
		} glPopMatrix();
	}

	void draw_itself() {
		glColor3fv(get_color(color));
		if (is_solid()) {
			glutSolidSphere(100, 40, 20);
			glutSolidTorus(radius, radius + OFFSET, 40, 20);
		} else {
			glutWireSphere(100, 40, 20);
			glutWireTorus(radius, radius + OFFSET, 40, 20);
		}
	}

	void update() {
		object3d::update_internal();
	}

private:
	float radius;
	float degree;
	color_name color;
};

#endif