// main.cpp

/* @brief：本示例向用户演示如何运用IKapC库中ItkStreamWaitOneFrameReady接口，通过直接连接的相机连续采集图像。
 *
 * @brief：This example shows users how to use IKapC library to grab images continuously with
 * directly linked camera and ItkStreamWaitOneFrameReady api. */

#include <Windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#include <vector>

#include "../GeneralConfigureCamera/ConfigureCamera.h"
#include "../GeneralGrab/GeneralGrab.h"

#pragma comment(lib, "IKapBoard.lib")
#pragma comment(lib, "IKapC.lib")

// 指示是否退出程序
// 
// indicate whether exit  the program or not
bool g_bExit = false;

/* @brief：注册回调函数。
 *
 * @brief：Register callback functions. */
void RegisterCallbackWithoutGrabber(pItkCamera pCam);

/* @brief：清除回调函数。
 *
 * @brief：Unregister callback functions. */
void UnRegisterCallbackWithoutGrabber(pItkCamera pCam);

/* @brief：本函数被注册为一个回调函数。当数据流开始时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When data stream
 * starts, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbStartOfStream(uint32_t eventType, void* pContext);

/* @brief：本函数被注册为一个回调函数。当数据流结束时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When data stream
 * ends, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbEndOfStream(uint32_t eventType, void* pContext);

/* @brief：本函数被注册为一个回调函数。当图像采集超时时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When grabbing
 * images time out, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbOnTimeOut(uint32_t eventType, void* pContext);

/* @brief：本函数被注册为一个回调函数。当采集丢帧时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When grabbing
 * frame lost, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbOnFrameLost(uint32_t eventType, void* pContext);

/* @brief：本函数是工作线程的执行函数，用于采集图像。
 * @param[in] pUser：输入参数。
 *
 * @brief：This function is the execution function of the work thread for image acquisition.
 * @param[in] pUser：Input parameter. */
unsigned int __stdcall WorkThread(void* pUser);

