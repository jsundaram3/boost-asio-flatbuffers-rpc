#pragma once
#include <boost/asio.hpp>
#include <vector>
#include "rpc_header.h"
using boost::asio::ip::tcp;

class RpcClient
{
    boost::asio::io_context io_;
    tcp::socket socket_;

public:
    RpcClient() : socket_(io_) {}

    void connect(const std::string& host,int port)
    {
        tcp::resolver resolver(io_);
        auto endpoints = resolver.resolve(host,std::to_string(port));
        boost::asio::connect(socket_,endpoints);
    }

    std::vector<uint8_t> call(uint16_t method,const uint8_t* data,size_t size)
    {
        RpcHeader header{static_cast<uint32_t>(size), method, 1};
        boost::asio::write(socket_, boost::asio::buffer(&header,sizeof(header)));
        boost::asio::write(socket_, boost::asio::buffer(data,size));

        std::vector<uint8_t> buffer(1024);
        size_t len = socket_.read_some(boost::asio::buffer(buffer));
        buffer.resize(len);
        return buffer;
    }
};