//
// Created by asdab on 21.03.2026.
//

#include "MessageBubble.h"

void MessageBubble::onPaint(wxPaintEvent &event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    auto gc = wxGraphicsContext::Create(dc);

    gc->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL), wxColour(255, 255, 255));

    double w, h;
    gc->GetTextExtent(text, &w, &h);

    wxBrush blue = wxBrush(wxColour(0, 29, 45), wxBRUSHSTYLE_SOLID);
    wxBrush green = wxBrush(wxColour(0, 74, 5), wxBRUSHSTYLE_SOLID);

    gc->SetPen(*wxTRANSPARENT_PEN);

    if (type == MessageBubbleType::own)
        gc->SetBrush(blue);

    else
        gc->SetBrush(green);


    gc->DrawRoundedRectangle(0, 0, w + 20, h + 5, 15);

    gc->SetPen(*wxWHITE_PEN);
    gc->DrawText(text, 10, 5);
}

MessageBubble::MessageBubble(const Message &msg, MessageBubbleType type, wxWindow* parent) : wxPanel(parent) {
    this->type = type;
    text = wxString(msg.wstring());

    wxClientDC dc(this);
    auto gc = wxGraphicsContext::Create(dc);
    gc->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL), wxColour(255, 255, 255));
    double w, h;
    gc->GetTextExtent(text, &w, &h);
    this->SetMinSize(wxSize(w + 30,h + 10));

    wxWindowBase::SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetDoubleBuffered(true);
    Bind(wxEVT_PAINT, &MessageBubble::onPaint, this);
}