int main(void)
{
    /// \~chinese IKapC 函数返回值				\~english Return value of IKapC functions
    ITKSTATUS res = ITKSTATUS_OK;

    ItkCamera cam;

    /// \~chinese 设置buffer个数				    \~english Set count of buffers
    cam.g_bufferCount = 10;

    /// \~chinese 设置要采集的帧数			        \~english Set count of frames wanted
    cam.g_grabCount = ITKSTREAM_CONTINUOUS;

    /// \~chinese 设置g_SerialNumber为NULL，后续ConfigureCamera函数中使用index打开相机			    \~english Set
    /// g_SerialNumber to NULL, ConfigureCamera function will use index to open camera later
    cam.g_SerialNumber = NULL;

    /// \~chinese 设置相机软触发为打开			    \~english Enable softTrigger
    cam.g_bSoftTriggerUsed = 1;

    /// \~chinese 设置chunkdata为关闭			    \~english disable chunkdata
    cam.benableChunkData = false;

    /// \~chinese 设置保存图片的文件名称			\~english Set filename of image to be saved
    sprintf_s(cam.g_saveFileName, "D:\\CImage.tif");

    printf("Itek Console GrabWithoutGrabber_WaitOneFrameReady Example(C version)\n");

    /// \~chinese 初始化 IKapC 运行环境				\~english Initialize IKapC runtime environment
    res = ItkManInitialize();
    CHECK(res);

    uint32_t numDevices = 0;
    res = ItkManGetDeviceCount(&numDevices);
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

    if (cam.g_hBoard != INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Please select camera without grabber.\n");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }
    /// \~chinese 配置相机触发方式		            \~english Configure trigger method of the camera
    SetSoftTriggerWithoutGrabber(&cam);

    /// \~chinese 配置相机chunkdata		            \~english Configure chunkdata of the camera
    SetChunkData(&cam, cam.benableChunkData);

    /// \~chinese 创建数据流和缓冲区				    \~english Create data stream and buffer
    CreateStreamAndBuffer(&cam);

    /// \~chinese 设置waitOneFrameReady的取流策略       \~english Set grab strategy for waitOneFrameReady api
    uint32_t grabstrategy = ITKSTREAM_VAL_GRAB_STRATEGY_ONE_BY_ONE;
    res = ItkStreamSetPrm(cam.g_hStream,ITKSTREAM_PRM_GRAB_STRATEGY,&grabstrategy);
    CHECK(res);

    /// \~chinese 注册回调函数                        \~english Register callback functions
    RegisterCallbackWithoutGrabber(&cam);

    /// \~chinese 开始图像采集				        \~english Start grabbing images
    res = ItkStreamStart(cam.g_hStream, cam.g_grabCount);
    CHECK(res);

    /// \~chinese 创建工作线程用于获取图像              \~english Create a worker thread that is used to acquire the images
    unsigned int nThreadID = 0;
    void* hThreadHandle = (void*)_beginthreadex(NULL, 0, WorkThread, &cam, 0, &nThreadID);
    if (NULL == hThreadHandle)
    {
        printf("fail to create workThread to grab image.\n");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    if (cam.g_bSoftTriggerUsed) {
        printf("Please intput t to softTrigger a frame and input e to exit: ");
        while (true) {
            char T = 0;
          int tmp = scanf_s("%c", &T, (unsigned)sizeof(T));
            if (T == 't') {
                res = ItkDevExecuteCommand(cam.g_hCamera, "TriggerSoftware");
            }
            else if (T == 'e') {
                break;
            }
            else if (T != '\n') {
                printf("Please intput t to softTrigger a frame and input e to exit: ");
            }
        }
    }

    /// \~chinese 等待图像采集结束				    \~english Wait for grabbing images finished
    { int getCharTmp = getchar(); }

    g_bExit = true;

    /// \~chinese 停止工作线程                    \~english stop workThread
    WaitForSingleObject(hThreadHandle, INFINITE);
    CloseHandle(hThreadHandle);

    /// \~chinese 停止图像采集				    \~english Stop grabbing images
    ItkStreamStop(cam.g_hStream);

    /// \~chinese 清除回调函数				    \~english Unregister callback functions
    UnRegisterCallbackWithoutGrabber(&cam);

    /// \~chinese 释放数据流和缓冲区				\~english Free data stream and buffers
    ItkDevFreeStream(cam.g_hStream);

    /// \~chinese 关闭相机设备				    \~english Close camera device
    res = ItkDevClose(cam.g_hCamera);
    CHECK(res);

    /// \~chinese 释放用户申请的用于存放缓冲区数据的内存				    \~english Release the memory for storing the buffer data
    if (cam.g_bufferData != NULL)
        free(cam.g_bufferData);

    /// \~chinese 释放用户申请的用于设置Buffer地址的内存				    \~english Release the memory that the user requests for setting the Buffer address
    /*if (cam.g_user_buffer!=NULL)
        free(cam.g_user_buffer);*/

    /// \~chinese 释放 IKapC 运行环境				\~english Release IKapC runtime environment
    ItkManTerminate();

    pressEnterToExit();
    return EXIT_SUCCESS;
}

/// \~chinese  注册回调函数		            \~english Register callback functions
void RegisterCallbackWithoutGrabber(pItkCamera pCam)
{
    ITKSTATUS res = ITKSTATUS_OK;

    res = ItkStreamRegisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_START_OF_STREAM, cbStartOfStream, pCam);
    CHECK(res);
    res = ItkStreamRegisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_END_OF_STREAM, cbEndOfStream, pCam);
    CHECK(res);
    res = ItkStreamRegisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_TIME_OUT, cbOnTimeOut, pCam);
    CHECK(res);
    res = ItkStreamRegisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_FRAME_LOST, cbOnFrameLost, pCam);
    CHECK(res);
}

/// \~chinese  清除回调函数		            \~english Unregister callback functions
void UnRegisterCallbackWithoutGrabber(pItkCamera pCam)
{
    ITKSTATUS res = ITKSTATUS_OK;

    res = ItkStreamUnregisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_START_OF_STREAM);
    res = ItkStreamUnregisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_END_OF_STREAM);
    res = ItkStreamUnregisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_TIME_OUT);
    res = ItkStreamUnregisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_FRAME_LOST);
}

