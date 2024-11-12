#pragma once
#include "App.h"
using namespace std;

const string path_buttons = "assert/buttons/";

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString &TITLE, const wxPoint &POS, const wxSize &SIZE, const wxString &currentEmail, const wxString &serverAddress);
private:
    vector<wxPanel*> buttonPanels;
    wxPanel* currentSelectedPanel = nullptr;

    void OnButtonClick(wxCommandEvent& event) {
        wxMessageBox("Button clicked!", "Info");
    }
    void HighlightButton(wxPanel* selectedPanel);
};