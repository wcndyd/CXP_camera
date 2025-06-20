// ReconnectDemo.cpp

/* @brief：本示例向用户演示如何掉线重连GigEVision或USB3Vision相机。出现帧完成回调打印后拔掉数据线等待相机掉线后再插上数据线。
 *
 * @brief：This example shows the user how to reconnect GigEVision or USB3Vision camera. After the end of frame is printed, unplug the data cable and wait for the camera to drop and then plug in the data cable. */

#include <Windows.h>
#include <fstream>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "IKapC.h"
#pragma comment(lib, "IKapC.lib")

// 图像缓冲区申请的帧数。
// 警告！如果图像缓冲区申请的帧数为1，可能会发生缓冲区溢出！
//
// The number of frames requested by buffer.
// WARNING! If STREAM_BUFFER_COUNT is set to 1, BufferOverFlow may happen!
#define STREAM_BUFFER_COUNT 10

// 相机设备句柄。
//
// Camera device handle.
ITKDEVICE g_hCamera = NULL;

// 数据流句柄。
//
// Data stream handle.
ITKSTREAM g_hStream = NULL;

// 是否开始取流
//
// start stream or not
bool bIsStartStream = false;

// 保存图像的文件名。
//
// File name of image.
const char *g_saveFileName = "C:\\CImage.tif";

// 缓冲区数据。
//
// Buffer data.
char *g_bufferData = NULL;

// 打开相机的信息。
//
// info of camera opened.
ITKDEV_INFO g_di = {0};

/* @brief：判断函数是否成功调用。
 * @param[in] errc：函数返回值。
 *
 * @brief：Determine whether the function is called successfully.
 * @param[in] errc：Function return value. */
#define CHECK(errc)                                                            \
  if (ITKSTATUS_OK != errc)                                                    \
  printErrorAndExit(errc)

/* @brief：用户输入回车键后退出程序。
 *
 * @brief：Users enter Enter to exit the program. */
void pressEnterToExit(void);

/* @brief：打印错误信息后退出程序。
 * @param[in] errc：函数返回值。
 *
 * @brief：Print error message and exit the program.
 * @param[in] errc：Function return value. */
void printErrorAndExit(ITKSTATUS errc);

/* @brief：配置相机设备。
 *
 * @brief：Configure camera device. */
void ConfigureCamera(int index=0);

/* @brief：创建数据流和缓冲区。
 *
 * @brief：Create data stream and buffer. */
void CreateStreamAndBuffer();

/* @brief：注册回调函数。
 *
 * @brief：Register callback functions. */
void RegisterCallback();

/* @brief：清除回调函数。
 *
 * @brief：Unregister callback functions. */
void UnRegisterCallback();

/* @brief：本函数被注册为一个回调函数。当数据流开始时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When data stream
 * starts, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbStartOfStream(uint32_t eventType, void *pContext);

/* @brief：本函数被注册为一个回调函数。当数据流结束时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When data stream
 * ends, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbEndOfStream(uint32_t eventType, void *pContext);

/* @brief：本函数被注册为一个回调函数。当一帧图像采集结束时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When grabbing a
 * frame of image finished, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbOnEndOfFrame(uint32_t eventType, void *pContext);

/* @brief：本函数被注册为一个回调函数。当图像采集超时时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When grabbing
 * images time out, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbOnTimeOut(uint32_t eventType, void *pContext);

/* @brief：本函数被注册为一个回调函数。当采集丢帧时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When grabbing
 * frame lost, the function will be called.
 * @param[in] eventType：Event type.
 * @param[in] pContext：Input parameter. */
void IKAPC_CC cbOnFrameLost(uint32_t eventType, void *pContext);

/* @brief：本函数被注册为一个回调函数。当相机掉线时，函数被调用。
 * @param[in] eventType：事件类型。
 * @param[in] pContext：输入参数。
 *
 * @brief：This function is registered as a callback function. When device
 * removed singal ignored, the function will be called.
 * @param[in] pContext：Input parameter.
 * @param[in] eventInfo：Output parameter. */
void IKAPC_CC cbOnReconnect(void *pContext, ITKEVENTINFO eventInfo);

