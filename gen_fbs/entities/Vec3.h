#pragma once
#include "reflect.h"
#include <array>

struct Vec3 {
    float x, y, z;
    REGISTER_FIELDS(FB_FIELD(x), FB_FIELD(y), FB_FIELD(z));
    static std::string fbs_name() { return "Vec3"; };
};

REGISTER_ENTITY(Vec3)