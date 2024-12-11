#pragma once
#include "App.h"

#ifndef CUSTOMBITMAPBUTTON_H
#define CUSTOMBITMAPBUTTON_H

class CustomBitmapButton : public wxBitmapButton {
public:
    CustomBitmapButton(wxWindow *parent, wxWindowID id, const wxString &nameImage, const wxPoint &pos, const wxSize &size);

    CustomBitmapButton(wxWindow *parent, wxWindowID id, const wxString &nameImage)
        : CustomBitmapButton(parent, id, nameImage, wxDefaultPosition, wxDefaultSize) {}

    ~CustomBitmapButton();

private:
    wxBitmap m_normalBitmap;
    wxBitmap m_hoverBitmap;
    wxBitmap m_pressedBitmap;

    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
};



#endif //CUSTOMBITMAPBUTTON_H
