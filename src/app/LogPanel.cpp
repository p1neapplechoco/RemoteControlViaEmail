#include "LogPanel.h"

#include <unistd.h>

LogPanel::LogPanel(wxWindow* parent, const wxString &IP_Address) : wxPanel(parent, wxID_ANY) {
    SetBackgroundColour(wxColor(255, 255, 255));

    const auto margin = FromDIP(10);
    auto mainSizer = new wxBoxSizer(wxVERTICAL);

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

    END_PPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(END_PPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    END_SPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(END_SPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

    SHUTDOWNPanel = new wxPanel(bottomPanel, wxID_ANY);
    bottomSizer->Add(SHUTDOWNPanel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, margin);

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
    CreateEND_P();
    CreateEND_S();
    CreateSHUTDOWN();

    // Set timer
    loadingTimer = new wxTimer(this);
    loadingDots = 0;
    Bind(wxEVT_TIMER, &LogPanel::OnTimer, this);

    // Process
    if(client.connectToServer(IP_Address.ToStdString())) {
        AppendLog("Connect to server IP " + IP_Address + " is Successfully!! Wait for response!\n");
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

void LogPanel::CreateEND_P() {
    auto ENP_PSizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(END_PPanel, wxID_ANY, "END PROCESS");
    label->SetFont(titleFont);

    auto idSizer = new wxBoxSizer(wxHORIZONTAL);

    auto idLabel = new wxStaticText(END_PPanel, wxID_ANY, "ID: ");

    m_processIdText = new wxTextCtrl(END_PPanel, wxID_ANY, "");

    idSizer->Add(idLabel, 0, wxALIGN_CENTER_VERTICAL);
    idSizer->Add(m_processIdText, 1, wxEXPAND | wxLEFT, 5);

    ENP_PSizer->Add(label, 0, wxEXPAND | wxBOTTOM, 5);
    ENP_PSizer->Add(idSizer, 0, wxEXPAND | wxTOP, 5);

    END_PPanel->SetSizer(ENP_PSizer);
    END_PPanel->Hide();
}

void LogPanel::CreateEND_S() {
    auto ENP_SSizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(END_SPanel, wxID_ANY, "END SERVICE");
    label->SetFont(titleFont);

    auto idSizer = new wxBoxSizer(wxHORIZONTAL);

    auto idLabel = new wxStaticText(END_SPanel, wxID_ANY, "ID: ");

    m_serviceIdText = new wxTextCtrl(END_SPanel, wxID_ANY, "");

    idSizer->Add(idLabel, 0, wxALIGN_CENTER_VERTICAL);
    idSizer->Add(m_serviceIdText, 1, wxEXPAND | wxLEFT, 5);

    ENP_SSizer->Add(label, 0, wxEXPAND | wxBOTTOM, 5);
    ENP_SSizer->Add(idSizer, 0, wxEXPAND | wxTOP, 5);

    END_SPanel->SetSizer(ENP_SSizer);
    END_SPanel->Hide();
}

void LogPanel::CreateSHUTDOWN() {
    auto CAPTURESizer = new wxBoxSizer(wxVERTICAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(SHUTDOWNPanel, wxID_ANY, "SHUTDOWN");
    label->SetFont(titleFont);

    CAPTURESizer->Add(label, 0, wxEXPAND | wxBOTTOM, 5);

    SHUTDOWNPanel->SetSizer(CAPTURESizer);
    SHUTDOWNPanel->Hide();
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

    if (END_PPanel) {
        END_PPanel->Show(false);
        m_processIdText->Clear();
    }

    if (END_SPanel) {
        END_SPanel->Show(false);
        m_serviceIdText->Clear();
    }

    if (SHUTDOWNPanel) {
        SHUTDOWNPanel->Show(false);
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
        case ID_END_PROCESS:
            END_PPanel->Show(true);
            break;
        case ID_END_SERVICE:
            END_SPanel->Show(true);
            break;
        case ID_SHUTDOWN:
            SHUTDOWNPanel->Show(true);
            break;
    }
    Layout();
}

void LogPanel::OnClearClick(wxCommandEvent& event) {
    logTextCtrl->Clear();
}

void LogPanel::OnSendClick(wxCommandEvent& event) {
    if(!isConnect) {
        AppendLog("Failed to send command! Disconnect to server!\n");
        return;
    }

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
        case ID_END_PROCESS:
            StartLoading("Sending command End Process");
        break;
        case ID_END_SERVICE:
            StartLoading("Sending command End Service");
        break;
        case ID_SHUTDOWN:
            StartLoading("Sending command Shutdown");
        break;
        default:
            break;
    }
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
    auto OpenPicture = [this](const string &filePath){
        wxImage image (filePath);
        if (image.IsOk()) {
            double scaleFactor = 0.5; // Adjust this value to control scaling (0.5 = 50%, 0.75 = 75%, etc.)
            int scaledWidth = image.GetWidth() * scaleFactor;
            int scaledHeight = image.GetHeight() * scaleFactor;
            image.Rescale(scaledWidth, scaledHeight);

            wxWindow* mainFrame = GetParent();
            wxFrame* imageFrame;

            if (auto frame = dynamic_cast<MainFrame*>(mainFrame)) {
                imageFrame = new wxFrame(frame, wxID_ANY, "Screenshot Viewer",
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
            case ID_LIST_PROCESSES: {
                if(!client.handleCommand("!list p", response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                    return;
                } else AppendLog("List of processes:\n" + response);
            }   break;
            case ID_LIST_SERVICES: {
                if(!client.handleCommand("!list s", response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                    return;
                } else AppendLog("List of services:\n" + response);
            }   break;
            case ID_SCREENSHOT: {
                if(!client.handleCommand("!screenshot", response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                    return;
                } else {
                    AppendLog(response);
                    OpenPicture(filepath);
                }
            }   break;
            case ID_TOGGLE_WEBCAM: {
                if(!client.handleCommand("!webcam", response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                    return;
                } else AppendLog(response);
            }   break;
            case ID_CAPTURE_WEBCAM: {
                if(!client.handleCommand("!capture", response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                    return;
                } else {
                    AppendLog(response);
                    OpenPicture(filepath);
                    cout << filepath << endl;
                }
            }   break;
            case ID_END_PROCESS: {
                string ProcessId = m_processIdText->GetValue().ToStdString();
                if(!client.handleCommand("!endp " + ProcessId, response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                    return;
                } else AppendLog(response);
            }   break;
            case ID_END_SERVICE: {
                string ProcessId = m_serviceIdText->GetValue().ToStdString();
                if(!client.handleCommand("!ends " + ProcessId, response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                    return;
                } else AppendLog(response);
            }   break;
            case ID_SHUTDOWN: {
                if(!client.handleCommand("!shutdown 2", response, filepath)) {
                    AppendLog("Failed to send command! Disconnected to server!");
                    isConnect = false;
                    return;
                } else AppendLog(response);
            }   break;
            default:
                break;
        }
    }
}

