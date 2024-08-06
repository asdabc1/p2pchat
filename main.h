#ifndef CHAT_MAIN_H
#define CHAT_MAIN_H

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <map>
#include <sstream>

#include <boost/asio.hpp>

#include <wx/wx.h>

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
    ~MainFrame();

    void connect(const char* address, int port);

private:

    io_context io;
    executor_work_guard<decltype(io.get_executor())> work{io.get_executor()};
    Connection connection;

    std::thread ioThread;

    void connect(wxCommandEvent& event);
    void disconnect(wxCommandEvent& event);
    void changePort(wxCommandEvent& event);
    void aboutConnection(wxCommandEvent& event);
    void changeFontSize(wxCommandEvent& event);
    void changeFontColor(wxCommandEvent& event);
    void helpPort(wxCommandEvent& event);
    void helpAbout(wxCommandEvent& event);
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
    InitWindow(ChatApp* owner);

private:

    wxTextCtrl* portInput = nullptr;
    ChatApp* owner;

    void portProvided(wxCommandEvent& event);
};

#endif //CHAT_MAIN_H