void IKAPC_CC cbStartOfStream(uint32_t eventType, void* pContext)
{
    pItkCamera cam = (pItkCamera)pContext;
    printf("On start of stream of camera with serialNumber:%s. \n", cam->g_devInfo.SerialNumber);
}

void IKAPC_CC cbEndOfStream(uint32_t eventType, void* pContext)
{
    pItkCamera cam = (pItkCamera)pContext;
    printf("On end of stream of camera with serialNumber:%s. \n", cam->g_devInfo.SerialNumber);
}

unsigned int __stdcall WorkThread(void* pUser)
{
    ITKSTATUS  res = ITKSTATUS_OK;
    ITKBUFFER  hBuffer = NULL;
    ITK_BUFFER_INFO bufferInfo = {0};
    pItkCamera cam = (pItkCamera)pUser;
    if (NULL == cam)
    {
        return 0;
    }
    do
    {
        /// \~chinese  timeout_ms 需要大于相机实际帧周期，此处需要根据实际情况修改       \~english timeout_ms needs to be greater than the actual camera frame period, which needs to be modified according to the actual situation
      res = ItkStreamWaitOneFrameReady(cam->g_hStream, &hBuffer, 5000);
      if (g_bExit)
      {
          break;
      }
      CHECK(res);

      printf("On end of frame of camera with serialNumber:%s. \n",
             cam->g_devInfo.SerialNumber);

      res = ItkBufferGetInfo(hBuffer,&bufferInfo);
      CHECK(res);

        /// \~chinese 当图像缓冲区满或者图像缓冲区非满但是无法采集完整的一帧图像时				\~english When buffer is full or  buffer is not full but cannot grab a complete frame of image
      if (bufferInfo.State == ITKBUFFER_VAL_STATE_FULL ||
          bufferInfo.State == ITKBUFFER_VAL_STATE_UNCOMPLETED) {

        /// \~chinese 获取chunk数据，前提是ChunkModeActive已开启，ChunkSelector中以下对应参数已使能      \~english Get chunk data provided that ChunkModeActive is on and the following corresponding parameters in ChunkSelector are enabled
        if (cam->benableChunkData) {
          int64_t nChunkWidth = 0;
          res = ItkBufferGetChunkInt64(hBuffer, "ChunkWidth", &nChunkWidth);
          CHECK(res);

          int64_t nChunkHeight = 0;
          res = ItkBufferGetChunkInt64(hBuffer, "ChunkHeight", &nChunkHeight);
          CHECK(res);

          int64_t nChunkOffsetX = 0;
          res = ItkBufferGetChunkInt64(hBuffer, "ChunkOffsetX", &nChunkOffsetX);
          CHECK(res);

          int64_t nChunkOffsetY = 0;
          res = ItkBufferGetChunkInt64(hBuffer, "ChunkOffsetY", &nChunkOffsetY);
          CHECK(res);

          char strChunkPixelFormat[128] = {0};
          uint32_t strLen = sizeof(strChunkPixelFormat);
          res = ItkBufferToChunkString(hBuffer, "ChunkPixelFormat",
                                       strChunkPixelFormat, &strLen);

          int64_t nChunkTimestamp = 0;
          res = ItkBufferGetChunkInt64(hBuffer, "ChunkTimestamp",
                                       &nChunkTimestamp);
          CHECK(res);

          double dchunkGain = 0;
          res = ItkBufferGetChunkDouble(hBuffer, "ChunkGain", &dchunkGain);
          CHECK(res);

          /// \~chinese  只有彩色相机才有这些参数		            \~english Only the color cameras have these parameters
          /*{
              double dchunkRedGain = 0;
              res =
                  ItkBufferGetChunkDouble(hBuffer, "ChunkRedGain", &dchunkRedGain);
              CHECK(res);

              double dChunkG1Gain = 0;
              res = ItkBufferGetChunkDouble(hBuffer, "ChunkG1Gain", &dChunkG1Gain);
              CHECK(res);

              double dChunkG2Gain = 0;
              res = ItkBufferGetChunkDouble(hBuffer, "ChunkG2Gain", &dChunkG2Gain);
              CHECK(res);

              double dChunkBlueGain = 0;
              res = ItkBufferGetChunkDouble(hBuffer, "ChunkBlueGain",
                                            &dChunkBlueGain);
              CHECK(res);
          } */

            int64_t nChunkExposureTime = 0;
          res = ItkBufferGetChunkInt64(hBuffer, "ChunkExposureTime",
                                       &nChunkExposureTime);
          CHECK(res);

          int64_t nChunkBrightnessInfo = 0;
          res = ItkBufferGetChunkInt64(hBuffer, "ChunkBrightnessInfo",
                                       &nChunkBrightnessInfo);
          CHECK(res);

          int64_t nChunkFrameCounter = 0;
          res = ItkBufferGetChunkInt64(hBuffer, "ChunkFrameCounter",
                                       &nChunkFrameCounter);
          CHECK(res);

          int64_t nChunkExtTriggerCount = 0;
          res = ItkBufferGetChunkInt64(hBuffer, "ChunkExtTriggerCount",
                                       &nChunkExtTriggerCount);
          CHECK(res);
        }

        ITKBUFFER hConvert = nullptr;

        /// \~chinese 判断像素格式是否可转换                \~english Judge if need convert
        if (bufferInfo.NeedAutoConvert) {
            res = ItkBufferNew(1024, 1204, ITKBUFFER_VAL_FORMAT_MONO8, &hConvert);
            CHECK(res);
            res = ItkBufferConvert(hBuffer, hConvert, 0, ITKBUFFER_VAL_CONVERT_OPTION_AUTO_FORMAT);
            CHECK(res);
        }

        /// \~chinese 如果MultiExposureTimeCount特征值大于1则开启了多重曝光功能，采集到的图像均分为N种曝光时间，[0~1*Height/N-1]行对应ExposureSelect为1时的ExposureTime，[1*Height/N~2*Height/N-1]行对应ExposureSelect为2时的ExposureTime，...，[(N-1)*Height/N~Height-1]行对应ExposureSelect为N时的ExposureTime.	\~english	If the MultiExposureTimeCount feature value is greater than 1, the multiple exposure function is turned on, The collected images were all divided into N exposure times, line [0~1 * Height / N-1] corresponds to the ExposureTime at a ExposureSelect of 1, line [1 * Height / N~2 * Height / N-1] corresponds to ExposureTime at ExposureSelect 2,..., line [(N-1) * Height / N~Height-1] corresponds to ExposureTime at ExposureSelect N.

        res = ItkBufferRead(hBuffer, 0, cam->g_bufferData, (uint32_t)bufferInfo.ValidImageSize);
        CHECK(res);

        /// \~chinese 保存图像				    \~english Save image
        /*
        res = ItkBufferSave(hBuffer,cam->g_saveFileName,ITKBUFFER_VAL_TIFF);
        CHECK(res);
        */

        /// \~chinese 需要调用ItkStreamClearBuffer将Buffer释放     \~english need to call ItkStreamClearBuffer to release buffer
        res = ItkStreamClearBuffer(cam->g_hStream, hBuffer);
        CHECK(res);

        if (nullptr != hConvert)
          ItkBufferFree(hConvert);
      }
    } while (!g_bExit);
    return 0;
}

void IKAPC_CC cbOnTimeOut(uint32_t eventType, void* pContext)
{
    pItkCamera cam = (pItkCamera)pContext;
    printf("on time out of camera with serialNumber:%s. \n", cam->g_devInfo.SerialNumber);
}

void IKAPC_CC cbOnFrameLost(uint32_t eventType, void* pContext)
{
    pItkCamera cam = (pItkCamera)pContext;
    printf("on frame lost of camera with serialNumber:%s. \n", cam->g_devInfo.SerialNumber);
}

