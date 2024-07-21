#ifndef CHAT_CONNECTION_H
#define CHAT_CONNECTION_H

#include <boost/asio.hpp>
#include <iostream>

#include "Message.h"
#include "MsgQ.h"

using namespace boost::asio;

class Connection {

private:

    io_context& io;
    ip::tcp::socket soc;

    Message temp;
    MsgQ queue;

public:
    void receiveHeader();
    void receiveBody();
    void sendHeader();
    void sendBody();

    void connect(const char* address, int port);
    void disconnect();
    bool isUp() {return soc.is_open();}

    Connection(io_context& io, unsigned int port);
};


#endif //CHAT_CONNECTION_H
