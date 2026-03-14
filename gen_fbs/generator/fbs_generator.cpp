// fbs_generator.cpp
#include "../entities/reflect.h"
//#include "fbs_enum_map_generator.h"
#include "../entities/generated_entity_includes.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <map>
#include <set>
#include <vector>
#include <queue>


namespace fs = std::filesystem;

int generate_fbs_files1()
{
     std::string currDir = std::filesystem::current_path().string();
    std::string output_dir = currDir + "\\fbs_schemas";

    if(!std::filesystem::exists(output_dir))
    {
        std::filesystem::create_directories(output_dir);
    }
    auto &registry = EntityRegistry::entity_fields();

    if(registry.empty())
    {
        std::cout << "No entities registered!\n";
        return 1;
    }

    generate_fbs_all(output_dir);
    // 1. Generate entity .fbs
    // for(auto &func : EntityRegistry::entity_map()) func(output_dir);

    // // 2. Generate enums + combined map pairs
    // generate_extra_fbs(output_dir);

    return 0;
}

// Detect dependencies of an fbs file by scanning `include` and table field types
std::set<std::string> parse_dependencies(const fs::path& file) {
    std::set<std::string> deps;
    std::ifstream in(file, std::ios::binary);
    std::string line;
    std::regex include_regex(R"(include\s+\"([^\"]+)\")");
    std::regex type_regex(R"(^\s*\w+\s*:\s*(\w+))");

    while (std::getline(in, line)) {
        std::smatch match;
        if (std::regex_search(line, match, include_regex)) {
            std::string inc = match[1];
            fs::path inc_path = fs::path(inc).filename();
            deps.insert(inc_path.string());
        } else if (std::regex_search(line, match, type_regex)) {
            std::string type = match[1];
            if (type != "string" && type != "bool" && type != "int" && type != "float" && type != "double") {
                deps.insert(type + ".fbs"); // assume table/struct in same folder
            }
        }
    }
    return deps;
}

// Topological sort
std::vector<fs::path> topological_sort(const std::vector<fs::path>& files) {
    std::map<std::string, std::set<std::string>> adj;
    std::map<std::string, int> indegree;
    std::map<std::string, fs::path> path_map;

    // Build graph
    for (auto& f : files) {
        std::string fname = f.filename().string();
        path_map[fname] = f;
        indegree[fname] = 0;
    }

    for (auto& f : files) {
        std::string fname = f.filename().string();
        auto deps = parse_dependencies(f);
        for (auto& d : deps) {
            if (adj[d].insert(fname).second) indegree[fname]++;
        }
    }

    // Kahn's algorithm
    std::queue<std::string> q;
    for (auto& [name, deg] : indegree) if (deg == 0) q.push(name);

    std::vector<fs::path> sorted;
    while (!q.empty()) {
        std::string u = q.front(); q.pop();
        sorted.push_back(path_map[u]);
        for (auto& v : adj[u]) {
            indegree[v]--;
            if (indegree[v] == 0) q.push(v);
        }
    }

    if (sorted.size() != files.size()) {
        throw std::runtime_error("Cyclic dependency detected in fbs files!");
    }
    return sorted;
}

// Generate stubs
void generate_stubs(const fs::path& output_dir, const std::vector<fs::path>& files, const std::string& flatc_path) {
    fs::create_directories(output_dir);
    for (auto& f : files) {
        std::string cmd = flatc_path + " --cpp -o " + output_dir.string() + " " + f.string();
        std::cout << "Running: " << cmd << std::endl;
        if (system(cmd.c_str()) != 0) {
            throw std::runtime_error("flatc failed on " + f.string());
        }
    }
}

int main(int argc, char** argv) {
    fs::path schema_dir = "fbs_schemas";
    fs::path output_dir = "generated_flatbuffers";
    std::string flatc_exe = "D:/BNP_Projects/flatbuffers/build/Release/flatc.exe"; // adjust your path

    if(generate_fbs_files1())
    {
        std::cout << ".fbs file generation failed!" << std::endl;
    }

    std::vector<fs::path> fbs_files;
    for (auto& p : fs::directory_iterator(schema_dir)) {
        if (p.path().extension() == ".fbs") fbs_files.push_back(p.path());
    }

    try {
        auto sorted_files = topological_sort(fbs_files);
        generate_stubs(output_dir, sorted_files, flatc_exe);
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "All stubs generated successfully!" << std::endl;
    return 0;
}