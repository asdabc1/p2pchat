#include "main.h"

using namespace boost::asio;

#define WinMain main
wxIMPLEMENT_APP(ChatApp);

enum {
   IDconnect = 1,
   IDdisconnect = 2,
   IDportChange = 3,
   IDaboutCon = 4,
   IDfont = 5,
   IDbackground = 6,
   IDdark = 9,
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

MainFrame::MainFrame(int port) : wxFrame(nullptr, wxID_ANY, "Chat", wxDefaultPosition), connection(io, port), messageOperations(io, std::chrono::seconds(1)) {
    Bind(wxEVT_THREAD, &MainFrame::messagePrint, this);

    connection.receiveConnection();

    messageOperations.async_wait([this](const std::error_code& e) { messageFunc(); });

    ioThread = std::thread([this](){io.run();});

    auto menuConnection = new wxMenu();
    menuConnection->Append(IDconnect, "&Connect\tCtrl-N");
    menuConnection->Append(IDdisconnect, "&Disconnect\tCtrl-Q");
    menuConnection->Append(IDportChange, "&Change used port\tCtrl-P");
    menuConnection->Append(IDaboutCon, "&About connection\tCtrl-I");

    auto menuAppearance = new wxMenu();
    menuAppearance->Append(IDfont, "&Font");
    menuAppearance->Append(IDbackground, "&Background");
    menuAppearance->Append(IDdark, "&Dark mode");

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
    Bind(wxEVT_MENU, &MainFrame::disconnect, this, IDdisconnect);
    Bind(wxEVT_MENU, &MainFrame::changeFont, this, IDfont);
    Bind(wxEVT_MENU, &MainFrame::changeBackground, this, IDbackground);
    Bind(wxEVT_MENU, &MainFrame::changePort, this, IDportChange);

    this->SetMinSize(wxSize(600, 450));

    auto sizer = new wxBoxSizer(wxVERTICAL);

    messageDisplay = new wxListBox(this, wxID_ANY);

    sizer->Add(messageDisplay, 1, wxEXPAND, 5);

    auto botSizer = new wxBoxSizer(wxHORIZONTAL);

    messageInput = new wxTextCtrl(this, wxID_ANY);
    messageSendButton = new wxButton(this, IDsend, "Send");
    this->SetDefaultItem(messageSendButton);

    botSizer->Add(messageInput, 1, wxALL, 5);
    botSizer->Add(messageSendButton, 0, wxALL, 5);

    sizer->Add(botSizer, 0, wxEXPAND, 5);

    this->SetSizer(sizer);

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
    auto dial = new PortChangeWindow(this);
    dial->Show(true);
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

void MainFrame::changeFont(wxCommandEvent &event) {

}

void MainFrame::changeBackground(wxCommandEvent &event) {

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
    messageOperations.cancel();
}

void MainFrame::sendButton(wxCommandEvent &event) {
    if (messageInput->IsEmpty())
        return;

    Message temp;
    messageDisplay->AppendString("You: " + messageInput->GetValue());
    temp << static_cast<std::wstring>(messageInput->GetValue());
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
    messageDisplay->AppendString("Chatter: " + event.GetPayload<Message>().wstring());

    event.Skip();
}

void MainFrame::messageFunc() {
    if(!outgoingMessages.queue.empty())
        sendMessage();

    if(!connection.qIsEmpty())
        messageReceived();

    messageOperations.expires_at(messageOperations.expiry() + std::chrono::milliseconds(200));
    messageOperations.async_wait([this](const std::error_code& e){ messageFunc(); });
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
    this->SetDefaultItem(okButton);

    Bind(wxEVT_BUTTON, &InitWindow::portProvided, this, 2137);
}

void InitWindow::portProvided(wxCommandEvent &event) {
    owner->port = std::stoi(static_cast<std::string>(portInput->GetValue()));
    this->Close(true);
}

PortChangeWindow::PortChangeWindow(MainFrame *owner) : wxDialog(nullptr, wxID_ANY, "Port selection", wxDefaultPosition, wxSize(200,140)), owner(owner) {
    auto portLabel = new wxStaticText(this, wxID_ANY, "Port: ", wxPoint(10, 10));
    portInput = new wxTextCtrl(this, wxID_ANY, "", wxPoint(50, 10));

    auto okButton = new wxButton(this, 2140, "Ok", wxPoint(140, 70), wxSize(40, -1));

    Bind(wxEVT_BUTTON, &PortChangeWindow::portProvided, this, 2140);
}

void PortChangeWindow::portProvided(wxCommandEvent &event) {
    owner->portChange(std::stoi(static_cast<std::string>(portInput->GetValue())));
    this->Close();
}
