// main.cpp

/* @brief：本示例向用户演示如何运用IKapC库和IKapBoard库，通过采集卡连接的相机连续采集图像。
 *
 * @brief：This example shows users how to use IKapC and IKapBoard libraries to
 * grab images continuously with grabber linked camera. */

#include <malloc.h>
#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "../GeneralConfigureCamera/ConfigureCamera.h"
#include "GeneralGrabWithGrabber.h"

#pragma comment(lib, "IKapBoard.lib")
#pragma comment(lib, "IKapC.lib")
#pragma comment(lib, "shlwapi.lib")

/* @brief：注册回调函数。
 *
 * @brief：Register callback functions. */
void RegisterCallbackWithGrabber(pItkCamera pCam);

/* @brief：清除回调函数。
 *
 * @brief：Unregister callback functions. */
void UnRegisterCallbackWithGrabber(pItkCamera pCam);

/* @brief：本函数被注册为一个回调函数。当图像采集开始时，函数被调用。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When starting
 * grabbing images, the function will be called.
 * @param[in] pContext：Input parameter. */
void CALLBACK OnGrabStart(void* pContext);

/* @brief：本函数被注册为一个回调函数。当一帧图像采集完成时，函数被调用。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When a frame of
 * image grabbing ready, the function will be called.
 * @param[in] pContext：Input parameter. */
void CALLBACK OnFrameReady(void* pContext);

/* @brief：本函数被注册为一个回调函数。当图像采集超时时，函数被调用。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When grabbing
 * images time out, the function will be called.
 * @param[in] pContext：Input parameter. */
void CALLBACK OnTimeout(void* pContext);

/* @brief：本函数被注册为一个回调函数。当采集丢帧时，函数被调用。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When grabbing
 * frame lost, the function will be called.
 * @param[in] pContext：Input parameter. */
void CALLBACK OnFrameLost(void* pContext);

/* @brief：本函数被注册为一个回调函数。当图像采集停止时，函数被调用。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When stopping
 * grabbing images, the function will be called.
 * @param[in] pContext：Input parameter. */
void CALLBACK OnGrabStop(void* pContext);

