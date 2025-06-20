// main.cpp

/* @brief����ʾ�����û���ʾ�������IKapC���IKapBoard�⣬��ITEK��������в�ͼ��
 *
 * @brief��This example shows users how to use IKapC library and IKapBoard Library to grab images continuously with
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

/* @brief��ע��ص�������
 *
 * @brief��Register callback functions. */
void RegisterCallbackWithGrabber(pItkCamera pCam);

/* @brief������ص�������
 *
 * @brief��Unregister callback functions. */
void UnRegisterCallbackWithGrabber(pItkCamera pCam);

/* @brief����������ע��Ϊһ���ص���������ͼ��ɼ���ʼʱ�����������á�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When starting
 * grabbing images, the function will be called.
 * @param[in] pContext��Input parameter. */
void CALLBACK OnGrabStart(void* pContext);

/* @brief����������ע��Ϊһ���ص���������һ֡ͼ��ɼ����ʱ�����������á�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When a frame of
 * image grabbing ready, the function will be called.
 * @param[in] pContext��Input parameter. */
void CALLBACK OnFrameReady(void* pContext);

/* @brief����������ע��Ϊһ���ص���������ͼ��ɼ���ʱʱ�����������á�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When grabbing
 * images time out, the function will be called.
 * @param[in] pContext��Input parameter. */
void CALLBACK OnTimeout(void* pContext);

/* @brief����������ע��Ϊһ���ص����������ɼ���֡ʱ�����������á�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When grabbing
 * frame lost, the function will be called.
 * @param[in] pContext��Input parameter. */
void CALLBACK OnFrameLost(void* pContext);

/* @brief����������ע��Ϊһ���ص���������ͼ��ɼ�ֹͣʱ�����������á�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When stopping
 * grabbing images, the function will be called.
 * @param[in] pContext��Input parameter. */
void CALLBACK OnGrabStop(void* pContext);

/* @brief��ע��ص�������
 *
 * @brief��Register callback functions. */
void RegisterCallbackWithoutGrabber(pItkCamera pCam);

/* @brief������ص�������
 *
 * @brief��Unregister callback functions. */
void UnRegisterCallbackWithoutGrabber(pItkCamera pCam);

