// ReconnectDemo.cpp

/* @brief����ʾ�����û���ʾ��ε�������GigEVision��USB3Vision���������֡��ɻص���ӡ��ε������ߵȴ�������ߺ��ٲ��������ߡ�
 *
 * @brief��This example shows the user how to reconnect GigEVision or USB3Vision camera. After the end of frame is printed, unplug the data cable and wait for the camera to drop and then plug in the data cable. */

#include <Windows.h>
#include <fstream>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "IKapC.h"
#pragma comment(lib, "IKapC.lib")

// ͼ�񻺳��������֡����
// ���棡���ͼ�񻺳��������֡��Ϊ1�����ܻᷢ�������������
//
// The number of frames requested by buffer.
// WARNING! If STREAM_BUFFER_COUNT is set to 1, BufferOverFlow may happen!
#define STREAM_BUFFER_COUNT 10

// ����豸�����
//
// Camera device handle.
ITKDEVICE g_hCamera = NULL;

// �����������
//
// Data stream handle.
ITKSTREAM g_hStream = NULL;

// �Ƿ�ʼȡ��
//
// start stream or not
bool bIsStartStream = false;

// ����ͼ����ļ�����
//
// File name of image.
const char *g_saveFileName = "C:\\CImage.tif";

// ���������ݡ�
//
// Buffer data.
char *g_bufferData = NULL;

// ���������Ϣ��
//
// info of camera opened.
ITKDEV_INFO g_di = {0};

/* @brief���жϺ����Ƿ�ɹ����á�
 * @param[in] errc����������ֵ��
 *
 * @brief��Determine whether the function is called successfully.
 * @param[in] errc��Function return value. */
#define CHECK(errc)                                                            \
  if (ITKSTATUS_OK != errc)                                                    \
  printErrorAndExit(errc)

/* @brief���û�����س������˳�����
 *
 * @brief��Users enter Enter to exit the program. */
void pressEnterToExit(void);

/* @brief����ӡ������Ϣ���˳�����
 * @param[in] errc����������ֵ��
 *
 * @brief��Print error message and exit the program.
 * @param[in] errc��Function return value. */
void printErrorAndExit(ITKSTATUS errc);

/* @brief����������豸��
 *
 * @brief��Configure camera device. */
void ConfigureCamera(int index=0);

/* @brief�������������ͻ�������
 *
 * @brief��Create data stream and buffer. */
void CreateStreamAndBuffer();

/* @brief��ע��ص�������
 *
 * @brief��Register callback functions. */
void RegisterCallback();

/* @brief������ص�������
 *
 * @brief��Unregister callback functions. */
void UnRegisterCallback();

/* @brief����������ע��Ϊһ���ص�����������������ʼʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When data stream
 * starts, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbStartOfStream(uint32_t eventType, void *pContext);

/* @brief����������ע��Ϊһ���ص�������������������ʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When data stream
 * ends, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbEndOfStream(uint32_t eventType, void *pContext);

/* @brief����������ע��Ϊһ���ص���������һ֡ͼ��ɼ�����ʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When grabbing a
 * frame of image finished, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbOnEndOfFrame(uint32_t eventType, void *pContext);

/* @brief����������ע��Ϊһ���ص���������ͼ��ɼ���ʱʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When grabbing
 * images time out, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbOnTimeOut(uint32_t eventType, void *pContext);

/* @brief����������ע��Ϊһ���ص����������ɼ���֡ʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When grabbing
 * frame lost, the function will be called.
 * @param[in] eventType��Event type.
 * @param[in] pContext��Input parameter. */
void IKAPC_CC cbOnFrameLost(uint32_t eventType, void *pContext);

/* @brief����������ע��Ϊһ���ص����������������ʱ�����������á�
 * @param[in] eventType���¼����͡�
 * @param[in] pContext�����������
 *
 * @brief��This function is registered as a callback function. When device
 * removed singal ignored, the function will be called.
 * @param[in] pContext��Input parameter.
 * @param[in] eventInfo��Output parameter. */
void IKAPC_CC cbOnReconnect(void *pContext, ITKEVENTINFO eventInfo);

