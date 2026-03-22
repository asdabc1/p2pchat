//
// Created by asdab on 21.03.2026.
//

#ifndef CHAT_MESSAGEBUBBLE_H
#define CHAT_MESSAGEBUBBLE_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <locale>
#include <codecvt>

#include "../Message.h"

enum class MessageBubbleType {
    own,
    external
};

class MessageBubble : public wxPanel {
    void onPaint(wxPaintEvent& event);

    wxString text;
    MessageBubbleType type;
public:
    MessageBubble(const Message& msg, MessageBubbleType type, wxWindow* parent);
    bool isExternal() {
        return type == MessageBubbleType::external;
    }
};


#endif //CHAT_MESSAGEBUBBLE_H