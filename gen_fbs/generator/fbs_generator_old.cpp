// fbs_generator.cpp
#include "../entities/reflect.h"
#include "fbs_enum_map_generator.h"
#include "../entities/generated_entity_includes.h"
#include <filesystem>
#include <iostream>

int main() {
    std::string currDir = std::filesystem::current_path().string();
    std::string output_dir = currDir + "\\fbs_schemas";

    if(!std::filesystem::exists(output_dir))
    {
        std::filesystem::create_directories(output_dir);
    }
    auto &registry = EntityRegistry::get_entities();

    if(registry.empty())
    {
        std::cout << "No entities registered!\n";
        return 1;
    }

    // 1. Generate entity .fbs
    for(auto &func : EntityRegistry::get_entities()) func(output_dir);

    // 2. Generate enums + combined map pairs
    generate_extra_fbs(output_dir);

    return 0;
}