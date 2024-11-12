//
// Created by phida on 10/11/2024.
//

#include <dshow.h>
#include <iostream>
#include <windows.h>
#include <dshow.h>
#include <iostream>
#include <conio.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

// #include <atlbase.h>

using namespace std;

#ifndef WEBCAMCONTROLLER_H
#define WEBCAMCONTROLLER_H


class WebcamController {
private:
    IGraphBuilder *pGraph = NULL;
    ICaptureGraphBuilder2 *pBuild = NULL;
    IMediaControl *pControl = NULL;
    IBaseFilter *pCap = NULL;
    std::atomic<bool> isRunning{false};
    std::atomic<bool> stopRequested{false};
    std::mutex mtx;
    std::condition_variable cv;

    void WebcamThread();

public:
    bool IsWebcamRunning() const { return isRunning; }

    HRESULT InitializeGraph();

    HRESULT EnumerateDevices();

    void StartWebcam();

    void StopWebcam();

    void CleanUp();
};

#endif //WEBCAMCONTROLLER_H
