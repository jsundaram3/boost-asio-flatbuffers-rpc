#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <optional>
#include <map>
#include <functional>
// ---------------------- FieldInfo ----------------------
struct FieldInfo {
    std::string name;
    std::string type; // e.g., "[Vec3]" or "int" or "map<string,int>"
    bool is_map = false;
    std::string map_key_type;
    std::string map_value_type;
};

// ---------------------- Type Traits ----------------------
// Detect STL containers
template<typename T> struct is_std_vector : std::false_type {};
template<typename T> struct is_std_vector<std::vector<T>> : std::true_type {};

template<typename T> struct is_std_array : std::false_type {};
template<typename T, size_t N> struct is_std_array<std::array<T,N>> : std::true_type {};

template<typename T> struct is_std_map : std::false_type {};
template<typename K, typename V> struct is_std_map<std::map<K,V>> : std::true_type {};

template<typename T> struct is_std_optional : std::false_type {};
template<typename T> struct is_std_optional<std::optional<T>> : std::true_type {};
// ---------------------- Type Mapping ----------------------
template<typename T, typename Enable = void>
struct FBType
{
    static std::string name()
    {
        if constexpr(std::is_enum_v<T>)
        {
            // fallback if enum not registered yet
            return typeid(T).name();
        }
        else
        {
            return T::fbs_name();
        }
    }
};
// ---------------------- get_type_name ----------------------
template<typename T>
std::string get_type_name(FieldInfo* out_field=nullptr) {
    if constexpr (std::is_same_v<T,int>) return "int";
    else if constexpr (std::is_same_v<T,float>) return "float";
    else if constexpr (std::is_same_v<T,double>) return "double";
    else if constexpr (std::is_same_v<T,bool>) return "bool";
    else if constexpr (std::is_same_v<T,std::string>) return "string";

    else if constexpr (is_std_vector<T>::value) {
        using Inner = typename T::value_type;
        return "[" + get_type_name<Inner>() + "]";
    }
    else if constexpr (is_std_array<T>::value) {
        using Inner = typename T::value_type;
        return "[" + get_type_name<Inner>() + "]";
    }
    else if constexpr (is_std_optional<T>::value) {
        using Inner = typename T::value_type;
        return get_type_name<Inner>();
    }
    else if constexpr (is_std_map<T>::value) {
        using K = typename T::key_type;
        using V = typename T::mapped_type;
        if (out_field) {
            out_field->is_map = true;
            out_field->map_key_type = get_type_name<K>();
            out_field->map_value_type = get_type_name<V>();
        }
        return "map<" + get_type_name<K>() + "," + get_type_name<V>() + ">";
    }
    else if constexpr(std::is_enum_v<T>)
    {
       // return out_field->type();
    }
    else
    {
         return typeid(T).name();
    }
}


// Primitive types
template<> struct FBType<int> { static std::string name(){ return "int"; } };
template<> struct FBType<float> { static std::string name(){ return "float"; } };
template<> struct FBType<double> { static std::string name(){ return "double"; } };
template<> struct FBType<bool> { static std::string name(){ return "bool"; } };
template<> struct FBType<std::string> { static std::string name(){ return "string"; } };

// std::vector
template<typename T> struct FBType<std::vector<T>> { static std::string name(){ return "[" + FBType<T>::name() + "]"; } };

// std::array
template<typename T, size_t N> struct FBType<std::array<T,N>> { static std::string name(){ return "[" + FBType<T>::name() + "]"; } };

// std::optional
template<typename T> struct FBType<std::optional<T>> { static std::string name(){ return FBType<T>::name(); } };

// std::map -> Pair table
template<typename K, typename V>
struct FBType<std::map<K,V>>
{
    static std::string name()
    {
        std::string pair_name = FBType<K>::name() + FBType<V>::name() + "Pair";
        auto &pairs = EntityRegistry::get_map_pairs();
        auto it = std::find_if(pairs.begin(), pairs.end(),
            [&](const MapPairInfo &p){ return p.name == pair_name; });
        if(it == pairs.end()) pairs.push_back({pair_name, FBType<K>::name(), FBType<V>::name()});
        return "[" + pair_name + "]";
    }
};

