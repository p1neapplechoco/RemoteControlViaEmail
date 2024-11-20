#pragma once
#include "App.h"
using namespace std;

class LoginFrame : public wxFrame {
public:
    LoginFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE);

private:
    wxComboBox* ipComboBox;
    wxTextCtrl* portTextCtrl;

    wxPanel* bottomPanel;
    wxPanel* googlePanel;
    wxPanel* ipPanel;
    wxBoxSizer* bottomSizer;
    wxString currentEmail;

    Client client;

    void OnGoogleLogin(wxCommandEvent& evt);
    void OnNoAccountLogin(wxCommandEvent& evt);
    void OnRefreshClick(wxCommandEvent& evt);
    void OnBackClick(wxCommandEvent& evt);
    void OnConnectClick(wxCommandEvent& evt);

    bool UpdateIPList();
    void ShowGooglePanel();
    void ShowIPPanel();

    void OnButtonClick(wxCommandEvent& event) {
        wxMessageBox("Button clicked!", "Info");
    };
};

class GmailLoginDialog : public wxDialog {
public:
    GmailLoginDialog(wxWindow* parent);
    bool IsLoginSuccessful() const { return loginSuccessful; }
    wxString GetEmail() const { return emailInput->GetValue(); }

private:
    wxTextCtrl* emailInput;
    wxTextCtrl* passwordInput;
    wxBitmapButton* loginButton;
    bool loginSuccessful = false;
    wxString email;

    void OnLogin(wxCommandEvent& evt);
    bool AuthenticateUser(const wxString& email, const wxString& password);
};
