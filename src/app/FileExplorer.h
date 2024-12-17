#pragma once
#include "App.h"
#include "../utils/GetWinDirectory.h"
#define fi first
#define se second
using namespace std;

class FileExplorer : public wxPanel {
public:
    FileExplorer(wxWindow* parent);
    ~FileExplorer();

    void Reset();

private:
    wxMenu menu;
    wxListCtrl* m_listCtrl;
    wxImageList* m_imageList;
    string m_currentPath;

    // Image indexes trong ImageList
    enum {
        ICON_FILE = 0,
        ICON_FOLDER,
        ICON_DRIVE,
        ICON_UP
    };

    void InitializeIcons();
    void PopulateList();
    void EnableMenuFunctions(bool enable);
    void OnItemActivated(wxListEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnMove(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnDragDrop(wxDropFilesEvent& event);
};