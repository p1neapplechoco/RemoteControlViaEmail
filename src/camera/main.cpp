#include <windows.h>
#include <dshow.h>
#include <iostream>
#include <conio.h>

using namespace std;

int main() {
    int DeviceNumber;

    HRESULT hr;

    ICaptureGraphBuilder2 *pBuild;
    IGraphBuilder *pGraph;
    IBaseFilter *pCap;

    // IBaseFilter *pMux; // Remove this line if you only want a preview.

    IMoniker *pMoniker;
    IEnumMoniker *pEnum;
    ICreateDevEnum *pDevEnum;

    IMediaControl *pControl;
    IVideoWindow *pWindow;

    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        /*Handle Error*/
    }

    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2,
                          (void **) &pBuild);
    if (FAILED(hr)) {
        /*Handle Error*/
    }
    hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, IID_IFilterGraph, (void **) &pGraph);
    if (FAILED(hr)) {
        /*Handle Error*/
    }
    pBuild->SetFiltergraph(pGraph);

    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));
    if (FAILED(hr)) {
        /*Handle Error*/
    }
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (FAILED(hr)) {
        /*Handle Error*/
    }

    cout << "Select Device :-" << endl;
    while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
        IPropertyBag *pPropBag;
        hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
        if (FAILED(hr)) {
            pMoniker->Release();
            continue;
        }

        VARIANT var;
        VariantInit(&var);

        hr = pPropBag->Read(L"FriendlyName", &var, 0);
        if (SUCCEEDED(hr)) {
            wcout << (wchar_t *) var.bstrVal << endl;
            VariantClear(&var);
        }

        pPropBag->Release();
        pMoniker->Release();
    }
    pEnum->Reset();
    cin >> DeviceNumber;
    for (int i = 0; i < DeviceNumber; i++) {
        hr = pEnum->Next(1, &pMoniker, NULL);
        if (FAILED(hr)) {
            /*Handle Error*/
        }
    }
    hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **) &pCap);
    if (FAILED(hr)) {
        /*Handle Error*/
    }
    hr = pGraph->AddFilter(pCap, L"Capture Filter");
    if (FAILED(hr)) {
        /*Handle Error*/
    }
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **) &pControl);
    if (FAILED(hr)) {
        /*Handle Error*/
    }
    hr = pGraph->QueryInterface(IID_IVideoWindow, (void **) &pWindow);
    if (FAILED(hr)) {
        /*Handle Error*/
    }

    // hr = pBuild->SetOutputFileName(&MEDIASUBTYPE_Avi, L"D:\\Repository\\RemoteControlViaEmail\\src\\camera\\sigma.avi", &pMux, NULL); // Remove this line if you only want a preview.
    // if (FAILED(hr)) { /*Handle Error*/ } // Remove this line if you only want a preview.
    // hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pCap, NULL, pMux); // Remove this line if you only want a preview.
    // if (FAILED(hr)) { /*Handle Error*/ } // Remove this line if you only want a preview.

    hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pCap, NULL, NULL);
    if (FAILED(hr)) {
        /*Handle Error*/
    }

    pControl->Run();

    pWindow->put_Caption((BSTR) L"Device Capture");

    MSG ms;
    while (true) {
        if (PeekMessage(&ms, NULL, 0, 0, PM_REMOVE) > 0) {
            DefWindowProc(ms.hwnd, ms.message, ms.wParam, ms.lParam);
        } else {
            if (kbhit()) {
                pControl->Stop();
                break;
            }
            Sleep(10);
        }
    }

    pBuild->Release();
    pGraph->Release();
    pCap->Release();

    // pMux->Release(); // Remove this line if you only want a preview.

    pMoniker->Release();
    pEnum->Release();
    pDevEnum->Release();

    pControl->Release();
    pWindow->Release();

    CoUninitialize();
}
