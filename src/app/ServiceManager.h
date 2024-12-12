#pragma once
#include "App.h"
using namespace std;

class ServiceManager : public wxPanel {
public:
    ServiceManager(wxWindow* parent);
    void LoadServiceFromFile(const wxString& filename);

private:
    wxMenu* contextMenu;
    wxListCtrl* serviceList;
    wxStaticText* totalServiceText;

    void AddServiceToPage(const wxString& service_name, const wxString& display_name, const wxString& state);

    void OnEndTask(wxCommandEvent& event);

    void OnStartTask(wxCommandEvent& event);

    void OnRightClick(wxListEvent& event);

    wxString trim(const wxString& str);

    wxString removeExtraSpaces(const wxString& str);

    wxString processLine(const wxString& line);

    wxDECLARE_EVENT_TABLE();
};


