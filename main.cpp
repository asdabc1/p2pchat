#include "main.h"

using namespace boost::asio;

#define WinMain main
wxIMPLEMENT_APP(ChatApp);

enum {
   IDconnect = 1,
   IDdisconnect = 2,
   IDportChange = 3,
   IDaboutCon = 4,
   IDfontSize = 5,
   IDfontColor = 6,
   IDaboutPort = 7,

   IDsend = 8,
};

bool ChatApp::OnInit() {
    auto init = new InitWindow(this);
    init->ShowModal();

    auto frame = new MainFrame(port);
    frame->Show(true);
    return true;
}

MainFrame::MainFrame(int port) : wxFrame(nullptr, wxID_ANY, "Chat", wxDefaultPosition, wxSize(1150, 650)), connection(io, port) {
    Bind(wxEVT_THREAD, &MainFrame::messagePrint, this);

    connection.receiveConnection();
    ioThread = std::thread([this](){io.run();});
    messageSend = std::thread([this](){
        while(threadsWork)
            if (!outgoingMessages.queue.empty())
                sendMessage();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });
    messageReceive = std::thread([this]() {
        while(threadsWork)
            if(!connection.qIsEmpty())
                messageReceived();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });

    auto menuConnection = new wxMenu();
    menuConnection->Append(IDconnect, "&Connect\tCtrl-N");
    menuConnection->Append(IDdisconnect, "&Disconnect\tCtrl-Q");
    menuConnection->Append(IDportChange, "&Change used port\tCtrl-P");
    menuConnection->Append(IDaboutCon, "&About connection\tCtrl-I");

    auto menuAppearance = new wxMenu();
    menuAppearance->Append(IDfontSize, "&Font size");
    menuAppearance->Append(IDfontColor, "&Font color");

    auto menuApp = new wxMenu();
    menuApp->Append(IDaboutPort, "&About port selection");
    menuApp->Append(wxID_ABOUT);
    menuApp->Append(wxID_EXIT);

    auto bar = new wxMenuBar();
    bar->Append(menuConnection, "&Connection");
    bar->Append(menuAppearance, "&Appearance");
    bar->Append(menuApp, "&Application");

    SetMenuBar(bar);

    Bind(wxEVT_MENU, [=](wxCommandEvent& event){Close(true); io.stop(); ioThread.join();}, wxID_EXIT);
    Bind(wxEVT_MENU, &MainFrame::aboutConnection, this, IDaboutCon);
    Bind(wxEVT_MENU, &MainFrame::helpAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrame::helpPort, this, IDaboutPort);
    Bind(wxEVT_MENU, &MainFrame::connect, this, IDconnect);

    messageDisplay = new wxListBox(this, wxID_ANY, wxPoint(10, 10), wxSize(800, 500));
    messageInput = new wxTextCtrl(this, wxID_ANY, "",wxPoint(10, 525), wxSize(725, -1));
    messageSendButton = new wxButton(this, IDsend, "Send", wxPoint(750, 525), wxSize(65, -1));

    Bind(wxEVT_BUTTON, &MainFrame::sendButton, this, IDsend);

    m_backgroundColour = wxColour(243,242,255);

}

void MainFrame::connect(wxCommandEvent &event) {
    auto dial = new NewConnectionDialog(this, IDconnect);
    dial->Show(true);
}

void MainFrame::disconnect(wxCommandEvent &event) {
    connection.disconnect();
}

void MainFrame::changePort(wxCommandEvent &event) {

}

void MainFrame::aboutConnection(wxCommandEvent &event) {
    std::stringstream s;
    std::string temp;
    if (connection.isUp()) {
        s << connection.chatter();
        s >> temp;
    }

    wxMessageBox(connection.isUp() ? "Connected to host: " + temp : "not connected", "Connection information", wxOK | wxICON_INFORMATION);
}

void MainFrame::changeFontSize(wxCommandEvent &event) {

}

