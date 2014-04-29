#ifndef __SOLAR_SYSTEM_VECTOR3_H
#define __SOLAR_SYSTEM_VECTOR3_H

#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;

namespace util {
    template <class T>
    class vector3 {
    private:
        T x, y, z;

    public:
        vector3(const T &x_param = T(), const T &y_param = T(), const T &z_param = T())
        :x(x_param), y(y_param), z(z_param) {
        }

        T &get_x() {
            return x;
        }

        const T get_x() const {
            return x;
        }

        T &get_y() {
            return y;
        }

        const T get_y() const {
            return y;
        }

        T &get_z() {
            return z;
        }

        const T get_z() const {
            return z;
        }

        void set(const T &x, const T &y, const T &z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        vector3<T> operator -() {
            return vector3<T>(-x, -y, -z);
        }

        /**
         * Addition
         *
         * @param: o
         *			other
         * @return:
         *			a copy of addition
         */
        vector3<T> operator +(const vector3<T> &o) const {
            return vector3<T>(x + o.x, y + o.y, z + o.z);
        }

        vector3<T> &operator += (const vector3<T> &o) {
            x += o.x, y += o.y, z += o.z;
            return *this;
        }

        vector3<T> operator -(const vector3<T> &o) const {
            return vector3<T>(x - o.x, y - o.y, z - o.z);
        };

        vector3<T> &operator -= (const vector3<T> &o) {
            x -= o.x, y -= o.y, z -= o.z;
            return *this;
        }

        vector3<T> operator *(const vector3<T> &o) const {
            return vector3<T>(x * o.x, y * o.y, z * o.z);
        }

        vector3<T> &operator *= (const vector3<T> &o) {
            x *= o.x, y *= o.y, z *= o.z;
            return *this;
        }

        vector3<T> operator *(const T &scalar) const {
            return vector3<T>(x * scalar, y * scalar, z * scalar);
        }

        vector3<T> &operator *= (const T &scalar) {
            x *= scalar, y *= scalar, z *= scalar;
            return *this;
        }

        T dot(const vector3<T> &o) const {
            return (x * o.x + y * o.y + z * o.z);
        }

        vector3<T> operator /(const T &scalar) const {
            return vector3<T>(x / scalar, y / scalar, z / scalar);
        }

        vector3<T> &operator /= (const T &scalar) {
            x /= scalar, y /= scalar, z /= scalar;
            return *this;
        }

        vector3<T> operator ^(const vector3<T> &o) const {
            return vector3<T>(
                    (y * o.z) - (o.y * z),
                    (z * o.x) - (o.z * x),
                    (x * o.y) - (o.x * y));
        }

        vector3<T> cross(const vector3<T> &o) const {
            return vector3<T>(
                    (y * o.z) - (o.y * z),
                    (z * o.x) - (o.z * x),
                    (x * o.y) - (o.x * y));
        }

        vector3<T> normal() const {
            return (*this / length());
        }

        void normalize() {
            T scalar = length();
            x /= scalar, y /= scalar, z /= scalar;
        }

        T length() const {
            return sqrt(x * x + y * y + z * z);
        }

        const T angle(const vector3<T> &o) const {
            const T dot = normal() * o.normal();
            if (dot == 0) {
                return 0;
            }

            T t = (*this * o) / dot;
            if (t > 1.0) {
                t = 1.0;
            } else if (t < -1.0) {
                t = -1.0;
            }
            return acos(t);
        }

        vector3<T> orthogonal() const {
            vector3<T> temp(1, y, z);
            if (y < x && y <= z) {
                temp = vector3<T>(x, 1, z);
            } else if (z < x && z <= y) {
                temp = vector3<T>(x, y, 1);
            }
            return (temp ^ *this);
        }

        T &operator [](const int i) {
            assert(i >= 0 && i < 3);
            if (i == 0) {
                return x;
            } else if (i == 1) {
                return y;
            } else {
                return z;
            }
        }

        const T operator [](const int i) const {
            assert(i >= 0 && i < 3);
            if (i == 0) {
                return x;
            } else if (i == 1) {
                return y;
            } else {
                return z;
            }
        }

        bool operator ==(const vector3<T> &rhs) {
            return
                    (abs(x - rhs.x) < 0.000001) &&
                            (abs(y - rhs.y) < 0.000001) &&
                            (abs(z - rhs.z) < 0.000001);
        }

        void show() const {
            cout << x << ", " << y << ", " << z;
        }

    public:
        friend
        ostream &operator <<(ostream &out, const vector3<T> &o) {
            return out << o.x << ", " << o.y << ", " << o.z;
        }
    };
}

#endif
