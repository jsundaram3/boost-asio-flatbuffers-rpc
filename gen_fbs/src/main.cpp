#include <iostream>
/*#include "flatbuffers/flatbuffers.h"
//#include "generated/Monster_generated.h"
#include "generated/Player_generated.h"
*/
int main() {
    /*
    flatbuffers::FlatBufferBuilder builder;

    // Player
    auto username = builder.CreateString("Hero");
    auto player_pos = MyGame::Sample::CreateVec3(builder, 0.f, 1.f, 2.f);
    auto player = MyGame::Sample::CreatePlayer(builder, username, 5, player_pos);

    // Monster
    auto name = builder.CreateString("Orc");
    auto monster_pos = MyGame::Sample::CreateVec3(builder, 10.f, 20.f, 30.f);
    std::vector<int> inventory = {100, 200, 300};
    auto inv_vec = builder.CreateVector(inventory);
    auto monster = MyGame::Sample::CreateMonster(builder, name, 100, monster_pos, inv_vec, true);

    builder.Finish(monster);

    auto monster_ptr = MyGame::Sample::GetMonster(builder.GetBufferPointer());
    std::cout << "Monster: " << monster_ptr->name()->str() << ", HP: " << monster_ptr->hp() << "\n";

    auto player_ptr = MyGame::Sample::GetPlayer(builder.GetBufferPointer()); // For demonstration
    std::cout << "Player: " << player_ptr->username()->str() << ", Level: " << player_ptr->level() << "\n";
    */
    return 0;
}