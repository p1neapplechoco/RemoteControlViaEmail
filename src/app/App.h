#pragma once
#include <vector>
#include <string>
#include <utility>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/progdlg.h>
#include <wx/bmpbuttn.h>

#include "../utils/EmailRetrieval.h"
#include "../utils/IniParser.h"
#include "CustomBitmapButton.h"
//#include "MainFrame.h"
#include "LoginFrame.h"
using namespace std;

#ifndef APP_H
#define APP_H

class App : public wxApp {
public:
    bool OnInit();
};

#endif //APP_H
