// main.cpp

/* @brief：本示例向用户演示如何运用IKapC库和IKapBoard库，对ITEK多相机进行采图。
 *
 * @brief：This example shows users how to use IKapC library and IKapBoard Library to grab images continuously with
 * ITEK cameras. */

#include <Windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "../GeneralConfigureCamera/ConfigureCamera.h"
#include "../GeneralGrabWithGrabber/GeneralGrabWithGrabber.h"
#include "../GeneralGrab/GeneralGrab.h"

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

/* @brief：本函数被注册为一个回调函数。当一帧图像采集结束时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When grabbing a
 * frame of image finished, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbOnEndOfFrame(uint32_t eventType, void* pContext);

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

int main()
{
    /// \~chinese IKapC 函数返回值				\~english Return value of IKapC functions
    ITKSTATUS res = ITKSTATUS_OK;

    /// \~chinese IKapBoard 函数返回值				\~english Return value of IKapBoard functions
    int ret = IK_RTN_OK;

    int camCount = 0;

    /*******************************/
    /// \~chinese 初始化 IKapC 运行环境，只需要运行一次				\~english Initialize IKapC runtime environment, Only need to run once
    res = ItkManInitialize();
    CHECK(res);

    /// \~chinese 获取当前连接到PC上相机设备的数量，只需要运行一次				\~english Get the current number of camera devices connected to PC, Only need to run once
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
    /*******************************/

    /// \~chinese 打开序号为0的相机并配置相机设备		\~english Open camera with 0 index and configure camera device
    printf("total device count: %d, the count of  which you want to open is:", numDevices);
    scanf_s("%d", &camCount);

    { int getCharTmp = getchar(); }

    if (camCount <= 0) {
        fprintf(stderr, "count: %d is not more than 0.\n", camCount);
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }
    if (camCount > (int)numDevices)
    {
        fprintf(stderr, "count: %d is more than %d.\n", camCount, numDevices);
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }
    ItkCamera* cam = new ItkCamera[camCount];

    for (int i = 0; i < camCount; i++) {
        {  /// \~chinese 设置buffer个数				    \~english Set count of buffers
            cam[i].g_bufferCount = 10;

            /// \~chinese 设置要采集的帧数			        \~english Set count of frames wanted
            cam[i].g_grabCount = ITKSTREAM_CONTINUOUS;

            /// \~chinese 设置g_SerialNumber为NULL，后续ConfigureCamera函数中使用index打开相机			    \~english Set
            /// g_SerialNumber to NULL, ConfigureCamera function will use index to open camera later
            cam[i].g_SerialNumber = NULL;

            /// \~chinese 设置相机软触发为关闭			    \~english Disable softTrigger
            cam[i].g_bSoftTriggerUsed = 0;

            /// \~chinese 设置chunkdata为关闭			    \~english disable chunkdata
            cam[i].benableChunkData = false;

            /// \~chinese 设置保存图片的文件名称			\~english Set filename of image to be saved
            sprintf_s(cam[i].g_saveFileName, "D:\\CImage%d.tif", i);
        }
        int tmpIndex = -1;
        printf("index of camera you want to open is:");
        scanf_s("%d", &tmpIndex);

        { int getCharTmp = getchar(); }

        if (tmpIndex >= (int)numDevices)
        {
            fprintf(stderr, "index: %d is more than %d.\n", tmpIndex, numDevices - 1);
            ItkManTerminate();
            pressEnterToExit();
            exit(EXIT_FAILURE);
        }
        if (tmpIndex < 0)
        {
            fprintf(stderr, "index: %d is less than %d.\n", tmpIndex, 0);
            ItkManTerminate();
            pressEnterToExit();
            exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < camCount; i++)
        {
            if (cam[i].g_index== tmpIndex)
            {
                fprintf(stderr, "index: %d is opened.\n", tmpIndex);
                ItkManTerminate();
                pressEnterToExit();
                exit(EXIT_FAILURE);
            }
        }

        ConfigureCamera(&cam[i], tmpIndex);
    }

    for (size_t i = 0; i < camCount; i++) {
        if (cam[i].g_hBoard != INVALID_HANDLE_VALUE) {
            /// \~chinese 配置相机触发方式		            \~english Configure trigger method of the camera
            SetSoftTriggerWithGrabber(&cam[i]);

            /// \~chinese 配置采集卡设备				        \~english Configure frame grabber device
            ConfigureFrameGrabber(&cam[i]);
			
			/// \~chinese 注册回调函数                        \~english Register callback functions.
            RegisterCallbackWithGrabber(&cam[i]);

            /// \~chinese 开始图像采集				        \~english Start grabbing images
            StartGrabImage(&cam[i]);

            if (cam[i].g_bSoftTriggerUsed) {
                printf("Please intput t to softTrigger a frame and input e to exit: ");
                while (true) {
                    char T = 0;
                    int scanTmp = scanf_s("%c", &T, (unsigned)sizeof(T));
                    if (T == 't') {
                        if (strcmp(cam[i].g_devInfo.DeviceClass, "CoaXPress") == 0 ||
                            strcmp(cam[i].g_devInfo.DeviceClass, "CameraLink") == 0) {
                            ret = IKapSetInfo(cam[i].g_hBoard, IKP_SOFTWARE_TRIGGER_START, 1);
                        } else {
                            res = ItkDevExecuteCommand(cam[i].g_hCamera, "TriggerSoftware");
                        }
                        CHECK_IKAPBOARD(ret);
                    } else if (T == 'e') {
                        break;
                    } else if (T != '\n') {
                        printf("Please intput t to softTrigger a frame and input e to exit: ");
                    }
                }
            }
        } else {
            /// \~chinese 配置相机触发方式		            \~english Configure trigger method of the camera
            SetSoftTriggerWithoutGrabber(&cam[i]);

            /// \~chinese 配置相机chunkdata		            \~english Configure chunkdata of the camera
            SetChunkData(&cam[i], cam[i].benableChunkData);

            /// \~chinese 创建数据流和缓冲区				    \~english Create data stream and buffer
            CreateStreamAndBuffer(&cam[i]);

			/// \~chinese 注册回调函数                        \~english Register callback functions
            RegisterCallbackWithoutGrabber(&cam[i]);

            /// \~chinese 开始图像采集				        \~english Start grabbing images
            res = ItkStreamStart(cam[i].g_hStream, cam[i].g_grabCount);
            CHECK(res);

            if (cam[i].g_bSoftTriggerUsed) {
                printf("Please intput t to softTrigger a frame and input e to exit: ");
                while (true) {
                    char T = 0;
                    int scanTmp = scanf_s("%c", &T, (unsigned)sizeof(T));
                    if (T == 't') {
                        res = ItkDevExecuteCommand(cam[i].g_hCamera, "TriggerSoftware");
                    } else if (T == 'e') {
                        break;
                    } else if (T != '\n') {
                        printf("Please intput t to softTrigger a frame and input e to exit: ");
                    }
                }
            }
        }
    }

    /// \~chinese 等待图像采集结束				    \~english Wait for grabbing images finished
    { int getCharTmp = getchar(); }

    for (size_t i = 0; i < camCount; i++) {
        if (cam[i].g_hBoard != INVALID_HANDLE_VALUE) {
            /// \~chinese 停止图像采集				        \~english Stop grabbing images
            ret = IKapStopGrab(cam[i].g_hBoard);
            CHECK_IKAPBOARD(ret);

            /// \~chinese 清除回调函数				        \~english Unregister callback functions
            UnRegisterCallbackWithGrabber(&cam[i]);

            /// \~chinese 关闭采集卡设备				        \~english Close frame grabber device
            ret = IKapClose(cam[i].g_hBoard);
            CHECK_IKAPBOARD(ret);

            /// \~chinese 关闭相机设备				        \~english Close camera device
            res = ItkDevClose(cam[i].g_hCamera);
            CHECK_IKAPC(res);
        } else {
            /// \~chinese 停止图像采集				    \~english Stop grabbing images
            ItkStreamStop(cam[i].g_hStream);

            /// \~chinese 清除回调函数				    \~english Unregister callback functions
            UnRegisterCallbackWithoutGrabber(&cam[i]);

            /// \~chinese 释放数据流和缓冲区				\~english Free data stream and buffers
            ItkDevFreeStream(cam[i].g_hStream);

            /// \~chinese 关闭相机设备				    \~english Close camera device
            res = ItkDevClose(cam[i].g_hCamera);
            CHECK(res);

            if (cam[i].g_bufferData != NULL)
                free(cam[i].g_bufferData);
        }
    }

    delete[] cam;
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
        /*
        ret=IKapSaveBuffer(cam->g_hBoard,nFrameIndex,cam->g_saveFileName,IKP_DEFAULT_COMPRESSION);
        CHECK_IKAPBOARD(ret);

        */
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

