#ifndef COLORS_H
#define COLORS_H

namespace colors {
	enum color_name {
		red,
		green,
	    blue,
	    white,
		black, 
		yellow,
		cyan,
		magenta,
		dim_gray,
		aquamarine,
		blue_violet,
		brown,
		cadet_blue,
		coral,
		cornflower_blue,
		navy,
		orange,
		violet,
		wheat,
		dark_orchid,
		pink,
		plum,
		sky_blue,
		light_wood,
		quartz,
		flesh,
		scarlet,
		thistle,
		turquoise,
		feldspar,
		dusty_rose,
		khaki,
		fire_brick
	};
}

static float red_ary[]					= {1.0f, 0.0f, 0.0f};
static float green_ary[]				= {0.0f, 1.0f, 0.0f};
static float blue_ary[]					= {0.0f, 0.0f, 1.0f};
static float white_ary[]				= {1.0f, 1.0f, 1.0f};
static float black_ary[]				= {0.0f, 0.0f, 0.0f};
static float yellow_ary[]				= {1.0f, 1.0f, 0.0f};
static float cyan_ary[]					= {0.0f, 1.0f, 1.0f};
static float magenta_ary[]				= {1.0f, 0.0f, 1.0f};
static float dim_gray_ary[]				= {0.329412f, 0.329412f, 0.329412};
static float aquamarine_ary[]			= {0.439216f, 0.858824f, 0.576471f};
static float blue_violet_ary[]			= {0.62352f, 0.372549f, 0.623529f};
static float brown_ary[]				= {0.647059f, 0.164706f, 0.164706f};
static float cadet_blue_ary[]			= {0.372549f, 0.623529f, 0.623529f};
static float coral_ary[]				= {1.0f, 0.498039f, 0.0f};
static float cornflower_blue_ary[]		= {0.258824f, 0.258824f, 0.435294f};
static float navy_ary[]					= {0.137255f, 0.137255f, 0.556863f};
static float orange_ary[]				= {1.0f, 0.5f, 0.0f};
static float violet_ary[]				= {0.309804f, 0.184314f, 0.309804f};
static float wheat_ary[]				= {0.847059f, 0.847059f, 0.74902f};
static float dark_orchid_ary[]			= {0.6f, 0.196078f, 0.8f};
static float pink_ary[]					= {0.737255f, 0.560784f, 0.560784f};
static float plum_ary[]					= {0.917647f, 0.678431f, 0.917647f};
static float sky_blue_ary[]				= {0.196078f, 0.6f, 0.8f};
static float light_wood_ary[]			= {0.91f, 0.76f, 0.65f};
static float quartz_ary[]				= {0.85f, 0.85f, 0.95f};
static float flesh_ary[]				= {0.96f, 0.80f, 0.69f};
static float scarlet_ary[]				= {0.55f, 0.09f, 0.09f};
static float thistle_ary[]              = {0.847059f, 0.74902f, 0.847059f};
static float turquoise_ary[]            = {0.678431, 0.917647, 0.917647};
static float feldspar_ary[]             = {0.82f, 0.57f, 0.46f};
static float dusty_rose_ary[]			= {0.52f, 0.39f, 0.39f};
static float khaki_ary[]				= {0.623529f, 0.623529f, 0.372549f};
static float fire_brick_ary[]			= {0.556863f, 0.137255f, 0.137255f};

float* get_color(const colors::color_name &name) {
	using namespace colors;
	switch (name) {
		case red:
			return red_ary;

		case green:
			return green_ary;

		case blue:
			return blue_ary;

		case white:
			return white_ary;

		case black:
			return black_ary;

		case yellow:
			return yellow_ary;

		case cyan:
			return cyan_ary;

		case magenta:
			return magenta_ary;

		case dim_gray:
			return dim_gray_ary;

		case aquamarine:
			return aquamarine_ary;

		case blue_violet:
			return blue_violet_ary;

		case brown:
			return brown_ary;

		case cadet_blue:
			return cadet_blue_ary;

		case coral:
			return coral_ary;

		case cornflower_blue:
			return cornflower_blue_ary;
			
		case navy:
			return navy_ary;

		case orange:
			return orange_ary;

		case violet:
			return violet_ary;

		case wheat:
			return wheat_ary;

		case dark_orchid:
			return dark_orchid_ary;

		case pink:
			return pink_ary;

		case plum:
			return plum_ary;

		case sky_blue:
			return sky_blue_ary;

		case light_wood:
			return light_wood_ary;

		case quartz:
			return quartz_ary;

		case flesh:
			return flesh_ary;

		case scarlet:
			return scarlet_ary;

		case thistle:
			return thistle_ary;

		case turquoise:
			return turquoise_ary;
			
		case feldspar:
			return feldspar_ary;

		case dusty_rose:
			return dusty_rose_ary;

		case khaki:
			return khaki_ary;

		case fire_brick:
			return fire_brick_ary;
	}
	return NULL;
}

#endif