#pragma once
#include <vector>
#include <string>
#include <utility>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/progdlg.h>
#include <wx/bmpbuttn.h>

#include "EmailRetrieval.h"
#include "IniParser.h"
#include "MainFrame.h"
#include "LoginFrame.h"
using namespace std;

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

class App : public wxApp {
public:
    bool OnInit();
};