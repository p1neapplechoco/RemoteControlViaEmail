#pragma once
#include "App.h"
using namespace std;

class NetworkDiscovery {
public:
    NetworkDiscovery();
    ~NetworkDiscovery();
    void sendBroadcast();
    vector<string> getDiscoveredIPs() { return discoveredIPs; }
    void listenForResponses(int timeoutSeconds = 5);

private:
    SOCKET broadcastSocket;
    const int PORT = 45678;
    const std::string DISCOVERY_MESSAGE = "DISCOVER_REQUEST";
    vector<string> discoveredIPs;
};

class LoginFrame : public wxFrame {
public:
    LoginFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE);

private:
    wxComboBox* ipComboBox;
    wxPanel* bottomPanel;
    wxPanel* googlePanel;
    wxPanel* ipPanel;
    wxBoxSizer* bottomSizer;
    NetworkDiscovery* networkDiscovery = new NetworkDiscovery();
    wxString currentEmail;

    void OnGoogleLogin(wxCommandEvent& evt);
    void OnNoAccountLogin(wxCommandEvent& evt);
    void OnRefreshClick(wxCommandEvent& evt);
    void OnBackClick(wxCommandEvent& evt);
    void OnConnectClick(wxCommandEvent& evt);
    void UpdateIPList();
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

    void OnLogin(wxCommandEvent& evt);
};
