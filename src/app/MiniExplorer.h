#pragma once
#include "App.h"
using namespace std;

class MiniExplorer : public wxFrame {
public:
    MiniExplorer(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnRename(wxCommandEvent& event);

    wxTreeCtrl* dirTree;
    wxListCtrl* fileList;

    wxDECLARE_EVENT_TABLE();
};