//
// Created by phida on 10/11/2024.
//

#include "WebcamController.h"


// Remember to link with strmiids.lib
int WebcamController::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;

    Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;
    Gdiplus::GetImageEncodersSize(&num, &size);

    if (size == 0) return -1;

    pImageCodecInfo = static_cast<Gdiplus::ImageCodecInfo *>(malloc(size));
    if (pImageCodecInfo == nullptr) return -1;

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (int j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

HRESULT WebcamController::GrabFrame(IMediaControl* pControl, IBaseFilter* pCap) {
    // Get the capture output pin
    IPin* pPin = nullptr;
    HRESULT hr = pBuild->FindPin(pCap, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, FALSE, 0, &pPin);
    if (FAILED(hr)) return hr;

    // Create sample grabber filter
    IBaseFilter* pGrabberF = nullptr;
    hr = CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER,
                         IID_IBaseFilter, (void**)&pGrabberF);
    if (FAILED(hr)) {
        pPin->Release();
        return hr;
    }

    // Add sample grabber to graph
    hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
    if (FAILED(hr)) {
        pPin->Release();
        pGrabberF->Release();
        return hr;
    }

    // Get sample grabber interface
    ISampleGrabber* pGrabber = nullptr;
    hr = pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
    if (FAILED(hr)) {
        pPin->Release();
        pGrabberF->Release();
        return hr;
    }

    // Set media type
    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;
    hr = pGrabber->SetMediaType(&mt);
    if (FAILED(hr)) {
        pPin->Release();
        pGrabber->Release();
        pGrabberF->Release();
        return hr;
    }

    // Connect sample grabber to capture filter
    hr = pGraph->Connect(pPin, nullptr);
    if (FAILED(hr)) {
        pPin->Release();
        pGrabber->Release();
        pGrabberF->Release();
        return hr;
    }

    // Get the frame buffer and size
    AM_MEDIA_TYPE actualMT;
    hr = pGrabber->GetConnectedMediaType(&actualMT);
    if (FAILED(hr)) {
        pPin->Release();
        pGrabber->Release();
        pGrabberF->Release();
        return hr;
    }

    VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)actualMT.pbFormat;
    int width = vih->bmiHeader.biWidth;
    int height = vih->bmiHeader.biHeight;

    // Allocate buffer for the frame
    long bufferSize = width * height * 3; // RGB24 format
    unsigned char* buffer = new unsigned char[bufferSize];

    // Set callback buffer
    hr = pGrabber->SetBufferSamples(TRUE);
    if (FAILED(hr)) {
        delete[] buffer;
        pPin->Release();
        pGrabber->Release();
        pGrabberF->Release();
        return hr;
    }

    // Run the graph for a short time to get a frame
    hr = pControl->Run();
    Sleep(100); // Wait for a frame

    long cbBuffer = bufferSize;
    hr = pGrabber->GetCurrentBuffer(&cbBuffer, (long*)buffer);
    if (SUCCEEDED(hr)) {
        // Convert to JPEG using GDI+
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

        // Create bitmap from buffer
        Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(width, height, width * 3,
            PixelFormat24bppRGB, buffer);

        // Save to IStream as JPEG
        IStream* istream = nullptr;
        CreateStreamOnHGlobal(nullptr, TRUE, &istream);

        // Get JPEG encoder CLSID
        CLSID jpegClsid;
        GetEncoderClsid(L"image/jpeg", &jpegClsid);

        // Set JPEG quality
        Gdiplus::EncoderParameters encoderParameters;
        ULONG quality = 75;
        encoderParameters.Count = 1;
        encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
        encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
        encoderParameters.Parameter[0].NumberOfValues = 1;
        encoderParameters.Parameter[0].Value = &quality;

        // Save to stream
        bitmap->Save(istream, &jpegClsid, &encoderParameters);

        // Get data from stream
        HGLOBAL hg = nullptr;
        GetHGlobalFromStream(istream, &hg);
        int jpegSize = GlobalSize(hg);

        // Lock mutex before updating currentFrame
        std::lock_guard<std::mutex> lock(frameMutex);
        currentFrame.resize(jpegSize);
        void* ptr = GlobalLock(hg);
        memcpy(currentFrame.data(), ptr, jpegSize);
        GlobalUnlock(hg);

        // Cleanup
        istream->Release();
        delete bitmap;
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }

    // Cleanup
    delete[] buffer;
    pPin->Release();
    pGrabber->Release();
    pGrabberF->Release();

    return hr;
}

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
    ICaptureGraphBuilder2* pBuild = nullptr;
    IGraphBuilder* pGraph = nullptr;
    IBaseFilter* pCap = nullptr;
    IMediaControl* pControl = nullptr;
    IBaseFilter* pGrabberF = nullptr;
    ISampleGrabber* pGrabber = nullptr;
    IMoniker* pMoniker = nullptr;
    IEnumMoniker* pEnum = nullptr;
    ICreateDevEnum* pDevEnum = nullptr;
    IVideoWindow* pWindow = nullptr;  // Add this for controlling window visibility

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr)) {
        // Create the Filter Graph Manager
        hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER,
                            IID_IGraphBuilder, (void**)&pGraph);
        if (FAILED(hr)) return;

        // Create the Capture Graph Builder
        hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER,
                            IID_ICaptureGraphBuilder2, (void**)&pBuild);
        if (FAILED(hr)) return;

        hr = pBuild->SetFiltergraph(pGraph);
        if (FAILED(hr)) return;

        // Create System Device Enumerator
        hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER,
                            IID_PPV_ARGS(&pDevEnum));
        if (FAILED(hr)) return;

        // Create enumerator for video capture devices
        hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
        if (FAILED(hr)) return;

        // Get first video capture device
        if (pEnum->Next(1, &pMoniker, nullptr) == S_OK) {
            hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void**)&pCap);
            if (FAILED(hr)) return;

            hr = pGraph->AddFilter(pCap, L"Capture Filter");
            if (FAILED(hr)) return;

            // Create and add Sample Grabber filter
            hr = CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IBaseFilter, (void**)&pGrabberF);
            if (FAILED(hr)) return;

            hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
            if (FAILED(hr)) return;

            // Get Sample Grabber interface
            hr = pGrabberF->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);
            if (FAILED(hr)) return;

            // Set media type to RGB24
            AM_MEDIA_TYPE mt;
            ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
            mt.majortype = MEDIATYPE_Video;
            mt.subtype = MEDIASUBTYPE_RGB24;
            hr = pGrabber->SetMediaType(&mt);
            if (FAILED(hr)) return;

            // Configure Sample Grabber
            hr = pGrabber->SetBufferSamples(TRUE);
            if (FAILED(hr)) return;
            hr = pGrabber->SetOneShot(FALSE);
            if (FAILED(hr)) return;

            // Build the capture graph
            hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                    pCap, pGrabberF, nullptr);
            if (FAILED(hr)) return;

            // Get video window interface and hide it
            hr = pGraph->QueryInterface(IID_IVideoWindow, (void**)&pWindow);
            if (SUCCEEDED(hr)) {
                pWindow->put_AutoShow(OAFALSE);  // Disable auto-show
                pWindow->put_Visible(OAFALSE);   // Hide the window
            }

            // Get media control interface
            hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
            if (FAILED(hr)) return;

            // Get the media type being used
            AM_MEDIA_TYPE actualMT;
            hr = pGrabber->GetConnectedMediaType(&actualMT);
            if (SUCCEEDED(hr)) {
                VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)actualMT.pbFormat;
                int width = vih->bmiHeader.biWidth;
                int height = vih->bmiHeader.biHeight;
                long bufferSize = width * height * 3; // RGB24 format
                unsigned char* buffer = new unsigned char[bufferSize];

                // Start capturing
                hr = pControl->Run();
                if (SUCCEEDED(hr)) {
                    std::unique_lock<std::mutex> lock(mtx);
                    while (!stopRequested) {
                        // Grab frame
                        long cbBuffer = bufferSize;
                        if (SUCCEEDED(pGrabber->GetCurrentBuffer(&cbBuffer, (long*)buffer))) {
                            // Initialize GDI+
                            Gdiplus::GdiplusStartupInput gdiplusStartupInput;
                            ULONG_PTR gdiplusToken;
                            Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

                            // Create bitmap from buffer
                            Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(width, height, width * 3,
                                PixelFormat24bppRGB, buffer);

                            // Save to IStream as JPEG
                            IStream* istream = nullptr;
                            CreateStreamOnHGlobal(nullptr, TRUE, &istream);

                            // Get JPEG encoder CLSID
                            CLSID jpegClsid;
                            GetEncoderClsid(L"image/jpeg", &jpegClsid);

                            // Set JPEG quality
                            Gdiplus::EncoderParameters encoderParameters;
                            ULONG quality = 75;
                            encoderParameters.Count = 1;
                            encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
                            encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
                            encoderParameters.Parameter[0].NumberOfValues = 1;
                            encoderParameters.Parameter[0].Value = &quality;

                            // Save to stream
                            bitmap->Save(istream, &jpegClsid, &encoderParameters);

                            // Get data from stream
                            HGLOBAL hg = nullptr;
                            GetHGlobalFromStream(istream, &hg);
                            int jpegSize = GlobalSize(hg);

                            // Update currentFrame
                            {
                                std::lock_guard<std::mutex> frameLock(frameMutex);
                                currentFrame.resize(jpegSize);
                                void* ptr = GlobalLock(hg);
                                memcpy(currentFrame.data(), ptr, jpegSize);
                                GlobalUnlock(hg);
                            }

                            // Cleanup
                            istream->Release();
                            delete bitmap;
                            Gdiplus::GdiplusShutdown(gdiplusToken);
                        }

                        // Wait for next frame (approximately 30fps)
                        cv.wait_for(lock, std::chrono::milliseconds(33));
                    }

                    delete[] buffer;
                }

                if (actualMT.pbFormat) CoTaskMemFree(actualMT.pbFormat);
                if (actualMT.pUnk) actualMT.pUnk->Release();
            }

            pControl->Stop();
        }

        // Cleanup
        if (pWindow) pWindow->Release();  // Release video window
        if (pControl) pControl->Release();
        if (pGrabber) pGrabber->Release();
        if (pGrabberF) pGrabberF->Release();
        if (pCap) pCap->Release();
        if (pMoniker) pMoniker->Release();
        if (pEnum) pEnum->Release();
        if (pDevEnum) pDevEnum->Release();
        if (pBuild) pBuild->Release();
        if (pGraph) pGraph->Release();
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

std::vector<char> WebcamController::GetCurrentFrame() {
    std::lock_guard<std::mutex> lock(frameMutex);
    return currentFrame;
}