/// \~chinese  注册回调函数		            \~english Register callback functions
void RegisterCallbackWithoutGrabber(pItkCamera pCam)
{
    ITKSTATUS res = ITKSTATUS_OK;

    res = ItkStreamRegisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_START_OF_STREAM, cbStartOfStream, pCam);
    CHECK(res);
    res = ItkStreamRegisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_END_OF_STREAM, cbEndOfStream, pCam);
    CHECK(res);
    res = ItkStreamRegisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_END_OF_FRAME, cbOnEndOfFrame, pCam);
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
    res = ItkStreamUnregisterCallback(pCam->g_hStream, ITKSTREAM_VAL_EVENT_TYPE_END_OF_FRAME);
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

void IKAPC_CC cbOnEndOfFrame(uint32_t eventType, void* pContext)
{
    ITKSTATUS  res = ITKSTATUS_OK;

    ITKBUFFER  hBuffer = NULL;
    pItkCamera cam = (pItkCamera)pContext;
    ITK_BUFFER_INFO bufferInfo = { 0 };
    printf("On end of frame of camera with serialNumber:%s. \n", cam->g_devInfo.SerialNumber);

    // \~chinese 获取当前buffer     \~english get current buffer    
    res = ItkStreamGetCurrentBuffer(cam->g_hStream, &hBuffer);
    CHECK(res);

    // \~chinese 获取当前buffer信息     \~english get current buffer info 
    res = ItkBufferGetInfo(hBuffer, &bufferInfo);
    CHECK(res);

    /// \~chinese 当图像缓冲区满或者图像缓冲区非满但是无法采集完整的一帧图像时				\~english When buffer is full or  buffer is not full but cannot grab a complete frame of image
    if (bufferInfo.State == ITKBUFFER_VAL_STATE_FULL || bufferInfo.State == ITKBUFFER_VAL_STATE_UNCOMPLETED) {

        /// \~chinese 获取chunk数据，前提是ChunkModeActive已开启，ChunkSelector中以下对应参数已使能      \~english Get chunk data provided that ChunkModeActive is on and the following corresponding parameters in ChunkSelector are enabled
        if (cam->benableChunkData)
        {
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

            char strChunkPixelFormat[128] = { 0 };
            uint32_t strLen = sizeof(strChunkPixelFormat);
            res = ItkBufferToChunkString(hBuffer, "ChunkPixelFormat",
                strChunkPixelFormat, &strLen);

            int64_t nChunkTimestamp = 0;
            res =
                ItkBufferGetChunkInt64(hBuffer, "ChunkTimestamp", &nChunkTimestamp);
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

        res = ItkBufferRead(hBuffer, 0, cam->g_bufferData, bufferInfo.ValidImageSize);
        CHECK(res);

        /// \~chinese 保存图像				    \~english Save image
        /*
        res = ItkBufferSave(hBuffer,cam->g_saveFileName,ITKBUFFER_VAL_TIFF);
        CHECK(res);
        */

        if (nullptr != hConvert)
            ItkBufferFree(hConvert);
    }
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
