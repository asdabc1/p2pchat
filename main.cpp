#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <chrono>

#include "Connection.h"
#include "MsgQ.h"

using namespace boost::asio;

void sendMessageFromQueue(MsgQ& queue, Connection& connection);
void receiveAndPrintMsg(Connection& connection);

int main() {
    io_context io;
    executor_work_guard<decltype(io.get_executor())> work{io.get_executor()};
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
        std::cout << "\nAttempting to connect...\n";
    }
    else if (control == 'a') {
        connection.receiveConnection();
    }

    if (!connection.isUp()) {
        std::cout << "Failed to establish connection, exiting..." << std::endl;
        io.stop();
        ioThread.join();
        return 15;
    }

    MsgQ outgoingMessages;

    std::cout << "\n\n\nNow chatting with " << connection.chatter() << std::endl;

    std::thread messageReceptionThread(receiveAndPrintMsg, std::ref(connection));
    std::thread messageSendThread(sendMessageFromQueue, std::ref(outgoingMessages), std::ref(connection));

    std::string input;
    Message msgToBeSent;


    std::cout << "Type \'!!quit\' to quit the application\n";
    std::cin.get();
    while (true) {
        std::getline(std::cin, input, '\n');

        if (input == "!!quit")
            break;

        if (!connection.isUp() && connection.socIsOpen()) {
            std::cout << "Disconnected by remote host.\n";
            break;
        }

        msgToBeSent << input;
        outgoingMessages.addToQueue(msgToBeSent);

        msgToBeSent = Message();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    connection.disconnect();
    messageReceptionThread.join();
    messageSendThread.join();
    io.stop();
    ioThread.join();

    std::cout << "\n\nPress ENTER to quit\n";
    std::cin.get();

    return 0;
}

void sendMessageFromQueue(MsgQ& queue, Connection& connection) {
    while (connection.isUp()) {
        if (queue.queue.empty())
            continue;

        Message temp;
        temp = queue.getFromQueue();

        connection.sendHeader(temp);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void receiveAndPrintMsg(Connection& connection) {
    while (connection.isUp()) {
        if (connection.qIsEmpty())
            continue;

        Message temp;
        temp = connection.retreiveMsgFromQueue();

        std::cout << "\n\nOther chatter:\n";
        std::cout << temp << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}