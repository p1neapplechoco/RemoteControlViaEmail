#include "LogPanel.h"

#include <unistd.h>

LogPanel::LogPanel(wxWindow* parent, const wxString &IP_Address, const wxString &IP_Port) : wxPanel(parent, wxID_ANY) {
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

    CreateSCREENSHOT();

    // Set timer
    loadingTimer = new wxTimer(this);
    loadingDots = 0;
    Bind(wxEVT_TIMER, &LogPanel::OnTimer, this);

    // Process
    long port;
    IP_Port.ToLong(&port);
    client.setServerPort(port);
    client.setServerIP(IP_Address.ToStdString());

    if(ConnectToServer(IP_Address, IP_Port)) {
        AppendLog("Connect to server IP " + IP_Address + " is Successfully!! Wait for response!\n");
    } else {
        AppendLog("Connect to server IP " + IP_Address + " is Unsuccessfully!! Try again!\n");
        isConnect = false;
    }
}

void LogPanel::CreateSCREENSHOT() {
    auto SCREENSHOTSizer = new wxBoxSizer(wxVERTICAL);
    auto headerSizer = new wxBoxSizer(wxHORIZONTAL);

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    auto label = new wxStaticText(SCREENSHOTPanel, wxID_ANY, "SCREENSHOT");
    label->SetFont(titleFont);

    auto openButton = new wxButton(SCREENSHOTPanel, wxID_ANY, "Open Image");
    auto closeButton = new wxButton(SCREENSHOTPanel, wxID_ANY, "x");
    closeButton->SetMinSize(wxSize(30, -1));

    headerSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL);
    headerSizer->AddStretchSpacer();
    headerSizer->Add(openButton, 0, wxALIGN_CENTER_VERTICAL);
    headerSizer->Add(closeButton, 0, wxALIGN_CENTER_VERTICAL);

    SCREENSHOTSizer->Add(headerSizer, 0, wxEXPAND | wxBOTTOM, 5);

    openButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent& event) {
        wxImage image ("./assert/capture/screenshot.png");
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
    });

    SCREENSHOTPanel->SetSizer(SCREENSHOTSizer);
    SCREENSHOTPanel->Hide();
}

void LogPanel::UpdatePanelVisibility(int selectedPanel) {
    if (SCREENSHOTPanel) {
        SCREENSHOTPanel->Show(false);
    }

    ID_SelectPanel = selectedPanel;

    switch (selectedPanel) {
        case ID_SCREENSHOT:
            SCREENSHOTPanel->Show(true);
        break;
        case ID_SHUTDOWN:
            // Show capture screen panel
                break;
        case ID_CAPTURE_WEBCAM:
            // Show webcam panel
                break;
        // Add cases for other panels
    }
    Layout();
}

bool LogPanel::ConnectToServer(const wxString &IP_Address, const wxString &IP_Port) {
    if (!client.setupWSA())
    {
        std::cerr << "Failed to setup WSA" << std::endl;
        return false;
    }
    if (!client.setupSocket())
    {
        std::cerr << "Failed to setup socket" << std::endl;
        WSACleanup();
        return false;
    }
    return client.connectToServer();
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
        string response = "";
        switch (ID_SelectPanel) {
            case ID_LIST_PROCESSES: {
                if(!client.handleCommand("!list p", response)) {
                    AppendLog("Failed to send command! Disconnect to server!");
                    isConnect = false;
                    return;
                } else AppendLog("List of processes:\n" + response);
            }   break;
            case ID_LIST_SERVICES: {
                if(!client.handleCommand("!list s", response)) {
                    AppendLog("Failed to send command! Disconnect to server!");
                    isConnect = false;
                    return;
                } else AppendLog("List of services:\n" + response);
            }   break;
            case ID_SCREENSHOT: {
                if(!client.handleCommand("!screenshot", response)) {
                    AppendLog("Failed to send command! Disconnect to server!");
                    isConnect = false;
                    return;
                } else AppendLog(response);
            }   break;
            case ID_TOGGLE_WEBCAM: {
                if(!client.handleCommand("!webcam", response)) {
                    AppendLog("Failed to send command! Disconnect to server!");
                    isConnect = false;
                    return;
                } else AppendLog(response);
            }   break;
            case ID_CAPTURE_WEBCAM: {
                if(!client.handleCommand("!capture", response)) {
                    AppendLog("Failed to send command! Disconnect to server!");
                    isConnect = false;
                    return;
                } else AppendLog(response);
            }   break;
            case ID_SHUTDOWN: {
                if(!client.handleCommand("!shutdown", response)) {
                    AppendLog("Failed to send command! Disconnect to server!");
                    isConnect = false;
                    return;
                } else AppendLog(response);
            }   break;
            default:
                break;
        }
    }
}

