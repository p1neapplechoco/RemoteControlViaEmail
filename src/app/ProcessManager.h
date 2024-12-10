#pragma once
#include "App.h"
using namespace std;

class ProcessManager : public wxPanel {
public:
    ProcessManager(wxWindow* parent);
    void LoadProcessesFromLog(const wxString& filename);

private:
    wxMenu* contextMenu;

    wxListbook* processBook;
    wxListCtrl* appsPage;
    wxListCtrl* backgroundPage;
    wxListCtrl* windowsPage;
    wxStaticText* totalProcessesText;

    void CreateProcessPages();
    void ParseLogFile(const wxString& filename);
    void AddProcessToPage(wxListCtrl* page, const wxString& pid, const wxString& name);
    void OnRightClick(wxListEvent& event);
    void OnEndTask(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

