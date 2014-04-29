// DONE!
#ifndef AUTO_ARRAY_H
#define AUTO_ARRAY_H

#include <cstdlib>

namespace util {
    template <class T>
    class auto_array {
    private:
        T *data;
        mutable bool is_released;

    public:
        explicit auto_array(T *data = NULL) :
        data(data), is_released(false) {
        }

        auto_array(const auto_array<T> &rhs) {
            data = rhs.data;
            is_released = rhs.is_released;
            rhs.is_released = true;
        }

        ~auto_array() {
            if (!is_released && data != NULL) {
                delete[] data;
            }
        }

        T *get() const {
            return data;
        }

        T &operator *() const {
            return *data;
        }

        void operator = (const auto_array<T> &rhs) {
            if (!is_released && data != NULL) {
                delete[] data;
            }
            data = rhs.data;
            is_released = rhs.is_released;
            rhs.is_released = true;
        }

        T *operator ->() const {
            return data;
        }

        T *release() {
            is_released = true;
            return data;
        }

        void reset(T *array_ = NULL) {
            if (!is_released && data != NULL) {
                delete[] data;
            }
            data = array_;
        }

        T *operator +(int i) {
            return data + i;
        }

        T &operator [](int i) {
            return data[i];
        }
    };

}

#endif