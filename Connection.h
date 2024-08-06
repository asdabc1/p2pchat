#ifndef CHAT_CONNECTION_H
#define CHAT_CONNECTION_H

#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <sstream>

#include <wx/wx.h>

#include "Message.h"
#include "MsgQ.h"

using namespace boost::asio;

class ConnectionAcceptDialog;

class Connection : public wxEvtHandler {

    friend class ConnectionAcceptDialog;

private:

    io_context& io;
    ip::tcp::socket soc;
    ip::tcp::socket tempAcceptorSocket;
    ip::tcp::acceptor acceptor;

    Message temp;
    MsgQ queue;

    bool isConnected = false;

public:
    void receiveHeader();
    void receiveBody();
    void sendHeader(Message msg);
    void sendBody(Message msg);

    void connect(const char* address, unsigned int port);
    void receiveConnection();
    void disconnect();
    void changePort(int port);

    void connectionReceived(wxThreadEvent& event);

    bool isUp() const {return isConnected;}
    bool socIsOpen() const {return soc.is_open();}

    bool qIsEmpty() const {return queue.queue.empty();}
    Message retreiveMsgFromQueue() {return queue.getFromQueue();}

    ip::tcp::endpoint chatter() {return soc.remote_endpoint();}

    Connection(io_context& io, unsigned int port);
    Connection();
};

class ConnectionAcceptDialog  : public wxDialog {
public:
    ConnectionAcceptDialog(Connection* con, ip::tcp::socket);

private:
    Connection* connection;
    ip::tcp::socket tempSocket;

    void onAccept(wxCommandEvent& event);
    void onReject(wxCommandEvent& event);
};

#endif //CHAT_CONNECTION_H