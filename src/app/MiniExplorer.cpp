#include "MiniExplorer.h"

enum {
    ID_TreeCtrl = 1,
    ID_ListCtrl,
    ID_Copy,
    ID_Delete,
    ID_Rename
};

wxBEGIN_EVENT_TABLE(MiniExplorer, wxFrame)
    EVT_MENU(wxID_EXIT, MiniExplorer::OnExit)
    EVT_MENU(wxID_ABOUT, MiniExplorer::OnAbout)
    EVT_MENU(ID_Copy, MiniExplorer::OnCopy)
    EVT_MENU(ID_Delete, MiniExplorer::OnDelete)
    EVT_MENU(ID_Rename, MiniExplorer::OnRename)
wxEND_EVENT_TABLE()

MiniExplorer::MiniExplorer(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size) {
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT);

    wxMenu* menuEdit = new wxMenu;
    menuEdit->Append(ID_Copy, "&Copy");
    menuEdit->Append(ID_Delete, "&Delete");
    menuEdit->Append(ID_Rename, "&Rename");

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuEdit, "&Edit");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to Mini Explorer!");

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

    dirTree = new wxTreeCtrl(this, ID_TreeCtrl, wxDefaultPosition, wxSize(300, -1), wxTR_DEFAULT_STYLE);
    fileList = new wxListCtrl(this, ID_ListCtrl, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);

    sizer->Add(dirTree, 1, wxEXPAND | wxALL, 5);
    sizer->Add(fileList, 2, wxEXPAND | wxALL, 5);

    SetSizer(sizer);

    // Populate the directory tree
    // wxDir dir(wxGetCwd());
    wxDir dir("D:\\DEMO");
    wxString filename;
    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
    while (cont) {
        dirTree->AppendItem(dirTree->GetRootItem(), filename);
        cont = dir.GetNext(&filename);
    }
}

void MiniExplorer::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MiniExplorer::OnAbout(wxCommandEvent& event) {
    wxMessageBox("This is a Mini Explorer application using wxWidgets",
                 "About Mini Explorer", wxOK | wxICON_INFORMATION);
}

void MiniExplorer::OnCopy(wxCommandEvent& event) {
    long itemIndex = fileList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (itemIndex == -1) {
        wxMessageBox("No file selected to copy", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString srcPath = fileList->GetItemText(itemIndex);
    wxFileDialog saveFileDialog(this, _("Save File As"), "", srcPath, "*.*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString destPath = saveFileDialog.GetPath();
    if (!wxCopyFile(srcPath, destPath)) {
        wxMessageBox("Failed to copy file", "Error", wxOK | wxICON_ERROR);
    } else {
        wxMessageBox("File copied successfully", "Success", wxOK | wxICON_INFORMATION);
    }
}

void MiniExplorer::OnDelete(wxCommandEvent& event) {
    long itemIndex = fileList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (itemIndex == -1) {
        wxMessageBox("No file selected to delete", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString filePath = fileList->GetItemText(itemIndex);
    if (wxRemoveFile(filePath)) {
        wxMessageBox("File deleted successfully", "Success", wxOK | wxICON_INFORMATION);
        fileList->DeleteItem(itemIndex);
    } else {
        wxMessageBox("Failed to delete file", "Error", wxOK | wxICON_ERROR);
    }
}

void MiniExplorer::OnRename(wxCommandEvent& event) {
    long itemIndex = fileList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (itemIndex == -1) {
        wxMessageBox("No file selected to rename", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString oldPath = fileList->GetItemText(itemIndex);
    wxTextEntryDialog renameDialog(this, "Enter new name:", "Rename File", oldPath);
    if (renameDialog.ShowModal() == wxID_OK) {
        wxString newPath = renameDialog.GetValue();
        if (wxRenameFile(oldPath, newPath)) {
            wxMessageBox("File renamed successfully", "Success", wxOK | wxICON_INFORMATION);
            fileList->SetItemText(itemIndex, newPath);
        } else {
            wxMessageBox("Failed to rename file", "Error", wxOK | wxICON_ERROR);
        }
    }
}