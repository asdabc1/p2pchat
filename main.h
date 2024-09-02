#ifndef CHAT_MAIN_H
#define CHAT_MAIN_H

#include <string>
#include <thread>
#include <chrono>
#include <map>
#include <sstream>

#include <boost/asio.hpp>

#include <wx/wx.h>
#include <wx/fontdlg.h>

#include "Connection.h"
#include "MsgQ.h"

class ChatApp : public wxApp {
    bool OnInit() override;

public:
    int port;

};

class MainFrame : public wxFrame {
public:
    explicit MainFrame(int port);
    ~MainFrame() override;

    void connect(const char* address, int port);
    void portChange(int port) {connection.changePort(port);}

private:

    io_context io;
    executor_work_guard<decltype(io.get_executor())> work{io.get_executor()};
    Connection connection;
    MsgQ outgoingMessages;

    bool threadsWork = true;
    std::thread ioThread;
    std::thread messageSend;
    std::thread messageReceive;

    wxListBox* messageDisplay;
    wxTextCtrl* messageInput;
    wxButton* messageSendButton;

    void connect(wxCommandEvent& event);
    void disconnect(wxCommandEvent& event);
    void changePort(wxCommandEvent& event);
    void aboutConnection(wxCommandEvent& event);
    void changeFont(wxCommandEvent& event);
    void changeBackground(wxCommandEvent& event);
    void helpPort(wxCommandEvent& event);
    void helpAbout(wxCommandEvent& event);

    void sendButton(wxCommandEvent& event);

    void sendMessage();
    void messageReceived();
    void messagePrint(wxThreadEvent& event);
};

class NewConnectionDialog : public wxDialog {
public:
    NewConnectionDialog(MainFrame* parent, wxWindowID id);

private:
    wxTextCtrl* IPInput;
    wxTextCtrl* portInput;
    MainFrame* parent;

    void onOkButton(wxCommandEvent& event);
};

class InitWindow : public wxDialog {
public:
    explicit InitWindow(ChatApp* owner);

private:

    wxTextCtrl* portInput = nullptr;
    ChatApp* owner;

    void portProvided(wxCommandEvent& event);
};

class PortChangeWindow : public wxDialog {
public:
    explicit PortChangeWindow(MainFrame* owner);

private:
    wxTextCtrl* portInput = nullptr;
    MainFrame* owner;

    void portProvided(wxCommandEvent& event);
};

#endif //CHAT_MAIN_H