int main(void) {
  // IKapC ��������ֵ��
  //
  // Return value of IKapC functions.
  ITKSTATUS res = ITKSTATUS_OK;

  // �����豸��������
  //
  // The number of available devices.
  uint32_t numDevices = 0;

  // ��Ҫ�򿪵��豸��������
  //
  // The index of device to be opened.
  uint32_t devIndex = 0;

  printf("Itek Console Find Camera Example (C version)\n");

  // ��ʼ�� IKapC ���л�����
  //
  // Initialize IKapC runtime environment.
  res = ItkManInitialize();
  CHECK(res);

  // ö�ٿ����豸���������ڴ��豸ǰ��������� ItkManGetDeviceCount() ������
  //
  // Enumerate the number of available devices. Before opening the device,
  // ItkManGetDeviceCount() function must be called.
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

  // ��ӡ���ӵ�����豸����Ϣ��
  //
  // Print information of connected cameras.
  uint32_t i = 0;
  for (; i < numDevices; ++i) {
    // �����Ϣ�ṹ�塣
    //
    // Camera information structure.
    ITKDEV_INFO di;

    // ��ȡ�豸��Ϣ��
    //
    // Get device information.
    ItkManGetDeviceInfo(i, &di);

    // ��ӡ�豸��Ϣ
    //
    // Print information.
    printf("i:%d \n Device Full Name:%s\n Friendly Name:%s\n Vendor Name:%s\n "
           "Model Name:%s\n Serial Name:%s\n Device Class:%s\n "
           "Device Version:%s\n User Defined Name:%s\n\n",
           i, di.FullName, di.FriendlyName, di.VendorName, di.ModelName,
           di.SerialNumber, di.DeviceClass, di.DeviceVersion,
           di.UserDefinedName);
  }

  /// \~chinese �����Ϊ0���������������豸		\~english Open camera with 0 index and configure camera device
  int tmpIndex = -1;
  printf("total device count: %d, the index of  which you want to open is:", numDevices);
  scanf_s("%d", &tmpIndex);

  { int getcharTmp = getchar(); }

  // ��������豸��
  //
  // Configure camera device.
  ConfigureCamera(tmpIndex);

  // �����������ͻ�������
  //
  // Create data stream and buffer.
  CreateStreamAndBuffer();

  // ע��ص�������
  //
  // Register callback functions.
  RegisterCallback();

  // ��ʼͼ��ɼ���
  //
  // Start grabbing images.
  res = ItkStreamStart(g_hStream, -1);
  CHECK(res);
  bIsStartStream = true;

  // �ȴ�ͼ��ɼ�������
  //
  // Wait for grabbing images finished.
  { int getCharTmp = getchar(); }

  // ֹͣͼ��ɼ���
  //
  // Stop grabbing images.
  ItkStreamStop(g_hStream);

  // ����ص�������
  //
  // Unregister callback functions.
  UnRegisterCallback();

  ItkDevFreeStream(g_hStream);

  // �ر�����豸��
  //
  // Close camera device.
  res = ItkDevClose(g_hCamera);
  CHECK(res);

  if (g_bufferData != NULL)
    free(g_bufferData);

  // �ͷ� IKapC ���л�����
  //
  // Release IKapC runtime environment.
  ItkManTerminate();

  pressEnterToExit();
  return EXIT_SUCCESS;
}

// ��ӡ������Ϣ���˳�����
//
// Print error message and exit the program.
void printErrorAndExit(ITKSTATUS errc) {
  fprintf(stderr, "Error Code:%08x\n", errc);
  ItkManTerminate();
  pressEnterToExit();
  exit(EXIT_FAILURE);
}

// �û�����س������˳�����
//
// Users enter Enter to exit the program.
void pressEnterToExit(void) {
  fprintf(stderr, "\nPress enter to exit.\n");
  while (getchar() != '\n')
    ;
}

