// main.cpp

/* @brief����ʾ�����û���ʾ�������IKapC�⣬ͨ��ֱ�����ӵ���������ɼ�ͼ��
 *
 * @brief��This example shows users how to use IKapC library to grab images continuously with
 * directly linked camera. */

#include <Windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "../GeneralConfigureCamera/ConfigureCamera.h"
#include "GeneralGrab.h"

#pragma comment(lib, "IKapBoard.lib")
#pragma comment(lib, "IKapC.lib")

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

int main(void)
{
    /// \~chinese IKapC ��������ֵ				\~english Return value of IKapC functions
    ITKSTATUS res = ITKSTATUS_OK;

    ItkCamera cam;

    /// \~chinese ����buffer����				    \~english Set count of buffers
    cam.g_bufferCount = 10;

    /// \~chinese ����Ҫ�ɼ���֡��			        \~english Set count of frames wanted
    cam.g_grabCount = ITKSTREAM_CONTINUOUS;

    /// \~chinese ����g_SerialNumberΪNULL������ConfigureCamera������ʹ��index�����			    \~english Set
    /// g_SerialNumber to NULL, ConfigureCamera function will use index to open camera later
    cam.g_SerialNumber = NULL;

    /// \~chinese �����������Ϊ��			    \~english Enable softTrigger
    cam.g_bSoftTriggerUsed = 0;

    /// \~chinese ����chunkdataΪ�ر�			    \~english disable chunkdata
    cam.benableChunkData = false;

    /// \~chinese ���ñ���ͼƬ���ļ�����			\~english Set filename of image to be saved
    sprintf_s(cam.g_saveFileName, "D:\\CImage.tif");

    printf("Itek Console GrabWithoutGrabber Example(C version)\n");

    /// \~chinese ��ʼ�� IKapC ���л���				\~english Initialize IKapC runtime environment
    res = ItkManInitialize();
    CHECK(res);

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

    /// \~chinese �����Ϊ0���������������豸		\~english Open camera with 0 index and configure camera device
    int tmpIndex = -1;
    printf("total device count: %d, the index of  which you want to open is:", numDevices);
    scanf_s("%d", &tmpIndex);

    { int getcharTmp = getchar(); }

    ConfigureCamera(&cam, tmpIndex);

    if (cam.g_hBoard != INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Please select camera without grabber.\n");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }
    /// \~chinese �������������ʽ		            \~english Configure trigger method of the camera
    SetSoftTriggerWithoutGrabber(&cam);

    /// \~chinese �������chunkdata		            \~english Configure chunkdata of the camera
    SetChunkData(&cam,cam.benableChunkData);

    /// \~chinese �����������ͻ�����				    \~english Create data stream and buffer
    CreateStreamAndBuffer(&cam);

	/// \~chinese ע��ص�����                        \~english Register callback functions
    RegisterCallbackWithoutGrabber(&cam);

    /// \~chinese ��ʼͼ��ɼ�				        \~english Start grabbing images
    res = ItkStreamStart(cam.g_hStream, cam.g_grabCount);
    CHECK(res);

    if (cam.g_bSoftTriggerUsed) {
        printf("Please intput t to softTrigger a frame and input e to exit: ");
        while (true) {
            char T = 0;
            int scanTmp = scanf_s("%c", &T, (unsigned)sizeof(T));
            if (T == 't') {
                res = ItkDevExecuteCommand(cam.g_hCamera, "TriggerSoftware");
            } else if (T == 'e') {
                break;
            } else if (T != '\n') {
                printf("Please intput t to softTrigger a frame and input e to exit: ");
            }
        }
    }

    /// \~chinese �ȴ�ͼ��ɼ�����				    \~english Wait for grabbing images finished
    { int getcharTmp = getchar(); }

    /// \~chinese ֹͣͼ��ɼ�				    \~english Stop grabbing images
    ItkStreamStop(cam.g_hStream);

    /// \~chinese ����ص�����				    \~english Unregister callback functions
    UnRegisterCallbackWithoutGrabber(&cam);

    /// \~chinese �ͷ��������ͻ�����				\~english Free data stream and buffers
    ItkDevFreeStream(cam.g_hStream);

    /// \~chinese �ر�����豸				    \~english Close camera device
    res = ItkDevClose(cam.g_hCamera);
    CHECK(res);

	/// \~chinese �ͷ��û���������ڴ�Ż��������ݵ��ڴ�				    \~english Release the memory for storing the buffer data
    if (cam.g_bufferData != NULL)
        free(cam.g_bufferData);

	/// \~chinese �ͷ��û��������������Buffer��ַ���ڴ�				    \~english Release the memory that the user requests for setting the Buffer address
	/*if (cam.g_user_buffer!=NULL)
		free(cam.g_user_buffer);*/

    /// \~chinese �ͷ� IKapC ���л���				\~english Release IKapC runtime environment
    ItkManTerminate();

    pressEnterToExit();
    return EXIT_SUCCESS;
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
    ITKSTATUS  res          = ITKSTATUS_OK;
    
    ITKBUFFER  hBuffer      = NULL;
    pItkCamera cam          = (pItkCamera)pContext;
    ITK_BUFFER_INFO bufferInfo = {0};
    printf("On end of frame of camera with serialNumber:%s. \n", cam->g_devInfo.SerialNumber);

    // \~chinese ��ȡ��ǰbuffer     \~english get current buffer    
    res = ItkStreamGetCurrentBuffer(cam->g_hStream, &hBuffer);
    CHECK(res);

    // \~chinese ��ȡ��ǰbuffer��Ϣ     \~english get current buffer info 
    res = ItkBufferGetInfo(hBuffer, &bufferInfo);
    CHECK(res);

    /// \~chinese ��ͼ�񻺳���������ͼ�񻺳������������޷��ɼ�������һ֡ͼ��ʱ				\~english When buffer is full or  buffer is not full but cannot grab a complete frame of image
    if (bufferInfo.State == ITKBUFFER_VAL_STATE_FULL || bufferInfo.State == ITKBUFFER_VAL_STATE_UNCOMPLETED) {
        ITKBUFFER  splitOutBuffer = NULL;

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

        char strChunkPixelFormat[128] = {0};
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

        printf("OK\n");
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

