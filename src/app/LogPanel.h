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
    ID_FILE_EXPLORER,
    ID_POWER,
    ID_EXIT,
    ID_TEAM,
    ID_INSTRUCTION
};

class ProcessManager;
class ServiceManager;
class FileExplorer;

class LogPanel : public wxPanel {
public:
    LogPanel(wxWindow* parent, const wxString &IP_Address);

    ~LogPanel();

    wxPanel* LIST_PPanel;
    wxPanel* LIST_SPanel;
    wxPanel* SCREENSHOTPanel;
    wxPanel* WEBCAMPanel;
    wxPanel* CAPTUREPanel;
    wxPanel* FILE_Panel;
    wxPanel* POWERPanel;

    bool isConnect = true;

    void CreateSCREENSHOT();

    void CreateWEBCAM();

    void CreateCAPTURE();

    void CreateLIST_P();

    void CreateLIST_S();

    void CreateFILE_EXP();

    void CreatePOWER();

    void UpdatePanelVisibility(int currentSelectedPanel);

    void AppendLog(const wxString& message);

    void EnableButtons(bool enable);

    bool ListProcesses();

    bool ListServices();

    bool EndProcess(const wxString& pidStr);

    bool EndService(const wxString& pidStr);

    bool StartServices(const wxString& pidStr);

    string scanDrive(const string& path);

    bool GetAndSendFile(const string& path);

    bool openFileExplorer();

private:
    int ID_SelectPanel;
    wxTextCtrl* logTextCtrl{};
    wxChoice* powerChoice;

    ProcessManager* processManager;
    ServiceManager* serviceManager;
    FileExplorer* fileExplorer;

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
