#include "Connection.h"

Connection::Connection(io_context& io, unsigned int port) : io(io), soc(io), acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), port)) {

    soc.bind(ip::tcp::endpoint(ip::tcp::v4(), port));
}

void Connection::connect(const char* address, int port) {
    ip::address_v4 add = boost::asio::ip::address_v4::from_string(address);

    ip::tcp::endpoint remoteChatter(add, port);

    soc.async_connect(remoteChatter, [this, &add](const std::error_code& ec) {
        if (!ec) {
            std::cout << "Now connected to " << add.to_string() << std::endl;
            isConnected = true;
        }
        else
            std::cout << "Failed to establish connection with " << add.to_string() << '\n';
    });
}

void Connection::receiveConnection() {
    std::thread thread([this](){
        acceptor.accept();
        while (acceptor.is_open()) {
            if (isConnected)
                acceptor.close();
        }
    });
}
