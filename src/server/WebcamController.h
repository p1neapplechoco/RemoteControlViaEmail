//
// Created by phida on 10/11/2024.
//

#include <dshow.h>
#include <iostream>
#include <windows.h>
#include <dshow.h>
#include <iostream>
#include <conio.h>
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

public:
    HRESULT InitializeGraph();

    HRESULT EnumerateDevices();

    HRESULT CaptureImage();

    void StartWebcam();

    HRESULT StopWebcam();

    void CleanUp();
};

#endif //WEBCAMCONTROLLER_H
