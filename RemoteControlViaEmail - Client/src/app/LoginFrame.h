#pragma once
#include "App.h"
#include "../utils/Email.h"

using namespace std;

class LoginFrame : public wxFrame {
public:
    LoginFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE);

private:
    UserCredentials userCredentials;
    wxComboBox* ipComboBox;
    wxPanel* bottomPanel;
    wxPanel* googlePanel;
    wxPanel* ipPanel;
    wxBoxSizer* bottomSizer;
    wxString currentEmail, currentPassword;

    Client client;

    void OnGoogleLogin(wxCommandEvent& evt);
    void OnNoAccountLogin(wxCommandEvent& evt);
    void OnRefreshClick(wxCommandEvent& evt);
    void OnBackClick(wxCommandEvent& evt);
    void OnConnectClick(wxCommandEvent& evt);

    bool UpdateIPList();
    void ShowGooglePanel();
    void ShowIPPanel();

    void OnButtonClick(wxCommandEvent& event);
};
