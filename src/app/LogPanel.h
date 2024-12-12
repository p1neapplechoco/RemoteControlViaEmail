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

class ProcessManager;
class ServiceManager;

class LogPanel : public wxPanel {
public:
    LogPanel(wxWindow* parent, const wxString &IP_Address);

    ~LogPanel();

    wxPanel* LIST_PPanel;
    wxPanel* LIST_SPanel;
    wxPanel* SCREENSHOTPanel;
    wxPanel* WEBCAMPanel;
    wxPanel* CAPTUREPanel;
    wxPanel* END_PPanel;
    wxPanel* END_SPanel;
    wxPanel* SHUTDOWNPanel;

    wxTextCtrl* m_processIdText;
    wxTextCtrl* m_serviceIdText;

    bool isConnect = true;

    void CreateSCREENSHOT();

    void CreateWEBCAM();

    void CreateCAPTURE();

    void CreateLIST_P();

    void CreateLIST_S();

    void CreateEND_P();

    void CreateEND_S();

    void CreateSHUTDOWN();

    void UpdatePanelVisibility(int currentSelectedPanel);

    void AppendLog(const wxString& message);

    void EnableButtons(bool enable);

    bool ListProcesses();

    bool ListServices();

    bool EndProcess(const wxString& pidStr);

    bool EndService(const wxString& pidStr);

    bool StartServices(const wxString& pidStr);

private:
    int ID_SelectPanel;
    wxTextCtrl* logTextCtrl{};

    ProcessManager* processManager;
    ServiceManager* serviceManager;

    Client client;

    wxBitmapButton* sendButton;
    wxBitmapButton* cancelButton;

    wxTimer* loadingTimer;
    int loadingDots;

    void OnTimer(wxTimerEvent& event);

    void StartLoading(const wxString& command);

    void OnClearClick(wxCommandEvent& event);

    void OnSendClick(wxCommandEvent& event);
};

#endif //LOGPANEL_H