/* @brief����������ע��Ϊһ���ص�����������������ʼʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When data stream
 * starts, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbStartOfStream(uint32_t eventType, void* pContext);

/* @brief����������ע��Ϊһ���ص�������������������ʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When data stream
 * ends, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbEndOfStream(uint32_t eventType, void* pContext);

/* @brief����������ע��Ϊһ���ص���������һ֡ͼ��ɼ�����ʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When grabbing a
 * frame of image finished, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbOnEndOfFrame(uint32_t eventType, void* pContext);

/* @brief����������ע��Ϊһ���ص���������ͼ��ɼ���ʱʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When grabbing
 * images time out, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbOnTimeOut(uint32_t eventType, void* pContext);

/* @brief����������ע��Ϊһ���ص����������ɼ���֡ʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When grabbing
 * frame lost, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbOnFrameLost(uint32_t eventType, void* pContext);

int main()
{
    /// \~chinese IKapC ��������ֵ				\~english Return value of IKapC functions
    ITKSTATUS res = ITKSTATUS_OK;

    /// \~chinese IKapBoard ��������ֵ				\~english Return value of IKapBoard functions
    int ret = IK_RTN_OK;

    int camCount = 0;

    /*******************************/
    /// \~chinese ��ʼ�� IKapC ���л�����ֻ��Ҫ����һ��				\~english Initialize IKapC runtime environment, Only need to run once
    res = ItkManInitialize();
    CHECK(res);

    /// \~chinese ��ȡ��ǰ���ӵ�PC������豸��������ֻ��Ҫ����һ��				\~english Get the current number of camera devices connected to PC, Only need to run once
    uint32_t numDevices = 0;
    res                 = ItkManGetDeviceCount(&numDevices);
    CHECK(res);

    // ��û�����ӵ��豸ʱ��
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

    /// \~chinese �����Ϊ0���������������豸		\~english Open camera with 0 index and configure camera device
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
        {  /// \~chinese ����buffer����				    \~english Set count of buffers
            cam[i].g_bufferCount = 10;

            /// \~chinese ����Ҫ�ɼ���֡��			        \~english Set count of frames wanted
            cam[i].g_grabCount = ITKSTREAM_CONTINUOUS;

            /// \~chinese ����g_SerialNumberΪNULL������ConfigureCamera������ʹ��index�����			    \~english Set
            /// g_SerialNumber to NULL, ConfigureCamera function will use index to open camera later
            cam[i].g_SerialNumber = NULL;

            /// \~chinese �����������Ϊ�ر�			    \~english Disable softTrigger
            cam[i].g_bSoftTriggerUsed = 0;

            /// \~chinese ����chunkdataΪ�ر�			    \~english disable chunkdata
            cam[i].benableChunkData = false;

            /// \~chinese ���ñ���ͼƬ���ļ�����			\~english Set filename of image to be saved
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
            /// \~chinese �������������ʽ		            \~english Configure trigger method of the camera
            SetSoftTriggerWithGrabber(&cam[i]);

            /// \~chinese ���òɼ����豸				        \~english Configure frame grabber device
            ConfigureFrameGrabber(&cam[i]);
			
			/// \~chinese ע��ص�����                        \~english Register callback functions.
            RegisterCallbackWithGrabber(&cam[i]);

            /// \~chinese ��ʼͼ��ɼ�				        \~english Start grabbing images
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
            /// \~chinese �������������ʽ		            \~english Configure trigger method of the camera
            SetSoftTriggerWithoutGrabber(&cam[i]);

            /// \~chinese �������chunkdata		            \~english Configure chunkdata of the camera
            SetChunkData(&cam[i], cam[i].benableChunkData);

            /// \~chinese �����������ͻ�����				    \~english Create data stream and buffer
            CreateStreamAndBuffer(&cam[i]);

			/// \~chinese ע��ص�����                        \~english Register callback functions
            RegisterCallbackWithoutGrabber(&cam[i]);

            /// \~chinese ��ʼͼ��ɼ�				        \~english Start grabbing images
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

    /// \~chinese �ȴ�ͼ��ɼ�����				    \~english Wait for grabbing images finished
    { int getCharTmp = getchar(); }

    for (size_t i = 0; i < camCount; i++) {
        if (cam[i].g_hBoard != INVALID_HANDLE_VALUE) {
            /// \~chinese ֹͣͼ��ɼ�				        \~english Stop grabbing images
            ret = IKapStopGrab(cam[i].g_hBoard);
            CHECK_IKAPBOARD(ret);

            /// \~chinese ����ص�����				        \~english Unregister callback functions
            UnRegisterCallbackWithGrabber(&cam[i]);

            /// \~chinese �رղɼ����豸				        \~english Close frame grabber device
            ret = IKapClose(cam[i].g_hBoard);
            CHECK_IKAPBOARD(ret);

            /// \~chinese �ر�����豸				        \~english Close camera device
            res = ItkDevClose(cam[i].g_hCamera);
            CHECK_IKAPC(res);
        } else {
            /// \~chinese ֹͣͼ��ɼ�				    \~english Stop grabbing images
            ItkStreamStop(cam[i].g_hStream);

            /// \~chinese ����ص�����				    \~english Unregister callback functions
            UnRegisterCallbackWithoutGrabber(&cam[i]);

            /// \~chinese �ͷ��������ͻ�����				\~english Free data stream and buffers
            ItkDevFreeStream(cam[i].g_hStream);

            /// \~chinese �ر�����豸				    \~english Close camera device
            res = ItkDevClose(cam[i].g_hCamera);
            CHECK(res);

            if (cam[i].g_bufferData != NULL)
                free(cam[i].g_bufferData);
        }
    }

    delete[] cam;
    /// \~chinese �ͷ� IKapC ���л���				\~english Release IKapC runtime environment
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

    /// \~chinese ��ͼ�񻺳�����ʱ				      \~english When the buffer is full
    if (status.uFull == 1) {
        /// \~chinese ��ȡһ֡ͼ��Ĵ�С		      \~english Get the size of a frame of image
        ret = IKapGetInfo(cam->g_hBoard, IKP_FRAME_SIZE, &nFrameSize);
        CHECK_IKAPBOARD(ret);

        /// \~chinese ��������MultiExposureTimeCount����ֵ����1���Ҳɼ�����IKP_MULTIPLE_LIGHT_COUNT��������Ϊ��MultiExposureTimeCount��ͬ��ֵ�����˶����ع⹦�ܣ��ɼ�����ͼ�����ΪN���ع�ʱ�䣬[0~1*Height/N-1]�ж�ӦExposureSelectΪ1ʱ��ExposureTime��[1*Height/N~2*Height/N-1]�ж�ӦExposureSelectΪ2ʱ��ExposureTime��...��[(N-1)*Height/N~Height-1]�ж�ӦExposureSelectΪNʱ��ExposureTime.	\~english	If the MultiExposureTimeCount feature value is greater than 1, the multiple exposure function is turned on, The collected images were all divided into N exposure times, line [0~1 * Height / N-1] corresponds to the ExposureTime at a ExposureSelect of 1, line [1 * Height / N~2 * Height / N-1] corresponds to ExposureTime at ExposureSelect 2,..., line [(N-1) * Height / N~Height-1] corresponds to ExposureTime at ExposureSelect N.
        /// \~chinese ��ȡ��������ַ				  \~english Get the buffer address
        ret = IKapGetBufferAddress(cam->g_hBoard, nFrameIndex, (void**)&pUserBuffer);
        CHECK_IKAPBOARD(ret);

        /// \~chinese ����ͼ��				       \~english Save image
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

/// \~chinese  ע��ص�����		            \~english Register callback functions
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

/// \~chinese  ����ص�����		            \~english Unregister callback functions
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

    // \~chinese ��ȡ��ǰbuffer     \~english get current buffer    
    res = ItkStreamGetCurrentBuffer(cam->g_hStream, &hBuffer);
    CHECK(res);

    // \~chinese ��ȡ��ǰbuffer��Ϣ     \~english get current buffer info 
    res = ItkBufferGetInfo(hBuffer, &bufferInfo);
    CHECK(res);

    /// \~chinese ��ͼ�񻺳���������ͼ�񻺳������������޷��ɼ�������һ֡ͼ��ʱ				\~english When buffer is full or  buffer is not full but cannot grab a complete frame of image
    if (bufferInfo.State == ITKBUFFER_VAL_STATE_FULL || bufferInfo.State == ITKBUFFER_VAL_STATE_UNCOMPLETED) {

        /// \~chinese ��ȡchunk���ݣ�ǰ����ChunkModeActive�ѿ�����ChunkSelector�����¶�Ӧ������ʹ��      \~english Get chunk data provided that ChunkModeActive is on and the following corresponding parameters in ChunkSelector are enabled
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

            /// \~chinese  ֻ�в�ɫ���������Щ����		            \~english Only the color cameras have these parameters
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

        /// \~chinese �ж����ظ�ʽ�Ƿ��ת��                \~english Judge if need convert

        if (bufferInfo.NeedAutoConvert) {
            res = ItkBufferNew(1024, 1204, ITKBUFFER_VAL_FORMAT_MONO8, &hConvert);
            CHECK(res);
            res = ItkBufferConvert(hBuffer, hConvert, 0, ITKBUFFER_VAL_CONVERT_OPTION_AUTO_FORMAT);
            CHECK(res);
        }

        /// \~chinese ���MultiExposureTimeCount����ֵ����1�����˶����ع⹦�ܣ��ɼ�����ͼ�����ΪN���ع�ʱ�䣬[0~1*Height/N-1]�ж�ӦExposureSelectΪ1ʱ��ExposureTime��[1*Height/N~2*Height/N-1]�ж�ӦExposureSelectΪ2ʱ��ExposureTime��...��[(N-1)*Height/N~Height-1]�ж�ӦExposureSelectΪNʱ��ExposureTime.	\~english	If the MultiExposureTimeCount feature value is greater than 1, the multiple exposure function is turned on, The collected images were all divided into N exposure times, line [0~1 * Height / N-1] corresponds to the ExposureTime at a ExposureSelect of 1, line [1 * Height / N~2 * Height / N-1] corresponds to ExposureTime at ExposureSelect 2,..., line [(N-1) * Height / N~Height-1] corresponds to ExposureTime at ExposureSelect N.

        res = ItkBufferRead(hBuffer, 0, cam->g_bufferData, bufferInfo.ValidImageSize);
        CHECK(res);

        /// \~chinese ����ͼ��				    \~english Save image
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
