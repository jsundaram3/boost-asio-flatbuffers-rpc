class RpcSession : public std::enable_shared_from_this<RpcSession>
{
    tcp::socket socket_;
    std::vector<uint8_t> buffer_;
    RpcDispatcher& dispatcher_;

public:
    RpcSession(tcp::socket socket, RpcDispatcher& dispatcher)
        : socket_(std::move(socket)), dispatcher_(dispatcher) {}

    void start() { do_read_header(); }

private:
    void do_read_header();
    void do_read_body(uint32_t body_size, uint16_t method_id, uint32_t request_id);
};

class RpcServer
{
    boost::asio::io_context& io_;
    tcp::acceptor acceptor_;
    RpcDispatcher dispatcher_;

public:
    RpcServer(boost::asio::io_context& io, int port)
        : io_(io), acceptor_(io, tcp::endpoint(tcp::v4(), port)) {}

    void start_accept()
    {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if(!ec)
                std::make_shared<RpcSession>(std::move(socket), dispatcher_)->start();
            start_accept();
        });
    }

    RpcDispatcher& dispatcher() { return dispatcher_; }
};