template<typename K, typename V>
struct FBType<std::unordered_map<K,V>>
{
    static std::string name()
    {
        std::string pair_name = FBType<K>::name() + FBType<V>::name() + "Pair";

        auto &pairs = EntityRegistry::get_map_pairs();
        auto it = std::find_if(pairs.begin(), pairs.end(),
            [&](const MapPairInfo &p){ return p.name == pair_name; });
        if(it == pairs.end()) pairs.push_back({pair_name, FBType<K>::name(), FBType<V>::name()});

        return "[" + pair_name + "]";
    }
};

//#define FB_FIELD(member) FieldInfo{#member, FBType<decltype(member)>::name()}
#define FB_FIELD(member) FieldInfo{#member, get_type_name<decltype(member)>()}
// ---------------------- Enum Info ----------------------
struct EnumInfo
{
    std::string name;
    std::vector<std::pair<std::string,int>> values;
};
// ---------------------- Map Pair Info ----------------------
struct MapPairInfo
{
    std::string name;
    std::string key_type;
    std::string value_type;
};

// ---------------------- Reflection Registry ----------------------
struct EntityRegistry {
    using FieldsVec = std::vector<FieldInfo>;
    static std::unordered_map<std::string, FieldsVec>& entity_fields() {
        static std::unordered_map<std::string, FieldsVec> map;
        return map;
    }

    static std::vector<EnumInfo>& get_enums()
    {
        static std::vector<EnumInfo> instance;
        return instance;
    }
    static std::vector<MapPairInfo>& get_map_pairs()
    {
        static std::vector<MapPairInfo> instance;
        return instance;
    }

     // Dependency tracking: entity -> set of other entities it depends on
    static std::unordered_map<std::string,std::unordered_set<std::string>>& entity_deps() {
        static std::unordered_map<std::string,std::unordered_set<std::string>> map;
        return map;
    }
};

 

// ---------------------- Enum Registry ----------------------
struct EnumRegistry {
    using EnumMap = std::unordered_map<std::string, std::vector<std::pair<std::string,int>>>;
    static EnumMap& enums() { static EnumMap map; return map; }
};
inline std::unordered_map<std::string,std::string> EnumNameMap;


// ---------------------- Macros ----------------------
#define FB_FIELD(member) FieldInfo{#member, get_type_name<decltype(member)>((FieldInfo*)nullptr)}

#define REGISTER_FIELDS(...) \
    static std::vector<FieldInfo> get_fields() { \
        auto fields = std::vector<FieldInfo>{ __VA_ARGS__ }; \
        for (auto& f : fields) get_type_name<void>(&f); /* detect maps */ \
        return fields; \
    }

#define REGISTER_ENTITY(Type) \
namespace { \
    struct Type##Registry { \
        Type##Registry() { \
            auto fields = Type::get_fields(); \
            EntityRegistry::entity_fields()[#Type] = fields; \
            std::unordered_set<std::string> deps; \
            for (auto& f : fields) { \
                std::string t = f.type; \
                if (!t.empty() && t.front()=='[' && t.back()==']') t=t.substr(1,t.size()-2); \
                if (EntityRegistry::entity_fields().count(t)) deps.insert(t); \
                if (EnumRegistry::enums().count(t)) deps.insert("enums.fbs"); \
                if (f.is_map) deps.insert(""); /* map entry tables handled separately */ \
            } \
            EntityRegistry::entity_deps()[#Type] = deps; \
        } \
    }; \
    static Type##Registry global_##Type##_registry; \
}

