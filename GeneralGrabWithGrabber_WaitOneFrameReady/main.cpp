// main.cpp

/* @brief����ʾ�����û���ʾ�������IKapC���IKapBoard�⣬ͨ��IKapWaitOneFrameReady�ӿںͲɼ������ӵ���������ɼ�ͼ��
 *
 * @brief��This example shows users how to use IKapC and IKapBoard libraries to
 * grab images continuously with grabber linked camera and IKapWaitOneFrameReady api. */

#include <malloc.h>
#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <process.h>

#include "../GeneralConfigureCamera/ConfigureCamera.h"
#include "../GeneralGrabWithGrabber/GeneralGrabWithGrabber.h"

#pragma comment(lib, "IKapBoard.lib")
#pragma comment(lib, "IKapC.lib")
#pragma comment(lib, "shlwapi.lib")

// ָʾ�Ƿ��˳�����
// 
// indicate whether exit  the program or not
bool g_bExit = false;

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

/* @brief���������ǹ����̵߳�ִ�к��������ڲɼ�ͼ��
 * @param[in] pUser�����������
 *
 * @brief��This function is the execution function of the work thread for image acquisition.
 * @param[in] pUser��Input parameter. */
unsigned int __stdcall WorkThread(void* pUser);

int main()
{
    /// \~chinese IKapC ��������ֵ				    \~english Return value of IKapC functions
    ITKSTATUS res = ITKSTATUS_OK;

    /// \~chinese IKapBoard ��������ֵ				\~english Return value of IKapBoard functions
    int ret = IK_RTN_OK;

    /// \~chinese ��ʼ�� IKapC ���л���				\~english Initialize IKapC runtime environment
    res = ItkManInitialize();
    CHECK_IKAPC(res);

    ItkCamera cam;

    cam.g_bufferCount = 10;

    sprintf_s(cam.g_saveFileName, "D:\Github\CXP_camera\CXP_camera\image_save_path");

    cam.g_SerialNumber = NULL;

    cam.g_bSoftTriggerUsed = 1;

    cam.g_bLoadGrabberConfig = 1;

    uint32_t numDevices = 0;
    res = ItkManGetDeviceCount(&numDevices);
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

    { int getCharTmp = getchar(); }

    ConfigureCamera(&cam, tmpIndex);

    if (cam.g_hBoard == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Please select camera with grabber.\n");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    /// \~chinese ���òɼ����豸				        \~english Configure frame grabber device
    ConfigureFrameGrabber(&cam);

    /// \~chinese ע��ص�����                        \~english Register callback functions.
    RegisterCallbackWithGrabber(&cam);

    /// \~chinese �������������ʽ		            \~english Configure trigger method of the camera
    SetSoftTriggerWithGrabber(&cam);

    /// \~chinese ��ʼͼ��ɼ�				        \~english Start grabbing images
    StartGrabImage(&cam);

    /// \~chinese ���������߳����ڻ�ȡͼ��              \~english Create a worker thread that is used to acquire the images
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
            int scanTmp = scanf_s("%c", &T, (unsigned)sizeof(T));
            if (T == 't') {
                if (strcmp(cam.g_devInfo.DeviceClass, "CoaXPress") == 0 ||
                    strcmp(cam.g_devInfo.DeviceClass, "CameraLink") == 0) {
                    ret = IKapSetInfo(cam.g_hBoard, IKP_SOFTWARE_TRIGGER_START, 1);
                }
                else {
                    res = ItkDevExecuteCommand(cam.g_hCamera, "TriggerSoftware");
                }
                CHECK_IKAPBOARD(ret);
            }
            else if (T == 'e') {
                break;
            }
            else if (T != '\n') {
                printf("Please intput t to softTrigger a frame and input e to exit: ");
            }
        }
    }

    /// \~chinese �ȴ�ͼ��ɼ�����				    \~english Wait for grabbing images finished
    { int getCharTmp = getchar(); }

    g_bExit = true;

    /// \~chinese ֹͣ�����߳�                    \~english stop workThread
    WaitForSingleObject(hThreadHandle, INFINITE);
    CloseHandle(hThreadHandle);

    /// \~chinese ֹͣͼ��ɼ�				        \~english Stop grabbing images
    ret = IKapStopGrab(cam.g_hBoard);
    CHECK_IKAPBOARD(ret);

    /// \~chinese ����ص�����				        \~english Unregister callback functions
    UnRegisterCallbackWithGrabber(&cam);

    /// \~chinese �رղɼ����豸				        \~english Close frame grabber device
    ret = IKapClose(cam.g_hBoard);
    CHECK_IKAPBOARD(ret);

    /// \~chinese �ر�����豸				        \~english Close camera device
    res = ItkDevClose(cam.g_hCamera);
    CHECK_IKAPC(res);

    /// \~chinese �ͷ��û��������������Buffer��ַ���ڴ�				    \~english Release the memory that the user requests for setting the Buffer address
    /*if (cam.g_user_buffer!=NULL)
    free(cam.g_user_buffer);*/

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

unsigned int __stdcall WorkThread(void* pUser)
{
    ITKSTATUS res = ITKSTATUS_OK;

    int ret = IK_RTN_OK;

    unsigned char* pUserBuffer = NULL;
    int              nFrameSize = 0;
    int              nFrameCount = 0;
    int              nFrameIndex = 0;
    IKAPBUFFERSTATUS status;

    pItkCamera cam = (pItkCamera)pUser;
    if (NULL == cam)
    {
        return 0;
    }
    do
    {
        /// \~chinese  timeout_ms ��Ҫ�������ʵ��֡���ڣ��˴���Ҫ����ʵ������޸�       \~english timeout_ms needs to be greater than the actual camera frame period, which needs to be modified according to the actual situation
        res = IKapWaitOneFrameReady(cam->g_hBoard, &nFrameIndex, 5000);
        if (g_bExit)
        {
            break;
        }
        CHECK_IKAPBOARD(ret);

        printf("Grab frame ready of camera with serialNumber:%s.\n",
               cam->g_devInfo.SerialNumber);

        ret = IKapGetInfo(cam->g_hBoard, IKP_FRAME_COUNT, &nFrameCount);
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
          ret = IKapGetBufferAddress(cam->g_hBoard, nFrameIndex,
                                     (void **)&pUserBuffer);
          CHECK_IKAPBOARD(ret);

          /// \~chinese ����ͼ��				       \~english Save image
          
          ret=IKapSaveBuffer(cam->g_hBoard,nFrameIndex,cam->g_saveFileName,IKP_DEFAULT_COMPRESSION);
          CHECK_IKAPBOARD(ret);
          
        }

        /// \~chinese �ͷ�buffer				       \~english release buffer
        ret = IKapReleaseBuffer(cam->g_hBoard,nFrameIndex);
        CHECK_IKAPBOARD(ret);

    } while (!g_bExit);
    return 0;
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
