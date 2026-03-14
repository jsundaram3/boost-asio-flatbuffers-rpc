#include "banking_client_stub.h"
#include <iostream>

int main()
{
    RpcClient rpc;
    rpc.connect("127.0.0.1", 9000);

    BankingServiceClient client(rpc);
    auto resp = client.GetAccount("ACC1001");
    std::cout << resp->account()->owner()->str() << std::endl;
}