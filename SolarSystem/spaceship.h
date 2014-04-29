#ifndef __SOLAR_SYSTEM_SPACESHIP_H
#define __SOLAR_SYSTEM_SPACESHIP_H

#include <cstdio>
#include <cstdlib>
#include <string>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include "object3d.h"
#include "colors.h"
#include "movable.h"
#include "drawable.h"
#include "vector3.h"
#include "missile.h"
#include "torpedo.h"

using namespace std;
using namespace util;

class spaceship:	
	public object3d,
	public movable,
	public drawable {

public:
	enum direction {
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		FORWARD = 4,
		BACKWARD = 5
	};

	static const int MAX_MISSILES = 20;

public:
	spaceship() {
		alive = true;
		bounding_sphere = false;
	}

	spaceship(const string &name, float speed, double base, double height, float x, float y, float z, const color_name &base_color, const color_name &top_color):
		object3d(name, 0.0f, true, vector3<float>(x, y, z),  vector3<float>(0, 1, 0)) {
		initialize(speed, 2.0f, base, height, base_color, top_color);
	}

	spaceship(const string &name, float speed, double base, double height, float pos[3], const color_name &base_color, const color_name &top_color): 
		object3d(name, 0.0f, true, vector3<float>(pos[0], pos[1], pos[2]),  vector3<float>(0, 1, 0)) {
		initialize(speed, 2.0f, base, height, base_color, top_color);
	}

	spaceship(const string &name, float speed, double base, double height, const vector3<float> &pos, const color_name &base_color, const color_name &top_color): 
		object3d(name, 0.0f, true, pos, vector3<float>(0, 1, 0)) {
		initialize(speed, 1.0f, base, height, base_color, top_color); 
	} 

	void initialize(float speed, float turn_amount, double base, double height, const color_name &base_color, const color_name &top_color) {
		this->speed = speed;
		this->turn_amount = turn_amount;
		this->base = base;
		this->height = height;
		this->base_color = base_color;
		this->top_color = top_color;

		no_torpedo = 0;
		moving = false;
		alive = true;
		explosion_count_down = 50;
		bounding_sphere_radius = 130;
		explosion_radius = 1000;
		no_lives = 5;
		list_id = 1;
		target = vector3<float>(0.0f, 0.0f, 0.0f);
		torpe = NULL;
		model_or_primitive = true;
	}

	void adjust_speed() {
		if (speed >= 200.0f) {
			speed = 50.0f;
		} else {
			speed += 10.0f;
		}
	}

	void draw() {
		glPushMatrix(); {
			// update
			glMultMatrixf(OM);
			// actual drawing
			if (alive) {
				if (model_or_primitive) {
					draw_model();
				} else {
					draw_spaceship();
				}
			} else {
				draw_explosion();
			}
		}
        glPopMatrix();
		// reset moving
		moving = false;
	}
	
	/** 
	 * Move the ship to a new location "destination" 
	 */
	void teleport(const vector3<float>& destination) {
		position = destination;
	}

	/** 
	 * Turn to face target
	 */
	void turn_to_face(const vector3<float> &target) {
		set_yaw(180.0);
		set_pitch(90.0f);
		set_roll(0.0f);
	}

	/** 
	 * Face down the -y axis
	 */
	void face_down() {
		set_yaw(180.0);
		set_pitch(90.0f);
		set_roll(0.0f);
	}

	void update() {
		object3d::update_internal();
	}

	/** 
	 * Update closes target 
	 */
	void track(const vector3<float> &closest_target) {
		target = closest_target;
	}

	/** 
	 * Fire the closest target, and return a torpedo 
	 * for galaxy to handle
	 */
	torpedo* fire() {
		if (torpe == NULL || torpe->is_alive() == false) {
			no_torpedo++;
			if (no_torpedo < MAX_MISSILES) {
				if (torpe == NULL) {
					torpe = new torpedo("torpedo", get_position(), get_position() + (get_forward() + 100), colors::red, torpedo_type::AIM_4_FALCON, 20, 1000);
					return torpe;
				} else {
					torpe->set_new_position(get_position());
					torpe->set_new_target(get_position() + (get_forward() * 100));
					torpe->reborn();
					return torpe;
				}
			}
		}
		return NULL;
	}

	void toggle_bounding_sphere(bool flag) {
		object3d::toggle_bounding_sphere(flag);
	}

	void toggle_model_or_primitive() {
		model_or_primitive = !model_or_primitive;
	}
	/** 
	 * To move with random speed, 
	 * this function can be used
	 */
	void move(int step_size) {
		vector3<float> velocity = get_forward();
		velocity *= step_size;
		position = get_position();
		position += velocity;
		moving = true;
	}

	void move_forward() {
		move(speed);
	}

	void move_backward() {
		move(-speed);
	}

	void move() {
		move(speed);
	}
	
	void turn(const direction &dir) {
		switch (dir) {
			case DOWN:
				pitch += turn_amount;
				if (pitch > 360.f) {
					pitch -= 360.0f;
				}
				break;

			case UP:
				pitch -= turn_amount;
				if (pitch < 0.0f) {
					pitch += 360.0f;
				}
				break;

			case LEFT:
				yaw += turn_amount;
				if (yaw > 360.f) {
					yaw -= 360.0f;
				}
				break;

			case RIGHT:
				yaw -= turn_amount;
				if (yaw < 0.0f) {
					yaw += 360.0f;
				}
				break;

			case FORWARD:
				roll += turn_amount;
				if (roll > 360.f) {
					roll -= 360.0f;
				}
				break;

			case BACKWARD:
				roll -= turn_amount;
				if (roll < 0.0f) {
					roll += 360.0f;
				}
				break;
		}
	}

	void destroy() {
		alive = false;
	}

public:
	void set_moving(bool flag) {
		moving = flag;
	}

	bool is_moving() const {
		return moving;
	}

	void set_current_target_id(int id) {
		target_id = id;
	}

	int get_current_target_id() const {
		return target_id;
	}

	int get_shooted_torpedo() const {
		return no_torpedo;
	}

	bool is_alive() const {
		return alive;
	}

	string get_pyr_string() const {
		ostringstream ostr;
		ostr << pitch << " " << yaw << " " << roll;
		return ostr.str();
	}

	void set_speed(float amount) {
		if (abs(amount) <= 200.0f) {
			speed = amount;
		} else {
			speed = 10.0f;
		}
	}

	float get_speed() const {
		return speed;
	}

	int is_run_out_of_torpedo() const {
		return (no_torpedo == MAX_MISSILES);
	}


private:
	void draw_model() {
		glCallList(list_id);
		if (bounding_sphere) {
			glColor3fv(get_color(colors::white));
			glutWireSphere(bounding_sphere_radius/100.0, 40, 17);
		}
	}

	void draw_explosion() {
		if (explosion_radius > 0) {
			glColor3fv(get_color(colors::red));
			glutWireSphere(explosion_radius, 30, 17);
			explosion_radius -= 50;
		} else {
			glColor3fv(get_color(colors::white));
			glutWireSphere(50, 30, 17);
		}
	}

	void draw_spaceship() {
		glColor3fv(get_color(base_color));
		if (is_solid()) {
			glutSolidCone(base, height, 30, 17);
		} else {
			glutWireCone(base, height, 30, 17);
		}
		glTranslatef(0, base/2, height/2);
		glColor3fv(get_color(top_color));
		if (solid) {
			glutSolidSphere(base/2, 30, 17);
		} else {
			glutWireSphere(base/2, 30, 17);
		}
		// draw tail
		if (moving) {
			glTranslatef(0, -80, -60);
			glColor3fv(get_color(colors::fire_brick));
			glutSolidCube(40);
		}
	}

private:
	/* speed of spaceship */
	float speed;

	/* the amount of angle to turn in pitch/yaw/roll */	
	float turn_amount;

	/* dimension of spaceship */
	double base;
	double height;

	/* color */
	color_name base_color;
	color_name top_color;

	/* radius of circle when explosion occur, lazy approach! */
	int explosion_radius;
	
	/* the number of times to draw explosion effect */
	int explosion_count_down;

	/* number of torpedo that are shoot so far */ 
	int no_torpedo;

	/* moving flag for drawing thruster */
	bool moving;

	/* alive flag */
	bool alive;

	/* id list for model loading from .tri file */
	int list_id;

	/* number of lives for ship */
	int no_lives;

	/* target id is the missile moon that the torpedo is currently following */
	int target_id;

	/* the current shooting torpedo */
	torpedo *torpe;

	/* toggle between drawing primitive and model */
	bool model_or_primitive;

	/* closest target for current torpedo */
	vector3<float> target;
};

#endif