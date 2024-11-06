//
// Created by phida on 10/11/2024.
//

#include "WebcamController.h"

// Remember to link with strmiids.lib

HRESULT WebcamController::InitializeGraph() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) return hr;

    // Create the Filter Graph Manager
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                          IID_IGraphBuilder, (void **) &pGraph);
    if (FAILED(hr)) return hr;

    // Create the Capture Graph Builder
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
                          IID_ICaptureGraphBuilder2, (void **) &pBuild);
    if (FAILED(hr)) return hr;

    // Initialize the Capture Graph Builder
    hr = pBuild->SetFiltergraph(pGraph);
    if (FAILED(hr)) return hr;

    // Get the IMediaControl interface
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **) &pControl);
    if (FAILED(hr)) return hr;

    return S_OK;
}

HRESULT WebcamController::EnumerateDevices() {
    HRESULT hr = S_OK;
    ICreateDevEnum *pDevEnum = NULL;
    IEnumMoniker *pEnum = NULL;

    // Create the System Device Enumerator
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                          IID_ICreateDevEnum, (void **) &pDevEnum);
    if (FAILED(hr)) return hr;

    // Create an enumerator for the video capture devices
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (FAILED(hr)) {
        pDevEnum->Release();
        return hr;
    }

    // If there are no enumerators for the requested type, then
    // CreateClassEnumerator will succeed, but pEnum will be NULL
    if (pEnum == NULL) {
        pDevEnum->Release();
        return E_FAIL;
    }

    // Use the first video capture device on the device list.
    IMoniker *pMoniker = NULL;
    if (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
        hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void **) &pCap);
        if (SUCCEEDED(hr)) {
            hr = pGraph->AddFilter(pCap, L"Capture Filter");
        }
        pMoniker->Release();
    }

    pDevEnum->Release();
    pEnum->Release();

    return hr;
}

void WebcamController::WebcamThread() {
    // Initialize DirectShow objects
    ICaptureGraphBuilder2 *pBuild = nullptr;
    IGraphBuilder *pGraph = nullptr;
    IBaseFilter *pCap = nullptr;
    IMediaControl *pControl = nullptr;
    IVideoWindow *pWindow = nullptr;

    // IBaseFilter *pMux; // Remove this line if you only want a preview.

    IMoniker *pMoniker;
    IEnumMoniker *pEnum;
    ICreateDevEnum *pDevEnum;


    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr)) {
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
        // cin >> DeviceNumber;
        int DeviceNumber = 1;
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
        if (SUCCEEDED(hr)) {
            pControl->Run();
            pWindow->put_Caption((BSTR) L"Device Capture");
            std::unique_lock<std::mutex> lock(mtx);
            while (!stopRequested) {
                cv.wait_for(lock, std::chrono::milliseconds(100));
                // Process messages if needed
                MSG msg;
                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            pControl->Stop();
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
    }
    CoUninitialize();
    isRunning = false;
}

void WebcamController::StartWebcam() {
    if (!isRunning) {
        isRunning = true;
        stopRequested = false;
        std::thread webcamThread(&WebcamController::WebcamThread, this);
        webcamThread.detach();
    }
}

void WebcamController::StopWebcam() {
    // if (pControl) {
    //     return pControl->Stop();
    // }
    if (isRunning) {
        stopRequested = true;
        cv.notify_one();
    }
}

void WebcamController::CleanUp() {
    if (pControl) pControl->Release();
    if (pCap) pCap->Release();
    if (pBuild) pBuild->Release();
    if (pGraph) pGraph->Release();
    CoUninitialize();
}
