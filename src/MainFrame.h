#pragma once
#include "App.h"
using namespace std;

const string path_buttons = "assert/buttons/";
enum {
    ID_KEY_LOGGER = wxID_HIGHEST + 1,
    ID_CAPTURE_SCREEN = wxID_HIGHEST + 2,
    ID_CAPTURE_WEBCAM = wxID_HIGHEST + 3,
    ID_MAC_ADDRESS = wxID_HIGHEST + 4,
    ID_DIRECTORY_TREE = wxID_HIGHEST + 5,
    ID_PROCESS = wxID_HIGHEST + 6,
    ID_REGISTRY = wxID_HIGHEST + 7,
    ID_LOGOUT = wxID_HIGHEST + 8
};

class RightPanel : public wxPanel {
public:
    RightPanel(wxWindow* parent);
    void UpdatePanelVisibility(int currentSelectedPanel);

private:
    wxTextCtrl* logTextCtrl{};
    wxPanel* buttonPanel{};

    wxTextCtrl* timeInputCtrl = nullptr;
    wxStaticText* timeLabel = nullptr;
    wxBoxSizer* timeInputSizer = nullptr;
    wxPanel* keyLoggerPanel = nullptr;

    void CreateKeyLogger();
    void OnCancelClick(wxCommandEvent& event);
    void OnSendClick(wxCommandEvent& event);
    void AppendLog(const wxString& message);
};

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE, const wxString &currentEmail, const wxString &serverAddress);

private:
    RightPanel* rightPanel;
    vector<wxPanel*> buttonPanels;
    wxPanel* currentSelectedPanel = nullptr;

    void OnButtonClick(wxCommandEvent& event);
    void HighlightButton(wxPanel* selectedPanel);
};