int main()
{
    /// \~chinese IKapC 函数返回值				    \~english Return value of IKapC functions
    ITKSTATUS res = ITKSTATUS_OK;

    /// \~chinese IKapBoard 函数返回值				\~english Return value of IKapBoard functions
    int ret = IK_RTN_OK;

    /// \~chinese 初始化 IKapC 运行环境				\~english Initialize IKapC runtime environment
    res = ItkManInitialize();
    CHECK_IKAPC(res);

    ItkCamera cam;

    cam.g_bufferCount = 10;

    sprintf_s(cam.g_saveFileName, "D:\\image\CImage.tif");

    cam.g_SerialNumber = NULL;

    cam.g_bSoftTriggerUsed = 1;

    cam.g_bLoadGrabberConfig = 1;

    uint32_t numDevices = 0;
    res                 = ItkManGetDeviceCount(&numDevices);
    CHECK(res);

    // 当没有连接的设备时。
    //
    // When there is no connected devices.
    if (numDevices == 0) {
        fprintf(stderr, "No device.\n");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    printCameraInfo(numDevices);

    /// \~chinese 打开序号为0的相机并配置相机设备		\~english Open camera with 0 index and configure camera device
    int tmpIndex = -1;
    printf("total device count: %d, the index of  which you want to open is:", numDevices);
    scanf_s("%d", &tmpIndex);

    { int getCharTmp = getchar(); }

    ConfigureCamera(&cam, tmpIndex);

    if (cam.g_hBoard == INVALID_HANDLE_VALUE) {
        fprintf(stderr,"Please select camera with grabber.\n");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    /// \~chinese 配置采集卡设备				        \~english Configure frame grabber device
    ConfigureFrameGrabber(&cam);
	
	/// \~chinese 注册回调函数                        \~english Register callback functions.
    RegisterCallbackWithGrabber(&cam);

    /// \~chinese 配置相机触发方式		            \~english Configure trigger method of the camera
    SetSoftTriggerWithGrabber(&cam);

    /// \~chinese 开始图像采集				        \~english Start grabbing images
    StartGrabImage(&cam);

    if (cam.g_bSoftTriggerUsed) {
        printf("Please intput t to softTrigger a frame and input e to exit: ");
        while (true) {
            char T = 0;
          int scanTmp = scanf_s("%c", &T, (unsigned)sizeof(T));
            if (T == 't') {
                if (strcmp(cam.g_devInfo.DeviceClass, "CoaXPress") == 0 ||
                    strcmp(cam.g_devInfo.DeviceClass, "CameraLink") == 0) {
                    ret = IKapSetInfo(cam.g_hBoard, IKP_SOFTWARE_TRIGGER_START, 1);
                } else {
                    res = ItkDevExecuteCommand(cam.g_hCamera, "TriggerSoftware");
                }
                CHECK_IKAPBOARD(ret);
            } else if (T == 'e') {
                break;
            } else if (T != '\n') {
                printf("Please intput t to softTrigger a frame and input e to exit: ");
            }
        }
    }

    /// \~chinese 等待图像采集结束				    \~english Wait for grabbing images finished
    { int getCharTmp = getchar(); }

    /// \~chinese 停止图像采集				        \~english Stop grabbing images
    ret = IKapStopGrab(cam.g_hBoard);
    CHECK_IKAPBOARD(ret);

    /// \~chinese 清除回调函数				        \~english Unregister callback functions
    UnRegisterCallbackWithGrabber(&cam);

    /// \~chinese 关闭采集卡设备				        \~english Close frame grabber device
    ret = IKapClose(cam.g_hBoard);
    CHECK_IKAPBOARD(ret);

    /// \~chinese 关闭相机设备				        \~english Close camera device
    res = ItkDevClose(cam.g_hCamera);
    CHECK_IKAPC(res);

	/// \~chinese 释放用户申请的用于设置Buffer地址的内存				    \~english Release the memory that the user requests for setting the Buffer address
	/*if (cam.g_user_buffer!=NULL)
	free(cam.g_user_buffer);*/

    /// \~chinese 释放 IKapC 运行环境				\~english Release IKapC runtime environment
    ItkManTerminate();

    pressEnterToExit();
    return 0;
}

void RegisterCallbackWithGrabber(pItkCamera pCam)
{
    int ret = IK_RTN_OK;

    ret = IKapRegisterCallback(pCam->g_hBoard, IKEvent_GrabStart, OnGrabStart, pCam);
    CHECK_IKAPBOARD(ret);
    ret = IKapRegisterCallback(pCam->g_hBoard, IKEvent_FrameReady, OnFrameReady, pCam);
    CHECK_IKAPBOARD(ret);
    ret = IKapRegisterCallback(pCam->g_hBoard, IKEvent_FrameLost, OnFrameLost, pCam);
    CHECK_IKAPBOARD(ret);
    ret = IKapRegisterCallback(pCam->g_hBoard, IKEvent_TimeOut, OnTimeout, pCam);
    CHECK_IKAPBOARD(ret);
    ret = IKapRegisterCallback(pCam->g_hBoard, IKEvent_GrabStop, OnGrabStop, pCam);
    CHECK_IKAPBOARD(ret);
}

void UnRegisterCallbackWithGrabber(pItkCamera pCam)
{
    int ret = IK_RTN_OK;

    ret = IKapUnRegisterCallback(pCam->g_hBoard, IKEvent_GrabStart);
    CHECK_IKAPBOARD(ret);

    ret = IKapUnRegisterCallback(pCam->g_hBoard, IKEvent_FrameReady);
    CHECK_IKAPBOARD(ret);

    ret = IKapUnRegisterCallback(pCam->g_hBoard, IKEvent_FrameLost);
    CHECK_IKAPBOARD(ret);

    ret = IKapUnRegisterCallback(pCam->g_hBoard, IKEvent_TimeOut);
    CHECK_IKAPBOARD(ret);

    ret = IKapUnRegisterCallback(pCam->g_hBoard, IKEvent_GrabStop);
    CHECK_IKAPBOARD(ret);
}

void CALLBACK OnGrabStart(void* pContext)
{
    printf("Start grabbing image.\n");
}

void CALLBACK OnFrameReady(void* pContext)
{
    ITKSTATUS res = ITKSTATUS_OK;

    int ret = IK_RTN_OK;

    pItkCamera cam = (pItkCamera)pContext;
    printf("Grab frame ready of camera with serialNumber:%s.\n", cam->g_devInfo.SerialNumber);
    unsigned char*   pUserBuffer = NULL;
    int              nFrameSize  = 0;
    int              nFrameCount = 0;
    int              nFrameIndex = 0;
    IKAPBUFFERSTATUS status;

    ret = IKapGetInfo(cam->g_hBoard, IKP_FRAME_COUNT, &nFrameCount);
    CHECK_IKAPBOARD(ret);

    ret = IKapGetInfo(cam->g_hBoard, IKP_CURRENT_BUFFER_INDEX, &nFrameIndex);
    CHECK_IKAPBOARD(ret);

    ret = IKapGetBufferStatus(cam->g_hBoard, nFrameIndex, &status);
    CHECK_IKAPBOARD(ret);

    /// \~chinese 当图像缓冲区满时				      \~english When the buffer is full
    if (status.uFull == 1) {
        /// \~chinese 获取一帧图像的大小		      \~english Get the size of a frame of image
        ret = IKapGetInfo(cam->g_hBoard, IKP_FRAME_SIZE, &nFrameSize);
        CHECK_IKAPBOARD(ret);

        /// \~chinese 如果相机的MultiExposureTimeCount特征值大于1并且采集卡的IKP_MULTIPLE_LIGHT_COUNT参数设置为与MultiExposureTimeCount相同的值则开启了多重曝光功能，采集到的图像均分为N种曝光时间，[0~1*Height/N-1]行对应ExposureSelect为1时的ExposureTime，[1*Height/N~2*Height/N-1]行对应ExposureSelect为2时的ExposureTime，...，[(N-1)*Height/N~Height-1]行对应ExposureSelect为N时的ExposureTime.	\~english	If the MultiExposureTimeCount feature value is greater than 1, the multiple exposure function is turned on, The collected images were all divided into N exposure times, line [0~1 * Height / N-1] corresponds to the ExposureTime at a ExposureSelect of 1, line [1 * Height / N~2 * Height / N-1] corresponds to ExposureTime at ExposureSelect 2,..., line [(N-1) * Height / N~Height-1] corresponds to ExposureTime at ExposureSelect N.
        /// \~chinese 获取缓冲区地址				  \~english Get the buffer address
        ret = IKapGetBufferAddress(cam->g_hBoard, nFrameIndex, (void**)&pUserBuffer);
        CHECK_IKAPBOARD(ret);

        /// \~chinese 保存图像				       \~english Save image
        
        ret=IKapSaveBuffer(cam->g_hBoard,nFrameIndex,cam->g_saveFileName,IKP_DEFAULT_COMPRESSION);
        CHECK_IKAPBOARD(ret);
    }
}

void CALLBACK OnTimeout(void* pContext)
{
    printf("Grab image timeout.\n");
}

void CALLBACK OnFrameLost(void* pContext)
{
    printf("Grab frame lost.\n");
}

void CALLBACK OnGrabStop(void* pContext)
{
    printf("Stop grabbing image.\n");
}
