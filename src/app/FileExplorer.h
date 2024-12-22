#pragma once
#include "App.h"
#include "../utils/GetWinDirectory.h"
#define fi first
#define se second
using namespace std;

class FileExplorer : public wxPanel
{
public:
    FileExplorer(wxWindow* parent);
    ~FileExplorer();

    void LoadDisksFromFile(const string& filename);

private:
    wxMenu* contextMenu;
    wxListCtrl* m_listCtrl;
    wxImageList* m_imageList{};
    vector<FOLDER> m_folder;

    // Image indexes trong ImageList
    enum
    {
        ICON_FILE = 0,
        ICON_FOLDER,
        ICON_DRIVE,
        ICON_UP
    };

    void InitializeIcons();
    void PopulateList() const;
    void OnItemActivated(wxListEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnRightClick(wxListEvent& event);

    wxDECLARE_EVENT_TABLE();
};