#define REGISTER_ENUM(EnumType, ...) \
namespace { \
    struct EnumType##Registry { \
        EnumType##Registry() { \
            EnumRegistry::enums()[#EnumType] = { __VA_ARGS__ }; \
            EnumNameMap[#EnumType] = #EnumType; \
        } \
    }; \
    static EnumType##Registry global_##EnumType##_registry; \
}

inline std::vector<std::string> topological_sort_entities() {
    auto& deps_map = EntityRegistry::entity_deps();
    std::unordered_set<std::string> visited;
    std::vector<std::string> result;

    std::function<void(const std::string&)> visit = [&](const std::string& e){
        if (visited.count(e)) return;
        visited.insert(e);
        for (auto& d : deps_map[e]) {
            if (EntityRegistry::entity_fields().count(d)) visit(d);
        }
        result.push_back(e);
    };

    for (auto& [name,_] : EntityRegistry::entity_fields()) {
        visit(name);
    }
    return result;
}

inline void generate_enums_fbs(const std::string& out_dir=".") {
    std::ofstream f(out_dir + "/enums.fbs");
    if (!f.is_open()) return;
    for (auto& [ename, values] : EnumRegistry::enums()) {
        f << "enum " << ename << " : int {\n";
        for (auto& [name,val] : values) f << "  " << name << " = " << val << ",\n";
        f << "}\n\n";
    }
    std::cout << "Generated " << out_dir << "/enums.fbs\n";
}


inline void generate_fbs_all(const std::string& out_dir=".") {
   // generate_enums_fbs(out_dir); // always first

    auto order = topological_sort_entities();

    for (auto& entity_name : order) {
        auto& fields = EntityRegistry::entity_fields()[entity_name];
        std::unordered_set<std::string> includes;

        for (auto& f : fields) {
            std::string t = f.type;
            if (!t.empty() && t.front() == '[' && t.back() == ']') t = t.substr(1, t.size()-2); // unwrap vector

            if (EntityRegistry::entity_fields().count(t)) includes.insert(t + ".fbs");
            if (EnumRegistry::enums().count(t)) includes.insert("enums.fbs");
        }

        std::ofstream f(out_dir + "/" + entity_name + ".fbs");
        if (!f.is_open()) continue;

        for (auto& inc : includes) f << "#include \"" << inc << "\"\n";
        if (!includes.empty()) f << "\n";

        f << "table " << entity_name << " {\n";
        for (auto& field : fields) f << "  " << field.name << ": " << field.type << ";\n";
        f << "}\n";

        std::cout << "Generated " << out_dir + "/" + entity_name + ".fbs\n";
    }
     std::cout << "Generated Completed..." << std::endl;
}

inline void generate_fbs_all_1(const std::string& output_dir = ".") {
    for (auto& [entity_name, fields] : EntityRegistry::entity_fields()) {
        std::string filename = output_dir + "/" + entity_name + ".fbs";
        std::ofstream f(filename);
        if (!f.is_open()) {
            std::cerr << "Failed to write " << filename << "\n";
            continue;
        }

        std::unordered_set<std::string> includes;

        // First pass: detect includes for user-defined types
        for (auto& field : fields) {
            std::string type = field.type;
            if (type != "int" && type != "float" && type != "double" &&
                type != "bool" && type != "string" && type.front() != '[') {
                includes.insert(type + ".fbs");
            }
            if (type.front() == '[') {
                std::string inner = type.substr(1, type.size() - 2);
                if (inner != "int" && inner != "float" && inner != "double" &&
                    inner != "bool" && inner != "string") {
                    includes.insert(inner + ".fbs");
                }
            }
        }

        // Write includes
        for (auto& inc : includes) {
            f << "#include \"" << inc << "\"\n";
        }
        if (!includes.empty()) f << "\n";

        // Write table
        f << "table " << entity_name << " {\n";
        for (auto& field : fields) {
            f << "  " << field.name << ": " << field.type << ";\n";
        }
        f << "}\n";

        std::cout << "Generated " << filename << "\n";
    }
}