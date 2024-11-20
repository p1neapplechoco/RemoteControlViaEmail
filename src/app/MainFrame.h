#pragma once
#include "App.h"
using namespace std;

const string path_buttons = "assert/buttons/";
enum {
    ID_KEY_LOGGER = wxID_HIGHEST + 1,
    ID_CAPTURE_SCREEN,
    ID_CAPTURE_WEBCAM,
    ID_MAC_ADDRESS,
    ID_DIRECTORY_TREE,
    ID_PROCESS,
    ID_REGISTRY,
    ID_LOGOUT,
    ID_EXIT,
    ID_TEAM,
    ID_INSTRUCTION
};

class RightPanel : public wxPanel {
public:
    RightPanel(wxWindow* parent);
    wxPanel* buttonPanel;
    wxPanel* keyLoggerPanel;
    wxPanel* keyLogoutPanel;
    void UpdatePanelVisibility(int currentSelectedPanel);
    void AppendLog(const wxString& message);

private:
    wxTextCtrl* logTextCtrl{};
    wxTextCtrl* timeInputCtrl = nullptr;
    wxChoice* choice = nullptr;
    wxBoxSizer* timeInputSizer = nullptr;

    void CreateKeyLogger();
    void CreateKeyLogout();
    void OnCancelClick(wxCommandEvent& event);
    void OnSendClick(wxCommandEvent& event);
};

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE, const wxString &currentEmail,
        const wxString &serverAddress, const wxString &portStr);

private:
    RightPanel* rightPanel;
    vector<wxPanel*> buttonPanels;
    wxPanel* currentSelectedPanel = nullptr;

    Client client;

    void OnButtonClick(wxCommandEvent& event);
    void HighlightButton(wxPanel* selectedPanel);
    bool ConnectToServer(const wxString &currentEmail, const wxString &serverAddress, const wxString &portStr);
};