// ��������豸��
//
// Configure camera device.
void ConfigureCamera(int index) {
  ITKSTATUS res = ITKSTATUS_OK;
  uint32_t numCameras = 0;

  // ö�ٿ���������������ڴ����ǰ��������� ItkManGetDeviceCount() ������
  //
  // Enumerate the number of available cameras. Before opening the camera,
  // ItkManGetDeviceCount() function must be called.
  res = ItkManGetDeviceCount(&numCameras);
  CHECK(res);

  // ��û�����ӵ����ʱ��
  //
  // When there is no connected cameras.
  if (numCameras == 0) {
    printf("No camera.\n");
    ItkManTerminate();
    pressEnterToExit();
    exit(EXIT_FAILURE);
  }

  if (index>= numCameras)
  {
      printf("index:%d is more than numCameras:%d.\n",index,numCameras);
      ItkManTerminate();
      pressEnterToExit();
      exit(EXIT_FAILURE);
  }

  // ��GigEVision��USB3Vision�����
  //
  // Open GigEVision or USB3Vision camera.
  for (uint32_t i = 0; i < numCameras; i++) {
    if (i == index) {

      ITKDEV_INFO di;

      // ��ȡ����豸��Ϣ��
      //
      // Get camera device information.
      res = ItkManGetDeviceInfo(i, &di);
      printf("Using camera: serial: %s, name: %s, interface: %s.\n",
             di.SerialNumber, di.FullName, di.DeviceClass);

      // ���豸ΪGigEVision��USB3Vision��������к���ȷʱ��
      //
      // When the device is GigEVision or USB3Vision camera and the serial
      // number is proper.
      if ((strcmp(di.DeviceClass, "GigEVision") == 0 ||
           strcmp(di.DeviceClass, "USB3Vision") == 0) &&
          strcmp(di.SerialNumber, "") != 0) {
        // �������
        //
        // Open camera.
        res = ItkDevOpen(i, ITKDEV_VAL_ACCESS_MODE_EXCLUSIVE, &g_hCamera);
        CHECK(res);
        g_di = di;
        break;
      }
    }
  }

}

/// \~chinese �����������ͻ�����				\~english Create data stream and
/// buffer
void CreateStreamAndBuffer() {
  ITKSTATUS res = ITKSTATUS_OK;

  /// \~chinese  ����������				     \~english The number of data
  /// stream
  uint32_t streamCount = 0;

  /// \~chinese  ��ȡ����������			     \~english Get the number of data
  /// stream
  res = ItkDevGetStreamCount(g_hCamera, &streamCount);
  CHECK(res);

  if (streamCount == 0) {
    fprintf(stderr, "Camera does not have image stream channel.");
    ItkManTerminate();
    pressEnterToExit();
    exit(EXIT_FAILURE);
  }

  /// \~chinese  ������������Դ		            \~english Allocate data stream
  /// source
  res = ItkDevAllocStreamEx(g_hCamera, 0, STREAM_BUFFER_COUNT, &g_hStream);
  CHECK(res);

  ITKBUFFER hBuffer = NULL;

  /// \~chinese  ��ȡ���Ϊ0��buffer		            \~english get buffer of index 0
  res = ItkStreamGetBuffer(g_hStream, 0, &hBuffer);
  CHECK(res);

  /// \~chinese  ��ȡbuffer����Ϣ		            \~english get buffer info
  ITK_BUFFER_INFO bufferInfo = { 0 };
  res = ItkBufferGetInfo(hBuffer, &bufferInfo);
  CHECK(res);

  /// \~chinese  �������������ݴ洢		       \~english Create buffer data
  /// saving
  g_bufferData = (char *)malloc(bufferInfo.ImageSize);
  if (g_bufferData == NULL) {
    pressEnterToExit();
    exit(EXIT_FAILURE);
  }
}

