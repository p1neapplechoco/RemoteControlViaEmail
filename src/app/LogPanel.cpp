#include "LogPanel.h"
#include "ProcessManager.h"
#include "ServiceManager.h"
#include <unistd.h>

void runClient(Client& client) {
    client.startClient();
}

LogPanel::LogPanel(wxWindow* parent, const wxString &IP_Address, const string &email, const string &password) : wxPanel(parent, wxID_ANY) {
    SetBackgroundColour(wxColor(255, 255, 255));

    const auto margin = FromDIP(10);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    processManager = new ProcessManager(this);
    mainSizer->Add(processManager, 1, wxEXPAND | wxALL, margin);
    processManager->Hide();

    serviceManager = new ServiceManager(this);
    mainSizer->Add(serviceManager, 1, wxEXPAND | wxALL, margin);
    serviceManager->Hide();

    fileExplorer = new FileExplorer(this);
    mainSizer->Add(fileExplorer, 1, wxEXPAND | wxALL, margin);
    fileExplorer->Hide();

    logTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    mainSizer->Add(logTextCtrl, 1, wxEXPAND | wxALL, margin);

    // Bottom panel
    auto bottomPanel = new wxPanel(this);
    auto bottomSizer = new wxBoxSizer(wxHORIZONTAL);

    // Build UI for func
    SCREENSHOTPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(SCREENSHOTPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    WEBCAMPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(WEBCAMPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    CAPTUREPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(CAPTUREPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    LIST_PPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(LIST_PPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    LIST_SPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(LIST_SPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    FILE_Panel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(FILE_Panel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    POWERPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(POWERPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->AddStretchSpacer();

    cancelButton = new CustomBitmapButton(bottomPanel, wxID_ANY, "clear");
    cancelButton->Bind(wxEVT_BUTTON, &LogPanel::OnClearClick, this);
    sendButton = new CustomBitmapButton(bottomPanel, wxID_ANY, "send");
    sendButton->Bind(wxEVT_BUTTON, &LogPanel::OnSendClick, this);

    buttonSizer->Add(cancelButton, 0, wxRIGHT, margin);
    buttonSizer->Add(sendButton, 0);

    bottomSizer->Add(buttonSizer, 1, wxEXPAND);
    bottomPanel->SetSizer(bottomSizer);

    mainSizer->Add(bottomPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, margin);
    SetSizer(mainSizer);

    CreateLIST_P();
    CreateLIST_S();
    CreateSCREENSHOT();
    CreateWEBCAM();
    CreateCAPTURE();
    CreateFILE_EXP();
    CreatePOWER();

    // Set timer
    loadingTimer = new wxTimer(this);
    loadingDots = 0;
    Bind(wxEVT_TIMER, &LogPanel::OnTimer, this);

    // Process
    std::string serverIP = IP_Address.ToStdString();
    if(client.connectToServer(IP_Address.ToStdString())) {
        AppendLog("Connect to server IP " + IP_Address + " is Successfully!! Wait for response!\n");
        client.email = email, client.password = password;
        std::thread clientThread(runClient, std::ref(client));
        clientThread.detach();
    } else {
        AppendLog("Connect to server IP " + IP_Address + " is Unsuccessfully!! Try again!\n");
        isConnect = false;
    }
}

LogPanel::~LogPanel() {
    if (loadingTimer) {
        delete loadingTimer;
    }
}

void LogPanel::CreateSCREENSHOT() {
    auto SCREENSHOTSizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(SCREENSHOTPanel, wxID_ANY, "SCREENSHOT");
    label->SetFont(titleFont);

    SCREENSHOTSizer->Add(label, 0, wxEXPAND | wxBOTTOM, 5);

    SCREENSHOTPanel->SetSizer(SCREENSHOTSizer);
    SCREENSHOTPanel->Hide();
}

void LogPanel::CreateWEBCAM() {
    auto WEBCAMSizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(WEBCAMPanel, wxID_ANY, "TOGGLE WEBCAM");
    label->SetFont(titleFont);

    WEBCAMSizer->Add(label, 0, wxEXPAND | wxBOTTOM, 5);

    WEBCAMPanel->SetSizer(WEBCAMSizer);
    WEBCAMPanel->Hide();
}

void LogPanel::CreateCAPTURE() {
    auto CAPTURESizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(CAPTUREPanel, wxID_ANY, "CAPTURE WEBCAM");
    label->SetFont(titleFont);

    CAPTURESizer->Add(label, 0, wxEXPAND | wxBOTTOM, 5);

    CAPTUREPanel->SetSizer(CAPTURESizer);
    CAPTUREPanel->Hide();
}

void LogPanel::CreateLIST_P() {
    auto CAPTURESizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(LIST_PPanel, wxID_ANY, "LIST PROCESSES");
    label->SetFont(titleFont);

    CAPTURESizer->Add(label, 0, wxEXPAND | wxBOTTOM, 5);

    LIST_PPanel->SetSizer(CAPTURESizer);
    LIST_PPanel->Hide();
}

void LogPanel::CreateLIST_S() {
    auto CAPTURESizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(LIST_SPanel, wxID_ANY, "LIST SERVICES");
    label->SetFont(titleFont);

    CAPTURESizer->Add(label, 0, wxEXPAND | wxBOTTOM, 5);

    LIST_SPanel->SetSizer(CAPTURESizer);
    LIST_SPanel->Hide();
}

void LogPanel::CreateFILE_EXP() {
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(FILE_Panel, wxID_ANY, "FILE EXPLORER");
    label->SetFont(titleFont);

    mainSizer->Add(label, 0, wxEXPAND | wxBOTTOM, 5);

    FILE_Panel->SetSizer(mainSizer);
    FILE_Panel->Hide();
}

void LogPanel::CreatePOWER() {
    auto POWERSizer = new wxBoxSizer(wxHORIZONTAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(POWERPanel, wxID_ANY, "POWER");
    label->SetFont(titleFont);

    POWERSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    wxArrayString choices;
    choices.Add("Shutdown");
    choices.Add("Sleep");
    choices.Add("Restart");

    powerChoice = new wxChoice(POWERPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
    POWERSizer->Add(powerChoice, 0, wxALIGN_CENTER_VERTICAL, 5);

    POWERPanel->SetSizer(POWERSizer);
    POWERPanel->Hide();
}

void LogPanel::UpdatePanelVisibility(int selectedPanel) {
    if (SCREENSHOTPanel) {
        SCREENSHOTPanel->Show(false);
    }

    if (WEBCAMPanel) {
        WEBCAMPanel->Show(false);
    }

    if (CAPTUREPanel) {
        CAPTUREPanel->Show(false);
    }

    if (LIST_PPanel) {
        LIST_PPanel->Show(false);
    }

    if (LIST_SPanel) {
        LIST_SPanel->Show(false);
    }

    if (FILE_Panel) {
        FILE_Panel->Show(false);
    }

    if (POWERPanel) {
        POWERPanel->Show(false);
    }

    ID_SelectPanel = selectedPanel;
    switch (selectedPanel) {
        case ID_LIST_PROCESSES:
            LIST_PPanel->Show(true);
            break;
        case ID_LIST_SERVICES:
            LIST_SPanel->Show(true);
            break;
        case ID_SCREENSHOT:
            SCREENSHOTPanel->Show(true);
            break;
        case ID_TOGGLE_WEBCAM:
            WEBCAMPanel->Show(true);
            break;
        case ID_CAPTURE_WEBCAM:
            CAPTUREPanel->Show(true);
            break;
        case ID_FILE_EXPLORER:
            FILE_Panel->Show(true);
            break;
        case ID_POWER:
            POWERPanel->Show(true);
            break;
    }
    Layout();
}

void LogPanel::OnClearClick(wxCommandEvent& event) {
    processManager->Hide();
    serviceManager->Hide();
    fileExplorer->Hide();

    if (logTextCtrl->IsShown()) {
        logTextCtrl->Clear();
    } else logTextCtrl->Show();
    Layout();
}

void LogPanel::OnSendClick(wxCommandEvent& event) {
    if(!isConnect) {
        AppendLog("Failed to send command! Disconnect to server!\n");
        return;
    }

    processManager->Hide();
    serviceManager->Hide();
    fileExplorer->Hide();
    logTextCtrl->Show();

    switch (ID_SelectPanel) {
        case ID_LIST_PROCESSES:
            StartLoading("Sending command List Processes");
        break;
        case ID_LIST_SERVICES:
            StartLoading("Sending command List Services");
        break;
        case ID_SCREENSHOT:
            StartLoading("Sending command Screenshot");
        break;
        case ID_TOGGLE_WEBCAM:
            StartLoading("Sending command Toggle Webcam");
        break;
        case ID_CAPTURE_WEBCAM:
            StartLoading("Sending command Capture Webcam");
        break;
        case ID_FILE_EXPLORER:
            StartLoading("Sending command File Explorer");
        break;
        case ID_POWER:
            StartLoading("Sending command Power");
        break;
        default:
            break;
    }
    Layout();
}

void LogPanel::AppendLog(const wxString& message) {
    logTextCtrl->AppendText(wxDateTime::Now().FormatTime() + ": " + message + '\n');
    if (message.Contains("Sending command")) {
        logTextCtrl->AppendText("Loading");

        for (int i = 0; i < 3; i++) {
            sleep(1);
            logTextCtrl->AppendText(".");
        }
        logTextCtrl->AppendText("\n");
    }
}

void LogPanel::EnableButtons(bool enable) {
    sendButton->Enable(enable);
    cancelButton->Enable(enable);
}

void LogPanel::StartLoading(const wxString& command) {
    loadingDots = 0;
    logTextCtrl->AppendText(wxDateTime::Now().FormatTime() + ": " + command);
    logTextCtrl->AppendText("\nLoading");
    loadingTimer->Start(1000); // Timer fires every 1 second

    EnableButtons(false);
    wxWindow* mainFrame = GetParent();
    if (auto frame = dynamic_cast<MainFrame*>(mainFrame)) {
        frame->EnableSideButtons(false);
    }
}

void LogPanel::OnTimer(wxTimerEvent& event) {
    auto OpenPicture = [this](const string &filePath, double scaleFactor){
        wxImage image (filePath);
        if (image.IsOk()) {
            int scaledWidth = image.GetWidth() * scaleFactor;
            int scaledHeight = image.GetHeight() * scaleFactor;
            image.Rescale(scaledWidth, scaledHeight);

            wxWindow* mainFrame = GetParent();
            wxFrame* imageFrame;

            if (auto frame = dynamic_cast<MainFrame*>(mainFrame)) {
                imageFrame = new wxFrame(frame, wxID_ANY, "Picture Viewer",
                                       wxDefaultPosition, wxSize(scaledWidth + 10, scaledHeight + 10),
                                       wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT);
            }
            // Create a panel inside the frame
            wxPanel* imagePanel = new wxPanel(imageFrame, wxID_ANY);
            imagePanel->SetBackgroundColour(*wxWHITE);

            // Create sizer for the panel
            wxBoxSizer* imageSizer = new wxBoxSizer(wxVERTICAL);

            // Create and add the static bitmap
            wxStaticBitmap* staticBitmap = new wxStaticBitmap(imagePanel, wxID_ANY, wxBitmap(image));
            imageSizer->Add(staticBitmap, 1, wxEXPAND | wxALL, 5);

            imagePanel->SetSizer(imageSizer);

            // Fit the frame to the image size
            imageFrame->SetClientSize(image.GetWidth() + 10, image.GetHeight() + 10);
            imageFrame->Center();
            imageFrame->Show();
        } else {
            wxMessageBox("Image file not found!", "Error", wxICON_ERROR);
        }
    };

    loadingDots++;
    logTextCtrl->AppendText(".");

    if (loadingDots >= 3) {
        loadingTimer->Stop();
        logTextCtrl->AppendText("\n\n");

        EnableButtons(true);
        wxWindow* mainFrame = GetParent();
        if (auto frame = dynamic_cast<MainFrame*>(mainFrame)) {
            frame->EnableSideButtons(true);
        }

        // Thực hiện command tương ứng
        string response = "", filepath = "";
        switch (ID_SelectPanel) {
            case ID_LIST_PROCESSES:
                ListProcesses();
                break;
            case ID_LIST_SERVICES:
                ListServices();
                break;
            case ID_SCREENSHOT: {
                if(!client.handleCommand("!screenshot", response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                } else {
                    AppendLog(response);
                    OpenPicture(filepath, 0.5);
                }
            }   break;
            case ID_TOGGLE_WEBCAM: {
                if(!client.handleCommand("!webcam", response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                } else AppendLog(response);
            }   break;
            case ID_CAPTURE_WEBCAM: {
                if(!client.handleCommand("!capture", response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                } else {
                    AppendLog(response);
                    OpenPicture(filepath, 1.5);
                }
            }   break;
            case ID_FILE_EXPLORER:
                openFileExplorer();
                break;
            case ID_POWER: {
                string opt;
                if (powerChoice->GetStringSelection() == "Shutdown") {
                    opt = "0";
                } else if (powerChoice->GetStringSelection() == "Sleep") {
                    opt = "1";
                } else if (powerChoice->GetStringSelection() == "Restart") {
                    opt = "2";
                }

                if(!client.handleCommand("!shutdown " + opt, response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                } else AppendLog(response);
            }   break;
            default:
                break;
        }
    }
}

string LogPanel::scanDrive(const string& path) {
    string response = "", filepath = "";
    if(!client.handleCommand("!index " + path, response, filepath)) {
        AppendLog("Failed to index " + path + "! Try again!");
        return "";
    }

    AppendLog(response);
    return filepath;
}

bool LogPanel::GetAndSendFile(const string& path) {
    string response = "", filepath = "";
    if(!client.handleCommand("!get file " + path, response, filepath)) {
        AppendLog("Failed to get file " + path + "! Try again!");
        return false;
    }

    AppendLog(response);
    return true;
}

bool LogPanel::openFileExplorer() {
    string response = "", filepath = "";
    if(!client.handleCommand("!list disks", response, filepath)) {
        AppendLog("Failed to list disks! Try again!");
        return false;
    }

    // Save response into disks.txt
    wxFile file("disks.txt", wxFile::write);
    if (file.IsOpened()) {
        file.Write(response);
        file.Close();
    }

    AppendLog("Server response: File Explorer opened");

    fileExplorer->LoadDisksFromFile("disks.txt");
    logTextCtrl->Hide();
    fileExplorer->Show();

    Layout();
    return true;
}

bool LogPanel::ListProcesses() {
    string response = "", filepath = "";
    if(!client.handleCommand("!list p", response, filepath)) {
        AppendLog("Failed to list processes! Try again!");
        return false;
    }

    // Save response into processes.txt
    wxFile file("processes.txt", wxFile::write);
    if (file.IsOpened()) {
        file.Write(response);
        file.Close();
    }

    AppendLog("Server response: List of processes saved as processes.txt");

    processManager->LoadProcessesFromFile("processes.txt");
    logTextCtrl->Hide();
    processManager->Show();

    Layout();
    return true;
}

bool LogPanel::ListServices() {
    string response = "", filepath = "";
    if(!client.handleCommand("!list s", response, filepath)) {
        AppendLog("Failed to send command! Disconnected to server!");
        isConnect = false;
        return false;
    }

    // Save response into services.txt
    wxFile file("services.txt", wxFile::write);
    if (file.IsOpened()) {
        file.Write(response);
        file.Close();
    }

    AppendLog("Server response: List of services saved as services.txt\n");

    serviceManager->LoadServiceFromFile("services.txt");
    logTextCtrl->Hide();
    serviceManager->Show();

    Layout();
    return true;
}

bool LogPanel::EndProcess(const wxString& pidStr) {
    string response = "", filepath = "";
    if(!client.handleCommand("!endp " + pidStr.ToStdString(), response, filepath)) {
        AppendLog("Failed to end process ID: " + pidStr.ToStdString() + "! Try again!");
        return false;
    }

    AppendLog(response);
    return true;
}

bool LogPanel::EndService(const wxString& pidStr) {
    string response = "", filepath = "";
    if(!client.handleCommand("!ends " + pidStr.ToStdString(), response, filepath)) {
        AppendLog("Failed to end service name: " + pidStr.ToStdString() + "! Try again!");
        return false;
    }

    AppendLog(response);
    return true;
}

bool LogPanel::StartServices(const wxString& pidStr) {
    string response = "", filepath = "";
    if(!client.handleCommand("!starts " + pidStr.ToStdString(), response, filepath)) {
        AppendLog("Failed to start service name: " + pidStr.ToStdString() + "! Try again!");
        return false;
    }

    AppendLog(response);
    return true;
}

bool LogPanel::Remove(const string& path) {
    string response = "", filepath = "";
    if(!client.handleCommand("!delete file " + path, response, filepath) || strstr(response.c_str(), "Error") != nullptr) {
        AppendLog("Failed to delete file " + path + "! Try again!");
        return false;
    } else {
        AppendLog(response);
        return true;
    }

}