void MainFrame::changeFontColor(wxCommandEvent &event) {

}

void MainFrame::helpPort(wxCommandEvent &event) {
    wxMessageBox("The application allows the user to manually select over which port they will be communicating.\nThe port choice should fall outside the scope of well-known ports, as they are likely to be already in use.\nThe well-known ports are numbered 1-1023. Ports in general are numbered 1-65535.", "About ports", wxOK | wxICON_INFORMATION);
}

void MainFrame::helpAbout(wxCommandEvent &event) {
    wxMessageBox("Simple chat application.\nCreated by Adam Bozek.\nThe messages are NOT encrypted! Please refrain from sharing any sensitive information!", "About", wxOK | wxICON_INFORMATION);
}

void MainFrame::connect(const char* address, int port) {
    connection.connect(address, port);
}

MainFrame::~MainFrame() {
    io.stop();
    ioThread.join();

    threadsWork = false;
    messageSend.join();
    messageReceive.join();
}

void MainFrame::sendButton(wxCommandEvent &event) {
    Message temp;
    messageDisplay->AppendString("You: " + messageInput->GetValue());
    temp << static_cast<std::string>(messageInput->GetValue());
    messageInput->Clear();

    outgoingMessages.addToQueue(temp);

    event.Skip();
}

void MainFrame::sendMessage() {
    connection.sendHeader(outgoingMessages.getFromQueue());
}

void MainFrame::messageReceived() {
    auto event = new wxThreadEvent();
    event->SetPayload(connection.retreiveMsgFromQueue());
    wxQueueEvent(this, event);
}

void MainFrame::messagePrint(wxThreadEvent& event) {
    messageDisplay->AppendString("Chatter: " + event.GetPayload<Message>().string());

    event.Skip();
}

NewConnectionDialog::NewConnectionDialog(MainFrame *parent, wxWindowID id) : wxDialog(parent, id, "New connection", wxDefaultPosition, wxSize(200,140)), parent(parent) {
    auto IPLabel = new wxStaticText(this, wxID_ANY, "IP address: ", wxPoint(10, 10));
    IPInput = new wxTextCtrl(this, wxID_ANY, "", wxPoint(70, 10));

    auto portLabel = new wxStaticText(this, wxID_ANY, "Port: ", wxPoint(10, 40));
    portInput = new wxTextCtrl(this, wxID_ANY, "", wxPoint(70, 40));

    auto okButton = new wxButton(this, 2138, "Ok", wxPoint(95, 70), wxSize(40, -1));
    auto cancelButton = new wxButton(this, 2139, "Cancel", wxPoint(140, 70), wxSize(40, -1));

    Bind(wxEVT_BUTTON, &NewConnectionDialog::onOkButton, this, 2138);
    Bind(wxEVT_BUTTON, [=](wxCommandEvent& event) {this->Close(true);}, 2139);
}

void NewConnectionDialog::onOkButton(wxCommandEvent &event) {
    parent->connect(IPInput->GetValue().c_str(), std::stoi(static_cast<std::string>(portInput->GetValue())));
    this->Close(false);
}

InitWindow::InitWindow(ChatApp *owner) : wxDialog(nullptr, wxID_ANY, "Port selection", wxDefaultPosition, wxSize(200,140)), owner(owner) {
    auto portLabel = new wxStaticText(this, wxID_ANY, "Port: ", wxPoint(10, 10));
    portInput = new wxTextCtrl(this, wxID_ANY, "", wxPoint(50, 10));

    auto okButton = new wxButton(this, 2137, "Ok", wxPoint(140, 70), wxSize(40, -1));

    Bind(wxEVT_BUTTON, &InitWindow::portProvided, this, 2137);
}

void InitWindow::portProvided(wxCommandEvent &event) {
    owner->port = std::stoi(static_cast<std::string>(portInput->GetValue()));
    this->Close(true);
}
