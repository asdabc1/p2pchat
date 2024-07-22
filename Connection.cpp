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
        acceptor.accept(soc);
    });
}

void Connection::disconnect() {
    soc.close();
    std::cout << "disconnected\n";
}

void Connection::receiveHeader() {
    async_read(soc, buffer(&temp.head, HEADER_SIZE), [this](const std::error_code& ec, size_t length) {
        if (!ec) {
            temp.update();
            receiveBody();
        }

        else
            std::cout << "Failed to receive the header of the message: " << ec.message() << '\n';

        receiveHeader();
    });
}

void Connection::receiveBody() {
    async_read(soc, buffer(temp.body.data(), temp.head.size), [this](const std::error_code& ec, size_t length){
        if (!ec) {
            queue.addToQueue(temp);
            temp = Message();
        }

        else
            std::cout << "Failed to receive the contents of the message: " << ec.message() << '\n';

        receiveHeader();
    });
}

void Connection::sendHeader(Message& msg) {
    async_write(soc, buffer(&msg.head, HEADER_SIZE), [this, &msg](const std::error_code& ec, size_t length) {
        if (ec)
            std::cout << "Failed to send the header of a message: " << ec.message() << '\n';

        sendBody(msg);
    });
}

void Connection::sendBody(Message& msg) {
    async_write(soc, buffer(msg.body.data(), msg.head.size), [this](const std::error_code& ec, size_t length) {
        if (ec)
            std::cout << "Failed to send the contents of the message: " << ec.message() << '\n';
    });
}

void Connection::sendMessage(Message &msg) {
    sendHeader(msg);
}