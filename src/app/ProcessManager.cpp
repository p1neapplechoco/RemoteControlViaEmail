#include "ProcessManager.h"
#include <algorithm>
#include <cctype>

enum {
    ID_EndTask = 1,
};

wxBEGIN_EVENT_TABLE(ProcessManager, wxPanel)
    EVT_MENU(ID_EndTask, ProcessManager::OnEndTask)
wxEND_EVENT_TABLE()

ProcessManager::ProcessManager(wxWindow* parent)
     : wxPanel(parent, wxID_ANY) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    processBook = new wxListbook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    CreateProcessPages();

    mainSizer->Add(processBook, 1, wxEXPAND | wxALL, 5);

    totalProcessesText = new wxStaticText(this, wxID_ANY, "Total processes: 0");
    mainSizer->Add(totalProcessesText, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizerAndFit(mainSizer);

    // Create context menu
    contextMenu = new wxMenu;
    contextMenu->Append(ID_EndTask, "End Process");

    // Bind right-click event
    appsPage->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &ProcessManager::OnRightClick, this);
    backgroundPage->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &ProcessManager::OnRightClick, this);
    windowsPage->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, &ProcessManager::OnRightClick, this);
}

void ProcessManager::CreateProcessPages() {
    // Tạo trang cho Apps
    appsPage = new wxListCtrl(processBook, wxID_ANY, wxDefaultPosition,
                             wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    appsPage->InsertColumn(0, "PID", wxLIST_FORMAT_LEFT, 80);
    appsPage->InsertColumn(1, "Process Name", wxLIST_FORMAT_LEFT, 400);
    processBook->AddPage(appsPage, "Apps");

    // Tạo trang cho Background Processes
    backgroundPage = new wxListCtrl(processBook, wxID_ANY, wxDefaultPosition,
                                  wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    backgroundPage->InsertColumn(0, "PID", wxLIST_FORMAT_LEFT, 80);
    backgroundPage->InsertColumn(1, "Process Name", wxLIST_FORMAT_LEFT, 400);
    processBook->AddPage(backgroundPage, "Background");

    // Tạo trang cho Windows Processes
    windowsPage = new wxListCtrl(processBook, wxID_ANY, wxDefaultPosition,
                                wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES);
    windowsPage->InsertColumn(0, "PID", wxLIST_FORMAT_LEFT, 80);
    windowsPage->InsertColumn(1, "Process Name", wxLIST_FORMAT_LEFT, 400);
    processBook->AddPage(windowsPage, "Windows");
}

void ProcessManager::AddProcessToPage(wxListCtrl* page, const wxString& pid, const wxString& name) {
    long index = page->GetItemCount();
    long item = page->InsertItem(index, pid);
    page->SetItem(item, 1, name);
}

void ProcessManager::LoadProcessesFromFile(const wxString& filename) {
    // Xóa tất cả dữ liệu cũ
    appsPage->DeleteAllItems();
    backgroundPage->DeleteAllItems();
    windowsPage->DeleteAllItems();

    wxFileInputStream input(filename);
    if (!input.IsOk()) {
        wxMessageBox("Cannot open log file!", "Error", wxICON_ERROR);
        return;
    }

    wxTextInputStream text(input);
    wxString line, totalProcesses;
    wxListCtrl* currentPage = nullptr;

    while (!input.Eof()) {
        line = text.ReadLine();

        if (line.StartsWith("Apps")) {
            currentPage = appsPage;
            continue;
        }
        else if (line.StartsWith("Background processes")) {
            currentPage = backgroundPage;
            continue;
        }
        else if (line.StartsWith("Windows processes")) {
            currentPage = windowsPage;
            continue;
        }
        else if (line.StartsWith("---")) {
            continue;
        } else if (line.StartsWith("Total processes")) {
            totalProcesses = line.AfterLast(' ');
            continue;
        }

        // Xử lý dòng chứa thông tin process
        if (!line.IsEmpty() && currentPage != nullptr) {
            // Tách PID và Process Name
            wxString pid = processLine(line.BeforeFirst(' ').Trim());
            wxString name = processLine(line.AfterFirst(' ').Trim());

            if (!pid.IsEmpty() && !name.IsEmpty()) {
                AddProcessToPage(currentPage, pid, name);
            }
        }
    }

    wxString totalLine = wxString::Format("Total processes: %s", totalProcesses);
    totalProcessesText->SetLabel(totalLine);
}

void ProcessManager::OnRightClick(wxListEvent& event) {
    PopupMenu(contextMenu, event.GetPoint());
}

void ProcessManager::OnEndTask(wxCommandEvent& event) {
    long itemIndex = -1;
    wxListCtrl* currentPage = dynamic_cast<wxListCtrl*>(processBook->GetCurrentPage());
    if (currentPage) {
        itemIndex = currentPage->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }

    if (itemIndex == -1) {
        wxMessageBox("No process selected", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString pidStr = currentPage->GetItemText(itemIndex);

    long pid;
    if (!pidStr.ToLong(&pid)) {
        wxMessageBox("Invalid process ID", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int checkbox = wxMessageBox("Are you sure you want to end this process?", "Confirm", wxYES_NO | wxICON_QUESTION);
    if (checkbox != wxYES) {
        return;
    }

    if(((LogPanel*)GetParent())-> EndProcess(pidStr)) {
        if(!((LogPanel*)GetParent())-> ListProcesses()) {
            wxMessageBox("Failed to list processes", "Error", wxOK | wxICON_ERROR);
        }
    } else {
        wxMessageBox("Failed to end process", "Error", wxOK | wxICON_ERROR);
    }
}

// Function to trim leading and trailing spaces
wxString ProcessManager::trim(const wxString& str) {
    size_t first = str.find_first_not_of(' ');
    if (wxString::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Function to remove extra spaces between words
wxString ProcessManager::removeExtraSpaces(const wxString& str) {
    wxString result;
    bool inSpaces = false;
    for (char ch : str) {
        if (isspace(ch)) {
            if (!inSpaces) {
                result += ' ';
                inSpaces = true;
            }
        } else {
            result += ch;
            inSpaces = false;
        }
    }
    return result;
}

// Function to process the line by trimming and removing extra spaces
wxString ProcessManager::processLine(const wxString& line) {
    wxString trimmedLine = trim(line);
    return removeExtraSpaces(trimmedLine);
}