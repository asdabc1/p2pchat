#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <vector>
#include <string>

enum class types {
    message,
    null
};

struct header {
    size_t size;
    types messageType;

    explicit header(size_t size) : size(size) {messageType = types::null;}
    header() : size(0), messageType(types::null) {}
};

const size_t HEADER_SIZE = sizeof(header);

class Message {
public:

    header head;
    std::vector<uint8_t> body;

    void update() {body.resize(this->head.size);}

    Message() = default;
    Message(types type, uint8_t val);
    explicit Message(size_t size) : head(size) {this->update();}

    Message& operator<<(std::wstring input);
    bool operator==(const Message& otherMsg) const;

    [[nodiscard]] std::wstring wstring() const;
};

#endif //CHAT_MESSAGE_H
