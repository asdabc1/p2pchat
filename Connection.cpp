#include "Connection.h"

Connection::Connection(io_context& io, unsigned int port) : io(io), soc(io) {

    soc.bind(ip::tcp::endpoint(ip::tcp::v4(), port));
}

void Connection::connect(const char* address, int port) {
    ip::address_v4 add = boost::asio::ip::address_v4::from_string(address);

    ip::tcp::endpoint remoteChatter(add, port);

    soc.async_connect(remoteChatter, [this, &add](const std::error_code& ec) {
        if (!ec) {

        }
        else
            std::cout << "Failed to establish connection with " << add.to_string() << '\n';
    });
}