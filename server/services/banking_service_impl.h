#pragma once
#include "../generated/banking_generated.h"
#include <flatbuffers/flatbuffers.h>
#include <vector>

class BankingServiceImpl
{
public:
    std::vector<uint8_t> GetAccount(const banking::GetAccountRequest* req)
    {
        flatbuffers::FlatBufferBuilder builder;
        auto id = builder.CreateString(req->account_id()->str());
        auto owner = builder.CreateString("John Doe");

        auto acc = banking::CreateAccount(builder,id,owner,10000.50);
        auto resp = banking::CreateGetAccountResponse(builder,acc);
        builder.Finish(resp);

        uint8_t* buf = builder.GetBufferPointer();
        size_t size = builder.GetSize();
        return std::vector<uint8_t>(buf,buf+size);
    }
};