// ע��ص�����
//
// Register callback functions.
void RegisterCallback() {
  ITKSTATUS res = ITKSTATUS_OK;

  res = ItkStreamRegisterCallback(g_hStream,
                                  ITKSTREAM_VAL_EVENT_TYPE_START_OF_STREAM,
                                  cbStartOfStream, g_hStream);
  CHECK(res);
  res = ItkStreamRegisterCallback(g_hStream,
                                  ITKSTREAM_VAL_EVENT_TYPE_END_OF_STREAM,
                                  cbEndOfStream, g_hStream);
  CHECK(res);
  res = ItkStreamRegisterCallback(g_hStream,
                                  ITKSTREAM_VAL_EVENT_TYPE_END_OF_FRAME,
                                  cbOnEndOfFrame, g_hStream);
  CHECK(res);
  CHECK(res);
  res = ItkStreamRegisterCallback(g_hStream, ITKSTREAM_VAL_EVENT_TYPE_TIME_OUT,
                                  cbOnTimeOut, g_hStream);
  CHECK(res);
  res = ItkStreamRegisterCallback(
      g_hStream, ITKSTREAM_VAL_EVENT_TYPE_FRAME_LOST, cbOnFrameLost, g_hStream);
  CHECK(res);
  res = ItkDevRegisterCallback(g_hCamera, "DeviceRemove", cbOnReconnect,
                               g_hCamera);
  CHECK(res);
}

// ����ص�������
//
// Unregister callback functions.
void UnRegisterCallback() {
  ITKSTATUS res = ITKSTATUS_OK;

  res = ItkStreamUnregisterCallback(g_hStream,
                                    ITKSTREAM_VAL_EVENT_TYPE_START_OF_STREAM);
  res = ItkStreamUnregisterCallback(g_hStream,
                                    ITKSTREAM_VAL_EVENT_TYPE_END_OF_STREAM);
  res = ItkStreamUnregisterCallback(g_hStream,
                                    ITKSTREAM_VAL_EVENT_TYPE_END_OF_FRAME);
  res =
      ItkStreamUnregisterCallback(g_hStream, ITKSTREAM_VAL_EVENT_TYPE_TIME_OUT);
  res = ItkStreamUnregisterCallback(g_hStream,
                                    ITKSTREAM_VAL_EVENT_TYPE_FRAME_LOST);
  res = ItkDevUnregisterCallback(g_hCamera, "DeviceRemove");
}

// ��������ע��Ϊһ���ص�����������������ʼʱ�����������á�
//
// This function is registered as a callback function. When data stream starts,
// the function will be called.
void IKAPC_CC cbStartOfStream(uint32_t eventType, void *pContext) {
  printf("On start of stream. \n");
}

// ��������ע��Ϊһ���ص�������������������ʱ�����������á�
//
// This function is registered as a callback function. When data stream ends,
// the function will be called.
void IKAPC_CC cbEndOfStream(uint32_t eventType, void *pContext) {
  printf("On end of stream. \n");
}

// ��������ע��Ϊһ���ص���������һ֡ͼ��ɼ�����ʱ�����������á�
//
// This function is registered as a callback function. When grabbing a frame of
// image finished, the function will be called.
void IKAPC_CC cbOnEndOfFrame(uint32_t eventType, void *pContext) {

    printf("on end of frame.\n");

  ITKSTATUS res = ITKSTATUS_OK;
 
  ITKBUFFER hBuffer = NULL;
  ITK_BUFFER_INFO bufferInfo = {0};
  ITKSTREAM hStream = (ITKSTREAM)(pContext);
  
  // \~chinese ��ȡ��ǰbuffer     \~english get current buffer    
  res = ItkStreamGetCurrentBuffer(hStream, &hBuffer);
  CHECK(res);

  // \~chinese ��ȡ��ǰbuffer��Ϣ     \~english get current buffer info   
  res = ItkBufferGetInfo(hBuffer,&bufferInfo);
  CHECK(res);

  // ��ͼ�񻺳���������ͼ�񻺳������������޷��ɼ�������һ֡ͼ��ʱ��
  //
  // When buffer is full or buffer is not full but cannot grab a complete frame
  // of image.
  if (bufferInfo.State == ITKBUFFER_VAL_STATE_FULL ||
      bufferInfo.State == ITKBUFFER_VAL_STATE_UNCOMPLETED) {
    // ����ͼ��
    //
    // Save image.

    /*res = ItkBufferSave(hBuffer,g_saveFileName,ITKBUFFER_VAL_TIFF);
    CHECK(res);
    res = ItkBufferSave(hBuffer, g_saveFileNameraw, ITKBUFFER_VAL_RAW);
    CHECK(res);*/

    res = ItkBufferRead(hBuffer, 0, g_bufferData, (uint32_t)bufferInfo.ValidImageSize);
    CHECK(res);
  }
}

