#ifndef __SOLAR_SYSTEM_COLLIDABLE_INTERFACE_H
#define __SOLAR_SYSTEM_COLLIDABLE_INTERFACE_H

class object3d;

class collidable {
    virtual bool collide_with(const object3d *rhs) const = 0;
};

#endif