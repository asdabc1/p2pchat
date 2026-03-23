Simple chat application project
===============================

Technologies and libraries used
-------------------------------

1. **C++**
2. **Boost Asio** - used for socket based network programming.
3. **wxWidgets** - used for creating the UI. I am aware it is quite obsolete, but I opted for it for its simplicity, as it allowed me to create the UI using only C++ code.

Functionalities
---------------

The app allows you to communicate with another host (be it local or remote), by providing their port and ip address. When running the application, you are required to provide a port on which the application will be functioning.
If you produce a port number that is occupied (ie. cannot be claimed by the application), it will crash - no point in running without a socket! If you successfully enter the port number to be used, the application runs an acceptor
in the background. Should a connection request come, the user will be prompted to either accept it or reject. If accepted, the users can now exchange messages.

Screenshots
-----------
The program begins by displaying the port selection dialog modally.

![Image](readme-screenshots/port.png)

Then the user can connect to another instance of the application, by selecting Connection>Connect (or Ctrl-N)

![image](readme-screenshots/connect.png)

The other user is prompted to accept (or reject) the incoming connection

![image](readme-screenshots/prompt.png)

The users can now exchange messages!

![image](readme-screenshots/chat.png)

Architecture
------------

The networking is done using Boost.asio's sockets. Incoming and outgoing messages are stored in queues, which are checked roughly every 200 milliseconds. If there are messages in a queue, they are processed by their respective functions.
The function for receiving a message sends a wx thread event to the main thread, as the messages are checked inside the io_context thread - message to be displayed must be passed.

```C++
void MainFrame::messageReceived() {
    auto event = new wxThreadEvent();
    event->SetPayload(connection.retreiveMsgFromQueue());
    wxQueueEvent(this, event);
}
```

the ~200 milliseconds interval is achieved through an async timer checking the queues when it expires.

```C++
void MainFrame::messageFunc() {
    if(!outgoingMessages.queue.empty())
        sendMessage();

    if(!connection.qIsEmpty())
        messageReceived();

    messageOperations.expires_at(messageOperations.expiry() + std::chrono::milliseconds(200));
    messageOperations.async_wait([this](const std::error_code& e){ messageFunc(); });
}
```

The base sending and receiving of data (or segments, as it's over TCP) is done through the Connection class methods:

```C++
void Connection::receiveHeader() {
    async_read(soc, buffer(&temp.head, HEADER_SIZE), [this](const std::error_code& ec, size_t length) {
        if (!ec) {
            temp.update();
            receiveBody();
        }

        else {
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
            disconnect();
        }
    });
}

void Connection::sendBody(Message msg) {
    async_write(soc, buffer(msg.body.data(), msg.head.size), [this](const std::error_code& ec, size_t length) {
        if (ec) {
            disconnect();
        }
    });
}
```