int main(void) {
  // IKapC 函数返回值。
  //
  // Return value of IKapC functions.
  ITKSTATUS res = ITKSTATUS_OK;

  // 可用设备的数量。
  //
  // The number of available devices.
  uint32_t numDevices = 0;

  // 需要打开的设备的索引。
  //
  // The index of device to be opened.
  uint32_t devIndex = 0;

  printf("Itek Console Find Camera Example (C version)\n");

  // 初始化 IKapC 运行环境。
  //
  // Initialize IKapC runtime environment.
  res = ItkManInitialize();
  CHECK(res);

  // 枚举可用设备的数量。在打开设备前，必须调用 ItkManGetDeviceCount() 函数。
  //
  // Enumerate the number of available devices. Before opening the device,
  // ItkManGetDeviceCount() function must be called.
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

  // 打印连接的相机设备的信息。
  //
  // Print information of connected cameras.
  uint32_t i = 0;
  for (; i < numDevices; ++i) {
    // 相机信息结构体。
    //
    // Camera information structure.
    ITKDEV_INFO di;

    // 获取设备信息。
    //
    // Get device information.
    ItkManGetDeviceInfo(i, &di);

    // 打印设备信息
    //
    // Print information.
    printf("i:%d \n Device Full Name:%s\n Friendly Name:%s\n Vendor Name:%s\n "
           "Model Name:%s\n Serial Name:%s\n Device Class:%s\n "
           "Device Version:%s\n User Defined Name:%s\n\n",
           i, di.FullName, di.FriendlyName, di.VendorName, di.ModelName,
           di.SerialNumber, di.DeviceClass, di.DeviceVersion,
           di.UserDefinedName);
  }

  /// \~chinese 打开序号为0的相机并配置相机设备		\~english Open camera with 0 index and configure camera device
  int tmpIndex = -1;
  printf("total device count: %d, the index of  which you want to open is:", numDevices);
  scanf_s("%d", &tmpIndex);

  { int getcharTmp = getchar(); }

  // 配置相机设备。
  //
  // Configure camera device.
  ConfigureCamera(tmpIndex);

  // 创建数据流和缓冲区。
  //
  // Create data stream and buffer.
  CreateStreamAndBuffer();

  // 注册回调函数。
  //
  // Register callback functions.
  RegisterCallback();

  // 开始图像采集。
  //
  // Start grabbing images.
  res = ItkStreamStart(g_hStream, -1);
  CHECK(res);
  bIsStartStream = true;

  // 等待图像采集结束。
  //
  // Wait for grabbing images finished.
  { int getCharTmp = getchar(); }

  // 停止图像采集。
  //
  // Stop grabbing images.
  ItkStreamStop(g_hStream);

  // 清除回调函数。
  //
  // Unregister callback functions.
  UnRegisterCallback();

  ItkDevFreeStream(g_hStream);

  // 关闭相机设备。
  //
  // Close camera device.
  res = ItkDevClose(g_hCamera);
  CHECK(res);

  if (g_bufferData != NULL)
    free(g_bufferData);

  // 释放 IKapC 运行环境。
  //
  // Release IKapC runtime environment.
  ItkManTerminate();

  pressEnterToExit();
  return EXIT_SUCCESS;
}

// 打印错误信息后退出程序。
//
// Print error message and exit the program.
void printErrorAndExit(ITKSTATUS errc) {
  fprintf(stderr, "Error Code:%08x\n", errc);
  ItkManTerminate();
  pressEnterToExit();
  exit(EXIT_FAILURE);
}

// 用户输入回车键后退出程序。
//
// Users enter Enter to exit the program.
void pressEnterToExit(void) {
  fprintf(stderr, "\nPress enter to exit.\n");
  while (getchar() != '\n')
    ;
}

