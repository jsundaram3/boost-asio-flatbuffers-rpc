#pragma once
#include "reflect.h"
#include <string>
#include <vector>
#include "Vec3.h"

enum MonsterType { Orc = 0, Goblin = 1, Dragon = 2 };

// register enum with names and values


struct ZMonster {
    std::string name;
    int hp;
    Vec3 position;
    std::vector<int> inventory;
    bool is_alive;
    //MonsterType mntype;
    //std::unordered_map<int, std::string> mpCustomer;
    //std::map<std::string, Vec3> mpNames;

    REGISTER_FIELDS(FB_FIELD(name), FB_FIELD(hp), FB_FIELD(position), FB_FIELD(inventory), FB_FIELD(is_alive))
         
    static std::string fbs_name() { return "ZMonster"; }
};

REGISTER_ENTITY(ZMonster)
//REGISTER_ENUM(MonsterType, {{"Orc",0},{"Goblin",1},{"Dragon",2}})