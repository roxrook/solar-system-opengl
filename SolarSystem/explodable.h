//
//  explodable.h
//  SolarSystem
//
//  Created by chan on 4/23/14.
//  Copyright (c) 2014 chan. All rights reserved.
//

#ifndef __SOLAR_SYSTEM_EXPLODABLE_H
#define __SOLAR_SYSTEM_EXPLODABLE_H

class explodable {
public:
	virtual void draw_explosion() = 0;
	virtual ~explodable() {}
};

#endif
