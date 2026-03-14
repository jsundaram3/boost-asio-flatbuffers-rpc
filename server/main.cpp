#include <boost/asio.hpp>
#include "../rpc/rpc_header.h"
#include "banking_service_impl.h"

using boost::asio::ip::tcp;

int main()
{
    boost::asio::io_context io;
    RpcServer server(io, 9000);

    BankingServiceImpl service;

    server.dispatcher().register_handler(
        static_cast<uint16_t>(RpcMethod::BankingService_GetAccount),
        [&service](const RpcRequestContext& ctx){
            auto req = flatbuffers::GetRoot<banking::GetAccountRequest>(ctx.payload.data());
            return service.GetAccount(req);
        });

    server.start_accept();
    io.run();
}