#pragma once
#include "../rpc/rpc_client.h"
#include "../generated/banking_generated.h"
#include <flatbuffers/flatbuffers.h>

class BankingServiceClient
{
    RpcClient& rpc_;
public:
    BankingServiceClient(RpcClient& rpc) : rpc_(rpc) {}

    const banking::GetAccountResponse* GetAccount(const std::string& id)
    {
        flatbuffers::FlatBufferBuilder builder;
        auto accId = builder.CreateString(id);
        auto req = banking::CreateGetAccountRequest(builder,accId);
        builder.Finish(req);

        auto resp = rpc_.call(1,builder.GetBufferPointer(),builder.GetSize());
        return flatbuffers::GetRoot<banking::GetAccountResponse>(resp.data());
    }
};