// ��������ע��Ϊһ���ص���������ͼ��ɼ���ʱʱ�����������á�
//
// This function is registered as a callback function. When grabbing images time
// out, the function will be called.
void IKAPC_CC cbOnTimeOut(uint32_t eventType, void *pContext) {
  printf("on time out.\n");
}

// ��������ע��Ϊһ���ص����������ɼ���֡ʱ�����������á�
//
// This function is registered as a callback function. When grabbing frame lost,
// the function will be called.
void IKAPC_CC cbOnFrameLost(uint32_t eventType, void *pContext) {
  printf("on frame lost.\n");
}

// ��������ע��Ϊһ���ص����������������ʱ�����������á�
//
// This function is registered as a callback function. When device removed
// signal ignored, the function will be called.
void IKAPC_CC cbOnReconnect(void *pContext, ITKEVENTINFO eventInfo) {
  printf("on device removed.\n");

  ITKSTATUS res = ITKSTATUS_OK;

  if (bIsStartStream) {
    res = ItkStreamStop(g_hStream);

    // ����ص�������
    //
    // Unregister callback functions.
    UnRegisterCallback();

    ItkDevFreeStream(g_hStream);
  }
  // �ر�����豸��
  //
  // Close camera device.
  res = ItkDevClose(g_hCamera);
  CHECK(res);

  if (g_bufferData != NULL)
    free(g_bufferData);

  uint32_t numCameras = 0;

  do {
    // ö�ٿ���������������ڴ����ǰ��������� ItkManGetDeviceCount()
    // ������
    //
    // Enumerate the number of available cameras. Before opening the camera,
    // ItkManGetDeviceCount() function must be called.
    res = ItkManGetDeviceCount(&numCameras);
    CHECK(res);

    if (numCameras == 0) {
      printf("numCameras:%d\n", numCameras);
      Sleep(200);
      continue;
    }

    // ��ӡ�����Ϣ��
    //
    // printf camera info.
    for (uint32_t i = 0; i < numCameras; i++) {

      ITKDEV_INFO di;

      // ��ȡ����豸��Ϣ��
      //
      // Get camera device information.
      res = ItkManGetDeviceInfo(i, &di);
      printf("index:%d camera: serial: %s, name: %s, interface: %s.\n", i,
             di.SerialNumber, di.FullName, di.DeviceClass);

      if (strcmp(di.SerialNumber, g_di.SerialNumber) == 0) {
        // �������
        //
        // Open camera.
        res = ItkDevOpen(i, ITKDEV_VAL_ACCESS_MODE_EXCLUSIVE, &g_hCamera);
        if (res != ITKSTATUS_OK) {
          printf("Open index:%d fail.\n", i);
          continue;
        }

        /// \~chinese  ������������Դ		            \~english
        /// Allocate data stream source
        res =
            ItkDevAllocStreamEx(g_hCamera, 0, STREAM_BUFFER_COUNT, &g_hStream);
        CHECK(res);

        ITKBUFFER hBuffer = NULL;

        /// \~chinese  ��ȡ���Ϊ0��buffer		            \~english get
        /// buffer of index 0
        res = ItkStreamGetBuffer(g_hStream, 0, &hBuffer);
        CHECK(res);

        /// \~chinese  ��ȡbuffer����Ϣ		            \~english get buffer
        /// info
        ITK_BUFFER_INFO bufferInfo = {0};
        res = ItkBufferGetInfo(hBuffer, &bufferInfo);
        CHECK(res);

        /// \~chinese  �������������ݴ洢		       \~english Create
        /// buffer data saving
        g_bufferData = (char *)malloc(bufferInfo.ImageSize);
        if (g_bufferData == NULL) {
          pressEnterToExit();
          exit(EXIT_FAILURE);
        }

        // ע��ص�������
        //
        // Register callback functions.
        RegisterCallback();

        // ��ʼͼ��ɼ���
        //
        // Start grabbing images.
        res = ItkStreamStart(g_hStream, -1);
        CHECK(res);
        bIsStartStream = true;
        printf("reopen success.\n");
        return;
      }
    }
    Sleep(200);
  } while (true);
}