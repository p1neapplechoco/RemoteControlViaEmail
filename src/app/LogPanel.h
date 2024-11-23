#pragma once
#include "App.h"
#ifndef LOGPANEL_H
#define LOGPANEL_H

enum {
    ID_LIST_PROCESSES = wxID_HIGHEST + 1,
    ID_LIST_SERVICES,
    ID_SCREENSHOT,
    ID_TOGGLE_WEBCAM,
    ID_CAPTURE_WEBCAM,
    ID_END_PROCESS,
    ID_END_SERVICE,
    ID_SHUTDOWN,
    ID_EXIT,
    ID_TEAM,
    ID_INSTRUCTION
};

class LogPanel : public wxPanel {
public:
    LogPanel(wxWindow* parent, const wxString &IP_Address);

    void UpdatePanelVisibility(int currentSelectedPanel);
    void AppendLog(const wxString& message);

private:
    wxTextCtrl* logTextCtrl{};

    void OnCancelClick(wxCommandEvent& event);
    void OnSendClick(wxCommandEvent& event);
};

#endif //LOGPANEL_H
