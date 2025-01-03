#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>

// LIBRARIES USE WXWIDGETS
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/progdlg.h>
#include <wx/bmpbuttn.h>
#include <wx/image.h>
#include <wx/statbmp.h>

// LIBRARIES FOR MINI EXPLORER
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/filename.h>
#include <wx/imaglist.h>
#include <wx/dir.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/filefn.h>
#include <wx/artprov.h>
#include <wx/menu.h>
#include <wx/listbook.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#include "../utils/Email.h"
#include "../utils/IniParser.h"
#include "../utils/IpDiscovery.h"
#include "../utils/NetworkDiscovery.h"
#include "../utils/Client.h"
#include "CustomBitmapButton.h"
#include "FileExplorer.h"
#include "MainFrame.h"
#include "LoginFrame.h"

using namespace std;

#ifndef APP_H
#define APP_H

class App final : public wxApp {
public:
    bool OnInit() override;
};

#endif //APP_H