// 配置相机设备。
//
// Configure camera device.
void ConfigureCamera(int index) {
  ITKSTATUS res = ITKSTATUS_OK;
  uint32_t numCameras = 0;

  // 枚举可用相机的数量。在打开相机前，必须调用 ItkManGetDeviceCount() 函数。
  //
  // Enumerate the number of available cameras. Before opening the camera,
  // ItkManGetDeviceCount() function must be called.
  res = ItkManGetDeviceCount(&numCameras);
  CHECK(res);

  // 当没有连接的相机时。
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

  // 打开GigEVision或USB3Vision相机。
  //
  // Open GigEVision or USB3Vision camera.
  for (uint32_t i = 0; i < numCameras; i++) {
    if (i == index) {

      ITKDEV_INFO di;

      // 获取相机设备信息。
      //
      // Get camera device information.
      res = ItkManGetDeviceInfo(i, &di);
      printf("Using camera: serial: %s, name: %s, interface: %s.\n",
             di.SerialNumber, di.FullName, di.DeviceClass);

      // 当设备为GigEVision或USB3Vision相机且序列号正确时。
      //
      // When the device is GigEVision or USB3Vision camera and the serial
      // number is proper.
      if ((strcmp(di.DeviceClass, "GigEVision") == 0 ||
           strcmp(di.DeviceClass, "USB3Vision") == 0) &&
          strcmp(di.SerialNumber, "") != 0) {
        // 打开相机。
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

/// \~chinese 创建数据流和缓冲区				\~english Create data stream and
/// buffer
void CreateStreamAndBuffer() {
  ITKSTATUS res = ITKSTATUS_OK;

  /// \~chinese  数据流数量				     \~english The number of data
  /// stream
  uint32_t streamCount = 0;

  /// \~chinese  获取数据流数量			     \~english Get the number of data
  /// stream
  res = ItkDevGetStreamCount(g_hCamera, &streamCount);
  CHECK(res);

  if (streamCount == 0) {
    fprintf(stderr, "Camera does not have image stream channel.");
    ItkManTerminate();
    pressEnterToExit();
    exit(EXIT_FAILURE);
  }

  /// \~chinese  申请数据流资源		            \~english Allocate data stream
  /// source
  res = ItkDevAllocStreamEx(g_hCamera, 0, STREAM_BUFFER_COUNT, &g_hStream);
  CHECK(res);

  ITKBUFFER hBuffer = NULL;

  /// \~chinese  获取序号为0的buffer		            \~english get buffer of index 0
  res = ItkStreamGetBuffer(g_hStream, 0, &hBuffer);
  CHECK(res);

  /// \~chinese  获取buffer的信息		            \~english get buffer info
  ITK_BUFFER_INFO bufferInfo = { 0 };
  res = ItkBufferGetInfo(hBuffer, &bufferInfo);
  CHECK(res);

  /// \~chinese  创建缓冲区数据存储		       \~english Create buffer data
  /// saving
  g_bufferData = (char *)malloc(bufferInfo.ImageSize);
  if (g_bufferData == NULL) {
    pressEnterToExit();
    exit(EXIT_FAILURE);
  }
}

// 注册回调函数
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

// 清除回调函数。
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

// 本函数被注册为一个回调函数。当数据流开始时，函数被调用。
//
// This function is registered as a callback function. When data stream starts,
// the function will be called.
void IKAPC_CC cbStartOfStream(uint32_t eventType, void *pContext) {
  printf("On start of stream. \n");
}

// 本函数被注册为一个回调函数。当数据流结束时，函数被调用。
//
// This function is registered as a callback function. When data stream ends,
// the function will be called.
void IKAPC_CC cbEndOfStream(uint32_t eventType, void *pContext) {
  printf("On end of stream. \n");
}

// 本函数被注册为一个回调函数。当一帧图像采集结束时，函数被调用。
//
// This function is registered as a callback function. When grabbing a frame of
// image finished, the function will be called.
void IKAPC_CC cbOnEndOfFrame(uint32_t eventType, void *pContext) {

    printf("on end of frame.\n");

  ITKSTATUS res = ITKSTATUS_OK;
 
  ITKBUFFER hBuffer = NULL;
  ITK_BUFFER_INFO bufferInfo = {0};
  ITKSTREAM hStream = (ITKSTREAM)(pContext);
  
  // \~chinese 获取当前buffer     \~english get current buffer    
  res = ItkStreamGetCurrentBuffer(hStream, &hBuffer);
  CHECK(res);

  // \~chinese 获取当前buffer信息     \~english get current buffer info   
  res = ItkBufferGetInfo(hBuffer,&bufferInfo);
  CHECK(res);

  // 当图像缓冲区满或者图像缓冲区非满但是无法采集完整的一帧图像时。
  //
  // When buffer is full or buffer is not full but cannot grab a complete frame
  // of image.
  if (bufferInfo.State == ITKBUFFER_VAL_STATE_FULL ||
      bufferInfo.State == ITKBUFFER_VAL_STATE_UNCOMPLETED) {
    // 保存图像。
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

// 本函数被注册为一个回调函数。当图像采集超时时，函数被调用。
//
// This function is registered as a callback function. When grabbing images time
// out, the function will be called.
void IKAPC_CC cbOnTimeOut(uint32_t eventType, void *pContext) {
  printf("on time out.\n");
}

// 本函数被注册为一个回调函数。当采集丢帧时，函数被调用。
//
// This function is registered as a callback function. When grabbing frame lost,
// the function will be called.
void IKAPC_CC cbOnFrameLost(uint32_t eventType, void *pContext) {
  printf("on frame lost.\n");
}

// 本函数被注册为一个回调函数。当相机掉线时，函数被调用。
//
// This function is registered as a callback function. When device removed
// signal ignored, the function will be called.
void IKAPC_CC cbOnReconnect(void *pContext, ITKEVENTINFO eventInfo) {
  printf("on device removed.\n");

  ITKSTATUS res = ITKSTATUS_OK;

  if (bIsStartStream) {
    res = ItkStreamStop(g_hStream);

    // 清除回调函数。
    //
    // Unregister callback functions.
    UnRegisterCallback();

    ItkDevFreeStream(g_hStream);
  }
  // 关闭相机设备。
  //
  // Close camera device.
  res = ItkDevClose(g_hCamera);
  CHECK(res);

  if (g_bufferData != NULL)
    free(g_bufferData);

  uint32_t numCameras = 0;

  do {
    // 枚举可用相机的数量。在打开相机前，必须调用 ItkManGetDeviceCount()
    // 函数。
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

    // 打印相机信息。
    //
    // printf camera info.
    for (uint32_t i = 0; i < numCameras; i++) {

      ITKDEV_INFO di;

      // 获取相机设备信息。
      //
      // Get camera device information.
      res = ItkManGetDeviceInfo(i, &di);
      printf("index:%d camera: serial: %s, name: %s, interface: %s.\n", i,
             di.SerialNumber, di.FullName, di.DeviceClass);

      if (strcmp(di.SerialNumber, g_di.SerialNumber) == 0) {
        // 打开相机。
        //
        // Open camera.
        res = ItkDevOpen(i, ITKDEV_VAL_ACCESS_MODE_EXCLUSIVE, &g_hCamera);
        if (res != ITKSTATUS_OK) {
          printf("Open index:%d fail.\n", i);
          continue;
        }

        /// \~chinese  申请数据流资源		            \~english
        /// Allocate data stream source
        res =
            ItkDevAllocStreamEx(g_hCamera, 0, STREAM_BUFFER_COUNT, &g_hStream);
        CHECK(res);

        ITKBUFFER hBuffer = NULL;

        /// \~chinese  获取序号为0的buffer		            \~english get
        /// buffer of index 0
        res = ItkStreamGetBuffer(g_hStream, 0, &hBuffer);
        CHECK(res);

        /// \~chinese  获取buffer的信息		            \~english get buffer
        /// info
        ITK_BUFFER_INFO bufferInfo = {0};
        res = ItkBufferGetInfo(hBuffer, &bufferInfo);
        CHECK(res);

        /// \~chinese  创建缓冲区数据存储		       \~english Create
        /// buffer data saving
        g_bufferData = (char *)malloc(bufferInfo.ImageSize);
        if (g_bufferData == NULL) {
          pressEnterToExit();
          exit(EXIT_FAILURE);
        }

        // 注册回调函数。
        //
        // Register callback functions.
        RegisterCallback();

        // 开始图像采集。
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