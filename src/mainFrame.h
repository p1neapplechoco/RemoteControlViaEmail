#pragma once
#include <wx/wx.h>
#include <wx/bmpbuttn.h>
using namespace std;

const string path_buttons = "assert/buttons/";

class CustomBitmapButton : public wxBitmapButton {
public:
    CustomBitmapButton(wxWindow* parent,
        wxWindowID id,
        const wxString& nameImage,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize)
        : wxBitmapButton(parent, id, wxBitmap(), pos, size, wxBORDER_NONE)
    {
        // Load images with transparency
        m_normalBitmap = wxBitmap(path_buttons + nameImage + "_normal.png", wxBITMAP_TYPE_PNG);


        m_hoverBitmap = wxBitmap(path_buttons + nameImage + "_hover.png", wxBITMAP_TYPE_PNG);


        m_pressedBitmap = wxBitmap(path_buttons + nameImage + "_pressed.png", wxBITMAP_TYPE_PNG);


        SetBitmap(m_normalBitmap); // Set initial bitmap

        // Bind events
        Bind(wxEVT_ENTER_WINDOW, &CustomBitmapButton::OnMouseEnter, this);
        Bind(wxEVT_LEAVE_WINDOW, &CustomBitmapButton::OnMouseLeave, this);
        Bind(wxEVT_LEFT_DOWN, &CustomBitmapButton::OnMouseDown, this);
        Bind(wxEVT_LEFT_UP, &CustomBitmapButton::OnMouseUp, this);
    }

    ~CustomBitmapButton() {
        //Delete the masks to prevent memory leaks
        delete m_normalBitmap.GetMask();
        delete m_hoverBitmap.GetMask();
        delete m_pressedBitmap.GetMask();
    }

private:
    wxBitmap m_normalBitmap;
    wxBitmap m_hoverBitmap;
    wxBitmap m_pressedBitmap;

    void OnMouseEnter(wxMouseEvent& event) {
        SetBitmap(m_hoverBitmap);
        Refresh();
    }

    void OnMouseLeave(wxMouseEvent& event) {
        SetBitmap(m_normalBitmap);
        Refresh();
    }

    void OnMouseDown(wxMouseEvent& event) {
        SetBitmap(m_pressedBitmap);
        Refresh();
        event.Skip();
    }

    void OnMouseUp(wxMouseEvent& event) {
        if (GetClientRect().Contains(event.GetPosition()))
            SetBitmap(m_hoverBitmap);
        else
            SetBitmap(m_normalBitmap);
        Refresh();
        event.Skip();
    }
};

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE);
private:
    void OnButtonClick(wxCommandEvent& event)
    {
        wxMessageBox("Button clicked!", "Info");
    }
};

// class MessengerFrame : public wxFrame {
// public:
//     MessengerFrame(const wxString &title);
//
// private:
//     wxListCtrl *messageList;
//     wxTextCtrl *messageInput;
//     std::vector<std::string> messages;
//
//     void OnSendClicked(wxCommandEvent &event);
//     void UpdateMessageList();
// };

// class MainFrame : public wxFrame {
//     public:
//         MainFrame(const wxString& title);
//
//     private:
//         void OnButtonClicked(wxCommandEvent& evt);
//         void OnTextChanged(wxCommandEvent& evt);
//         wxDECLARE_EVENT_TABLE();
// };