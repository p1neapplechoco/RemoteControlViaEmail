#pragma once
#include "App.h"
#include "LogPanel.h"
using namespace std;

class LogPanel;

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE, const wxString &currentEmail,
        const wxString &currentPassword, const wxString &IP_Address);

    void EnableSideButtons(bool enable);
    void UpdateStatusBar(const wxString& text);

private:
    LogPanel* rightPanel;
    vector<wxPanel*> buttonPanels;
    wxPanel* currentSelectedPanel = nullptr;
    vector<wxBitmapButton*> sideButtons;

    void OnButtonClick(wxCommandEvent& event);
    void HighlightButton(wxPanel* selectedPanel);
    bool ConnectToServer(const wxString &currentEmail, const wxString &serverAddress, const wxString &portStr);
};