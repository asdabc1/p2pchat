#include "Connection.h"

Connection::Connection(io_context& io, unsigned int port) : io(io), soc(io), tempAcceptorSocket(io) , acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), port)) {
    Bind(wxEVT_THREAD, &Connection::connectionReceived, this);

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
    acceptor.async_accept(io, [this](const std::error_code& ec, ip::tcp::socket socket) {
        if (!ec) {
            tempAcceptorSocket = std::move(socket);

            auto event = new wxThreadEvent();
            event->SetPayload(this);
            wxQueueEvent(this, event);
        }
        else {
            wxMessageBox("Error with connection reception:" + ec.message(), "Error", wxCLOSE | wxICON_ERROR);
        }
        receiveConnection();
    });

}


ConnectionAcceptDialog::ConnectionAcceptDialog(Connection* con, ip::tcp::socket socket) : wxDialog(nullptr, wxID_ANY, "New connection pending", wxDefaultPosition, wxSize(350, 350)), tempSocket(std::move(socket)) {
    connection = con;

    std::stringstream temp;
    std::string remoteAddress;

    temp << tempSocket.remote_endpoint();
    temp >> remoteAddress;

    auto connectInfo = new wxStaticText(this, wxID_ANY, "New connection request from: " + remoteAddress, wxPoint(10, 10));
    auto accept = new wxButton(this, 2140, "Accept", wxPoint(10, 250), wxSize(50, -1));
    auto reject = new wxButton(this, 2141, "Reject", wxPoint(250, 250), wxSize(50, -1));

    Bind(wxEVT_BUTTON, &ConnectionAcceptDialog::onAccept, this, 2140);
    Bind(wxEVT_BUTTON, &ConnectionAcceptDialog::onReject, this, 2141);
    }

void ConnectionAcceptDialog::onAccept(wxCommandEvent& event) {
    connection->soc = std::move(tempSocket);
    connection->isConnected = true;
    connection->receiveHeader();
    this->Close(false);
}

void ConnectionAcceptDialog::onReject(wxCommandEvent& event) {
    this->Close(true);
}


void Connection::disconnect() {
    soc.close();

    wxMessageBox("Disconnected", "", wxOK | wxICON_EXCLAMATION);

    isConnected = false;
}

void Connection::receiveHeader() {
    async_read(soc, buffer(&temp.head, HEADER_SIZE), [this](const std::error_code& ec, size_t length) {
        if (!ec) {
            temp.update();
            receiveBody();
        }

        else {
            std::cout << "Failed to receive the header of the message: " << ec.message() << '\n';
            disconnect();
        }
    });
}

void Connection::receiveBody() {
    async_read(soc, buffer(temp.body.data(), temp.head.size), [this](const std::error_code& ec, size_t length){
        if (!ec) {
            queue.addToQueue(temp);
            temp = Message();
            receiveHeader();
        }

        else {
            std::cout << "Failed to receive the contents of the message: " << ec.message() << '\n';\
            disconnect();
        }
    });
}

void Connection::sendHeader(Message msg) {
    async_write(soc, buffer(&msg.head, HEADER_SIZE), [this, msg](const std::error_code& ec, size_t length) {
        if (!ec) {
            sendBody(msg);
        }
        else {
            std::cout << "Failed to send the header of a message: " << ec.message() << std::endl;
            disconnect();
        }
    });
}

void Connection::sendBody(Message msg) {
    async_write(soc, buffer(msg.body.data(), msg.head.size), [this](const std::error_code& ec, size_t length) {
        if (ec) {
            std::cout << "Failed to send the contents of the message: " << ec.message() << '\n';
            disconnect();
        }
    });
}

void Connection::changePort(int port) {
    ip::address_v4 add = ip::address_v4::from_string("127.0.0.1");

    soc.bind(ip::tcp::endpoint(add, port));
}

void Connection::connectionReceived(wxThreadEvent &event) {
    auto dial = new ConnectionAcceptDialog(this, std::move(this->tempAcceptorSocket));
    dial->Show(true);
}
