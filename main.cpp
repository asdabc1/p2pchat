#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <chrono>

#include "Connection.h"
#include "MsgQ.h"

using namespace boost::asio;

void sendMessageFromQueue(MsgQ& queue);
void receiveAndPrintMsg(Connection& connection);

int main() {
    io_context io;
    char control;

    std::cout << "Chat application - temporary console interface\n";

    std::cout << "Provide a port number to be used by this instance of the application (the number should fall outside the scope of well-known ports): ";
    unsigned int port;
    std::cin >> port;

    Connection connection(io, port);

    std::thread ioThread([&io](){io.run();});

    std::cout << "Provide an IP address or await connection: (c - connect, a - await): ";

    std::cin >> control;
    if (control == 'c') {
        std::cout << "Provide the IP address you're willing to connect with in the dotted format (eg. 127.0.0.1): ";
        std::string address;
        std::cin >> address;

        std::cout << "Now provide the port that the person on the other end uses: ";
        unsigned int remotePort;
        std::cin >> remotePort;

        connection.connect(address.c_str(), remotePort);
    }
    else if (control == 'a') {
        for (int i = 0; i < 21; i++) {  //temporary solution
            std::this_thread::sleep_for(std::chrono::seconds(3));
            if (connection.isUp())
                break;
        }
    }

    if (!connection.isUp()) {
        std::cout << "Failed to establish connection, exiting..." << std::endl;
        io.stop();
        ioThread.join();
        return 15;
    }

    MsgQ outgoingMessages;
    std::mutex outputMutex;

    

    io.stop();
    ioThread.join();
    return 0;
}
