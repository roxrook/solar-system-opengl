#ifndef __SOLAR_SYSTEM_MATH_3D_H
#define __SOLAR_SYSTEM_MATH_3D_H

#include <cmath>
#include "vector3.h"

namespace util {
	namespace constants {
		const float PI = 3.1415926535f;
	}

	namespace math3d {
		using namespace constants;
		/** 
		 * Convert from degree to radian
		 */
		template <typename T>
		T to_radian(T degree) {
			return (degree * PI)/180.0;
		}

		/** 
		 * Convert from radian to degree
		 */
		template <typename T>
		T to_degree(T radian) {
			return (radian * 180.0)/PI;
		}

		/** 
		 * Return the distance between two vector3
		 */
		template <typename T>
		T distance(const util::vector3<T> &u, const util::vector3<T> &v) {
			util::vector3<T> r(u - v);
			return r.length();
		}

		/** 
		 * Rotates the vector by the indicated number of degrees about the specified axis
		 */
		template <typename T>
		vector3<T> rotate(const vector3<T> &v, vector3<T> axis, T degrees) {
			axis = axis.normal();
			float radians = degrees * util::constants::PI / 180;
			float s = sin(radians);
			float c = cos(radians);
			return (v * c) + axis * axis.dot(v) * (1 - c) + v.cross(axis) * s;
		}
	}
}

#endif
