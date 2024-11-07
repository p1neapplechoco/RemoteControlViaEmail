#pragma once
#include <wx/wx.h>
#include <wx/statline.h>

class LoginFrame : public wxFrame {
public:
    LoginFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE);
    // wxString GetIP() const { return ipInput->GetValue(); }
    // wxString GetPort() const { return portInput->GetValue(); }

private:
    // wxTextCtrl* ipInput{};
    // wxTextCtrl* portInput{};

    // void OnConnect(wxCommandEvent& event);

    void OnButtonClick(wxCommandEvent& event) {
        wxMessageBox("Button clicked!", "Info");
    }
};
