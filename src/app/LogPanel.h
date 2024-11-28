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
    LogPanel(wxWindow* parent, const wxString &IP_Address, const wxString &IP_Port);

    ~LogPanel() {
        if (loadingTimer) {
            delete loadingTimer;
        }
    }

    wxPanel* SCREENSHOTPanel;
    bool isConnect = true;

    void CreateSCREENSHOT();
    void UpdatePanelVisibility(int currentSelectedPanel);
    void AppendLog(const wxString& message);
    void EnableButtons(bool enable);

private:
    int ID_SelectPanel;
    wxTextCtrl* logTextCtrl{};
    Client client;

    wxBitmapButton* sendButton;
    wxBitmapButton* cancelButton;

    wxTimer* loadingTimer;
    int loadingDots;

    void OnTimer(wxTimerEvent& event);
    void StartLoading(const wxString& command);

    bool ConnectToServer(const wxString &IP_Address, const wxString &IP_Port);
    void OnClearClick(wxCommandEvent& event);
    void OnSendClick(wxCommandEvent& event);
};

#endif //LOGPANEL_H
