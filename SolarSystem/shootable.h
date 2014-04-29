#ifndef __SOLAR_SYSTEM_SHOOTABLE_INTERFACE
#define __SOLAR_SYSTEM_SHOOTABLE_INTERFACE

#include <vector>

#include "vector3.h"
#include "object3d.h"
#include "math3d.h"
#include "missile.h"
#include "torpedo.h"

using namespace std;

class shootable {
public:
	virtual torpedo *shoot_target(const vector3<float> &target) = 0;
	virtual bool scan_target(const object3d *obj) = 0;

public:
	virtual ~shootable() {}
};

#endif