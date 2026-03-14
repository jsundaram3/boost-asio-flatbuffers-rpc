// generate_rpc_stubs.cpp
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

struct Method {
    std::string name;
    std::string reqType;
    std::string respType;
};

struct Service {
    std::string name;
    std::vector<Method> methods;
};

// Regex to parse .fbs rpc_service and methods
const std::regex rpc_service_re(R"(rpc_service\s+(\w+)\s*\{)");
const std::regex rpc_method_re(R"((\w+)\s*\(\s*(\w+)\s*\)\s*:\s*(\w+)\s*;)");

Service parseFBS(const fs::path &path) {
    std::ifstream file(path);
    std::string line;
    Service service{};
    bool inService = false;

    while (std::getline(file, line)) {
        line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), ""); // trim

        if (line.empty() || line.starts_with("//")) continue;

        std::smatch match;
        if (!inService && std::regex_match(line, match, rpc_service_re)) {
            service.name = match[1];
            inService = true;
            continue;
        }

        if (inService) {
            if (line == "}") {
                inService = false;
                continue;
            }
            if (std::regex_match(line, match, rpc_method_re)) {
                Method m{match[1], match[2], match[3]};
                service.methods.push_back(m);
            }
        }
    }

    return service;
}

// Generate rpc_method_ids.h
void generateMethodIds(const Service &service, const fs::path &outDir) {
    std::ofstream out(outDir / "rpc_method_ids.h");
    out << "#pragma once\n\nenum class RpcMethod : uint16_t {\n";
    for (size_t i = 0; i < service.methods.size(); ++i) {
        out << "    " << service.name << "_" << service.methods[i].name
            << " = " << (i + 1) << ",\n";
    }
    out << "};\n";
}

// Generate client_stub.h
void generateClientStub(const Service &service, const fs::path &outDir) {
    std::ofstream out(outDir / "client_stub.h");
    out << "#pragma once\n";
    out << "#include \"../rpc/rpc_client.h\"\n";
    out << "#include \"../generated/banking_generated.h\"\n";
    out << "#include <flatbuffers/flatbuffers.h>\n";
    out << "#include \"rpc_method_ids.h\"\n\n";

    out << "class " << service.name << "Client {\n";
    out << "    RpcClient& rpc_;\npublic:\n";
    out << "    " << service.name << "Client(RpcClient& rpc) : rpc_(rpc) {}\n";

    for (const auto &m : service.methods) {
        out << "    const banking::" << m.respType << "* " << m.name
            << "(const std::string& account_id) {\n";
        out << "        flatbuffers::FlatBufferBuilder builder;\n";
        out << "        auto id = builder.CreateString(account_id);\n";
        out << "        auto req = banking::Create" << m.reqType << "(builder, id);\n";
        out << "        builder.Finish(req);\n";
        out << "        auto resp = rpc_.call(static_cast<uint16_t>(RpcMethod::"
            << service.name << "_" << m.name << "), builder.GetBufferPointer(), builder.GetSize());\n";
        out << "        return flatbuffers::GetRoot<banking::" << m.respType << ">(resp.data());\n";
        out << "    }\n";
    }
    out << "};\n";
}

// Generate server_stub.h
void generateServerStub(const Service &service, const fs::path &outDir) {
    std::ofstream out(outDir / "server_stub.h");
    out << "#pragma once\n#include <vector>\n#include <cstdint>\n";
    out << "#include \"../generated/banking_generated.h\"\n\n";

    out << "class " << service.name << "Handler {\npublic:\n";
    for (const auto &m : service.methods) {
        out << "    virtual std::vector<uint8_t> " << m.name
            << "(const banking::" << m.reqType << "* req) = 0;\n";
    }
    out << "};\n";
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "Usage: generate_rpc_stubs <schema.fbs> <output_dir>\n";
        return 1;
    }

    fs::path schemaFile = argv[1];
    fs::path outDir = argv[2];
    fs::create_directories(outDir);

    Service service = parseFBS(schemaFile);

    generateMethodIds(service, outDir);
    generateClientStub(service, outDir);
    generateServerStub(service, outDir);

    std::cout << "RPC stubs generated in " << outDir << std::endl;
    return 0;
}