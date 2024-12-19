#include "WindowsCommands.h"

int GetEncoderClsid(const WCHAR *, CLSID *);
std::pair<int, int> GetPhysicalDesktopDimensions();

std::vector<char> WindowsCommands::screenShot()
{
    const Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    const std::pair<int, int> resolution = GetPhysicalDesktopDimensions();

    int w = resolution.first;
    int h = resolution.second;

    constexpr float ratio = 1.0f;
    w = static_cast<int>(static_cast<float>(w) * ratio);
    h = static_cast<int>(static_cast<float>(h) * ratio);

    const HDC hScreen = GetDC(nullptr);
    const HDC hDC = CreateCompatibleDC(hScreen);
    const HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    const HGDIOBJ old_obj = SelectObject(hDC, hBitmap);

    SetStretchBltMode(hDC, HALFTONE);
    StretchBlt(hDC, 0, 0, w, h, hScreen, x, y, w, h, SRCCOPY);

    Gdiplus::Bitmap *bitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, nullptr);

    CLSID jpegClsid;
    GetEncoderClsid(L"image/jpeg", &jpegClsid);

    IStream *istream = nullptr;
    CreateStreamOnHGlobal(nullptr, TRUE, &istream);

    Gdiplus::EncoderParameters encoderParameters;
    ULONG quality = 150;

    encoderParameters.Count = 1;
    encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
    encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues = 1;
    encoderParameters.Parameter[0].Value = &quality;

    bitmap->Save(istream, &jpegClsid, &encoderParameters);

    HGLOBAL hg = nullptr;
    GetHGlobalFromStream(istream, &hg);
    const int buffer_size = GlobalSize(hg);
    std::vector<char> buffer(buffer_size);

    LPVOID ptr = GlobalLock(hg);
    memcpy(&buffer[0], ptr, buffer_size);
    GlobalUnlock(hg);

    istream->Release();
    delete bitmap;

    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(nullptr, hScreen);
    DeleteObject(hBitmap);

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return buffer;
}

void WindowsCommands::shutdown(const UINT nSDType)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    ::LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1; // set 1 privilege
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // get the shutdown privilege for this process
    ::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)nullptr, 0);

    switch (nSDType)
    {
        case 0:
            ::ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
        break;

        case 1:
            ::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
            break;

        case 2:
            ::ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
            break;

        default:
            break;;
    }
}

int GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
    UINT num = 0;
    UINT size = 0;

    Gdiplus::ImageCodecInfo *pImageCodecInfo = nullptr;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    pImageCodecInfo = static_cast<Gdiplus::ImageCodecInfo *>(malloc(size));
    if (pImageCodecInfo == nullptr)
        return -1;

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

std::pair<int, int> GetPhysicalDesktopDimensions()
{
    DEVMODE dev_mode;
    dev_mode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dev_mode);
    int screenWidth = dev_mode.dmPelsWidth;
    int screenHeight = dev_mode.dmPelsHeight;
    return {screenWidth, screenHeight};
}
