#pragma once
#include "reflect.h"
#include <string>

struct Player1 {
    std::string name;
    int score;
    REGISTER_FIELDS(FB_FIELD(name), FB_FIELD(score));
    static std::string fbs_name() { return "Player1"; }
};

REGISTER_ENTITY(Player1)