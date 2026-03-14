#pragma once
#include "reflect.h"
#include <string>
#include "Vec3.h"

struct Player {
    std::string username;
    int level;
    Vec3 position;

    REGISTER_FIELDS(FB_FIELD(username), FB_FIELD(level), FB_FIELD(position))
    static std::string fbs_name() { return "Player"; }
};

REGISTER_ENTITY(Player)