#pragma once
#include "App.h"
using namespace std;

class ProcessManager : public wxPanel {
public:
    ProcessManager(wxWindow* parent);
    void LoadProcessesFromFile(const wxString& filename);

private:
    wxMenu* contextMenu;

    wxListbook* processBook;
    wxListCtrl* appsPage;
    wxListCtrl* backgroundPage;
    wxListCtrl* windowsPage;
    wxStaticText* totalProcessesText;

    void CreateProcessPages();
    void AddProcessToPage(wxListCtrl* page, const wxString& pid, const wxString& name);
    void OnRightClick(wxListEvent& event);
    void OnEndTask(wxCommandEvent& event);

    wxString trim(const wxString& str);
    wxString removeExtraSpaces(const wxString& str);
    wxString processLine(const wxString& line);

    wxDECLARE_EVENT_TABLE();
};

