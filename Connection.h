#ifndef CHAT_CONNECTION_H
#define CHAT_CONNECTION_H

#include <boost/asio.hpp>
#include <iostream>
#include <thread>

#include "Message.h"
#include "MsgQ.h"

using namespace boost::asio;

class Connection {

private:

    io_context& io;
    ip::tcp::socket soc;
    ip::tcp::acceptor acceptor;

    Message temp;
    MsgQ queue;

    bool isConnected = false;

    void sendHeader(Message& msg);
    void sendBody(Message& msg);

public:
    void receiveHeader();
    void receiveBody();
    void sendMessage(Message& msg);

    void connect(const char* address, unsigned int port);
    void receiveConnection();
    void disconnect();
    bool isUp() {return soc.is_open() && isConnected;}

    bool qIsEmpty() {return queue.queue.empty();}
    Message retreiveMsgFromQueue() {Message tempor = queue.queue.front(); queue.queue.pop_front(); return temp;}

    ip::tcp::endpoint chatter() {return soc.remote_endpoint();}

    Connection(io_context& io, unsigned int port);
};


#endif //CHAT_CONNECTION_H
