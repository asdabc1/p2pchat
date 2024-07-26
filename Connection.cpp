#include "Connection.h"

Connection::Connection(io_context& io, unsigned int port) : io(io), soc(io), acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), port)) {
    ip::address_v4 add = ip::address_v4::from_string("127.0.0.1");

    soc.open(ip::tcp::v4());
    soc.bind(ip::tcp::endpoint(add, port));
}

void Connection::connect(const char* address, unsigned int port) {
    ip::address_v4 add = boost::asio::ip::address_v4::from_string(address);

    ip::tcp::endpoint remoteChatter(add, port);

    boost::system::error_code ec;
    soc.connect(remoteChatter, ec);

    if (!ec) {
        std::cout << "now connected\n";
        isConnected = true;
        this->receiveHeader();
    }
    else
        std::cout << "Connection error: " << ec.message() << std::endl;
}

void Connection::receiveConnection() {
    ip::tcp::socket tempSoc(io);
    boost::system::error_code ec;

    std::cout << "\nWaiting for connections...\n";

    acceptor.accept(tempSoc, ec);

    if (!ec) {
        std::cout << "Connection attempt from " << tempSoc.remote_endpoint() << ". Are you willing to accept? (y/n)";
        char choice;
        std::cin >> choice;

        if (choice == 'y') {
            soc = std::move(tempSoc);
            std::cout << "connected\n";
            isConnected = true;
            this->receiveHeader();
        }
        else
            receiveConnection();
    }
    else
        std::cout << "Problem with receiving a connection: " << ec.message() << std::endl;
}

void Connection::disconnect() {
    soc.close();
    std::cout << "\n\ndisconnected\n";
    isConnected = false;
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

void Connection::sendHeader(Message msg) {
    async_write(soc, buffer(&msg.head, HEADER_SIZE), [this, msg](const std::error_code& ec, size_t length) {
        if (!ec) {
            sendBody(msg);
        }
        else
            std::cout << "Failed to send the header of a message: " << ec.message() << std::endl;
    });
}

void Connection::sendBody(Message msg) {
    async_write(soc, buffer(msg.body.data(), msg.head.size), [](const std::error_code& ec, size_t length) {
        if (ec)
            std::cout << "Failed to send the contents of the message: " << ec.message() << '\n';
    });
}