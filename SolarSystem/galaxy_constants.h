#ifndef PLANET_CONSTANTS_H
#define PLANET_CONSTANTS_H

#include "vector3.h"

using namespace util;

namespace galaxy_constants {
    namespace warbird {
        const char *name = "Warbird";
        float position[3] = {5000, 1000, -5000};
        float speed = 50.0f;
        float base = 60.0;
        float height = 160.0;
    }

    namespace helios {
        const char *name = "Helios";
        float radius = 2000.0f;
    }

    namespace obstacles {
        const char *name = "pie";
        float radius = 500.0f;
        float position[3] = {11000.0f, 5000.0f, 0.0f};

        namespace one {
            const char *name = "one";
            float radius = 500.0f;
            float position[3] = {-5000.0f, 7000.0f, 0.0f};
        }

        namespace two {
            const char *name = "two";
            float radius = 500.0f;
            float position[3] = {0000.0f, 7000.0f, 0.0f};
        }

        namespace three {
            const char *name = "three";
            float radius = 500.0f;
            float position[3] = {5000.0f, 7000.0f, 0.0f};
        }

        namespace four {
            const char *name = "four";
            float radius = 500.0f;
            float position[3] = {10000.0f, 7000.0f, 0.0f};
        }
    }

    namespace helion {
        const char *name = "helion";
        float radius = 500.0f;
        float degree = 0.01f;
        float position[3] = {15000.0f, 0.0f, 0.0f};
    }

    namespace unum {
        const char *name = "Unum";
        float radius = 300.0f;
        float degree = 0.45f;
        float position[3] = {4000.0f, 0.0f, 0.0f};

        namespace u_missile {
            const char *name = "U.Missile";
            float radius = 50.0f;
            float degree = 0.0f;
            float position[3] = {325.0f, 0.0f, 0.0f};
        }

        namespace primun {
            const char *name = "U.Primun";
            float radius = 50.0f;
            float degree = 1.13f;
            float position[3] = {800.0f, 0.0f, 0.0f};
        }

        namespace secundo {
            const char *name = "U.Secundo";
            float radius = 50.0f;
            float degree = 0.750f;
            float position[3] = {1000.0f, 0.0f, 0.0f};
        }
    }

    namespace duo {
        const char *name = "Duo";
        float radius = 400.0f;
        float degree = 0.23f;
        float position[3] = {-7000.0f, 0.0f, 0.0f};
    }

    namespace tres {
        const char *name = "Tres";
        float radius = 800.0f;
        float degree = 0.11;
        // float degree = 0.001;
        float position[3] = {0.0f, 0.0f, 12000.0f};

        namespace primun {
            const char *name = "T.Primun";
            float radius = 50.0f;
            float degree = 0.750f;
            float position[3] = {0.0f, 0.0f, 1800.0f};
        }

        namespace secundo {
            const char *name = "T.Secundo";
            float radius = 50.0f;
            float degree = 0.56f;
            float position[3] = {0.0f, 0.0f, 2000.0f};
        }

        namespace tertia {
            const char *name = "T.Tertia";
            float radius = 80.0f;
            float degree = 0.450f;
            float position[3] = {0.0f, 0.0f, 2300.0f};
        }

        namespace t_missile {
            const char *name = "T.Missile";
            float radius = 50.0f;
            float degree = 0.0f;
            float position[3] = {0.0f, 0.0f, 3005.0f};
        }

        namespace quartum {
            const char *name = "T.Quartum";
            float radius = 100.0f;
            float degree = 0.32;
            float position[3] = {0.0f, 0.0f, 2700.0f};
        }
    }

    namespace quattuor {
        const char *name = "Quattuor";
        float radius = 500.0f;
        float degree = 0.08f;
        float position[3] = {0.0f, 0.0f, -20000.0f};

        namespace primun {
            const char *name = "Q.Primun";
            float radius = 100.0f;
            float degree = 0.45f;
            float position[3] = {0.0f, 0.0f, -1000.0f};
        }
    }

    namespace lighting {
        float ambient[4] = {0.0, 0.0, 0.0, 1.0};
        float diffuse[4] = {1.0, 1.0, 1.0, 1.0};
        float specular[4] = {1.0, 1.0, 1.0, 1.0};
        float position[4] = {4000.0f, 4000.0f, 4000.0f, 0.0f};
    }

    namespace camera_view {
        namespace front {
            float position[3] = {0.0f, 0.0f, 40000.0f};
            float lookat[3] = {0.0f, 0.0f, 0.0f};
            float up[3] = {0.0f, 1.0f, 0.0f};
        }

        namespace top {
            float position[3] = {0.0f, 40000.0f, 0.0f};
            float lookat[3] = {0.0f, 0.0f, 0.0f};
            float up[3] = {1.0f, 0.0f, 0.0f};
        }

        namespace ship {
            float position[3] = {6000.0f, 1500.0f, -6000.0f};
            float lookat[3] = {0.0f, 0.0f, 0.0f};
            float up[3] = {0.0f, 1.0f, 0.0f};
        }

        namespace moving {
                vector3<float> planet_cameras[4] = {
                vector3<float>(0.0f, 2000.0f, 0.0f),
                vector3<float>(0.0f, 2000.0f, 0.0f),
                vector3<float>(0.0f, 5000.0f, 0.0f),
                vector3<float>(0.0f, 3000.0f, 0.0f)
            };
        }
    }

    namespace test_object {
        const char *name = "Quattuor";
        float radius = 25.0f;
        float degree = 5.08f;
        float position[3] = {300.0f, 0.0f, 0.0f};

        namespace test_primun {
            const char *name = "Q.Primun";
            float radius = 10.0f;
            float degree = 2.05f;
            float position[3] = {100.0f, 0.0f, 0.0f};
        }
    }
}

#endif