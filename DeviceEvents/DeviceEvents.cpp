// DeviceEvents.cpp

/* @brief：本示例向用户演示如何注册相机FrameTrigger事件、FrameTriggerIgnored事件、LineTriggerIgnored事件、MessageChannel事件。
 * @note：仅有部分相机支持FrameTrigger事件、FrameTriggerIgnored事件、LineTriggerIgnored事件、MessageChannel事件，请查阅相机说明书确保相机支持该功能，MessageChannel事件首先需要在事件选择器处选择需要监控的事件，并将事件通知模式打开。
 *
 * @brief：This example shows the user how to register the camera FrameTrigger event, FrameTriggerIgnored event, LineTriggerIgnored event, MessageChannel event.
 * @note：Only some cameras support FrameTrigger, FrameTriggerIgnored, LineTriggerIgnored, and MessageChannel events. Please consult the camera instructions to ensure that the camera supports this function, For MessageChannel Events, please select the event to be monitored at the event selector and turn the event notification mode on.
 */

#include <Windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "IKapC.h"
#pragma comment(lib, "IKapC.lib")

/* @brief：判断函数是否成功调用。
 * @param[in] errc：函数返回值。
 *
 * @brief：Determine whether the function is called successfully.
 * @param[in] errc：Function return value. */
#define CHECK(errc)                                                                                                         \
    if (ITKSTATUS_OK != errc)                                                                                               \
    printErrorAndExit(errc)

// 相机设备句柄。
//
// Camera device handle.
ITKDEVICE g_hCamera;

/* @brief：用户输入回车键后退出程序。
 *
 *
 * @brief：Users enter Enter to exit the program. */
void pressEnterToExit(void);

/* @brief：打印错误信息后退出程序。
 * @param[in] errc：函数返回值。
 *
 * @brief：Print error message and exit the program.
 * @param[in] errc：Function return value. */
void printErrorAndExit(ITKSTATUS errc);

/* @brief：注册设备事件回调。
 * @param[in] hDev：设备句柄。
 *
 * @brief：register device events callback.
 * @param[in] hDev：device handle. */
void RegisterDeviceCallback(ITKDEVICE hDev);

/* @brief：取消注册设备事件回调。
 * @param[in] hDev：设备句柄。
 *
 * @brief：unregister device events callback.
 * @param[in] hDev：device handle. */
void UnregisterDeviceCallback(ITKDEVICE hDev);

/* @brief：本函数被注册为一个回调函数。当FrameTrigger特征值改变时，函数被调用。
 * @param[in] context：输入参数。
 * @param[in] eventInfo：事件信息句柄。
 *
 * @brief：This function is registered as a callback function. When FrameTrigger feature value is changed, the function will be called.
 * @param[in] context：Input parameter.
 * @param[in] eventInfo：Event information handle. */
void __stdcall FrameTriggerCallback(void* context, ITKEVENTINFO eventInfo);

/* @brief：本函数被注册为一个回调函数。当FrameTriggerIgnored特征值改变时，函数被调用。
 * @param[in] context：输入参数。
 * @param[in] eventInfo：事件信息句柄。
 *
 * @brief：This function is registered as a callback function. When FrameTriggerIgnored feature value is changed, the function will be called.
 * @param[in] context：Input parameter.
 * @param[in] eventInfo：Event information handle. */
void __stdcall FrameTriggerIgnoredCallback(void* context, ITKEVENTINFO eventInfo);

/* @brief：本函数被注册为一个回调函数。当LineTriggerIgnored特征值改变时，函数被调用。
 * @param[in] context：输入参数。
 * @param[in] eventInfo：事件信息句柄。
 *
 * @brief：This function is registered as a callback function. When LineTriggerIgnored feature value is changed, the function will be called.
 * @param[in] context：Input parameter.
 * @param[in] eventInfo：Event information handle. */
void __stdcall LineTriggerIgnoredCallback(void* context, ITKEVENTINFO eventInfo);

/* @brief：本函数被注册为一个回调函数。当MessageChannel事件被触发时，函数被调用。
 * @param[in] context：输入参数。
 * @param[in] eventInfo：事件信息句柄。
 *
 * @brief：This function is registered as a callback function. When message channel event is triggered, the function will be called.
 * @param[in] context：Input parameter.
 * @param[in] eventInfo：Event information handle. */
void __stdcall MessageChannelCallback(void* context, ITKEVENTINFO eventInfo);

int main(void)
{
    // IKapC 函数返回值。
    //
    // Return value of IKapC functions.
    ITKSTATUS res;

    // 可用设备的数量。
    //
    // The number of available devices.
    uint32_t numDevices = 0;

    // 需要打开的设备的索引。
    //
    // The index of device to be opened.
    uint32_t devIndex = 0;

    printf("Itek Console Camera Features Example(C version)\n");

    // 初始化 IKapC 运行环境。
    //
    // Initialize IKapC runtime environment.
    res = ItkManInitialize();
    CHECK(res);

    // 枚举可用设备的数量。在打开设备前，必须调用 ItkManGetDeviceCount() 函数。
    //
    // Enumerate the number of available devices. Before opening the device, ItkManGetDeviceCount() function must be called.
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
    printf("total device count: %d, the index of  which you want to open is:", numDevices);
    int tmp = scanf_s("%d", &devIndex);

    if (devIndex >= numDevices)
    {
        fprintf(stderr, "index: %d is more than %d.\n", devIndex, numDevices - 1);
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    // 打开设备，获取设备句柄。
    //
    // Open the device and get the handle of the device.
    res = ItkDevOpen(devIndex, ITKDEV_VAL_ACCESS_MODE_CONTROL, &g_hCamera);
    CHECK(res);

    // 注册相机FrameTrigger、FrameTriggerIgnored、LineTriggerIgnored、MessageChannel事件。
    //
    // register FrameTrigger、FrameTriggerIgnored、LineTriggerIgnored、MessageChannel events.
    RegisterDeviceCallback(g_hCamera);

    { int getCharTmp = getchar(); }

    // 取消注册相机FrameTrigger、FrameTriggerIgnored、LineTriggerIgnored、MessageChannel事件。
    //
    // unregister FrameTrigger、FrameTriggerIgnored、LineTriggerIgnored、MessageChannel events.
    UnregisterDeviceCallback(g_hCamera);


    //关闭设备。
    //
    // Close the device.
    res = ItkDevClose(g_hCamera);
    CHECK(res);

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
void printErrorAndExit(ITKSTATUS errc)
{
    fprintf(stderr, "Error Code:%08X\n", errc);
    ItkManTerminate();
    pressEnterToExit();
    exit(EXIT_FAILURE);
}

void RegisterDeviceCallback(ITKDEVICE hDev)
{
    ITKSTATUS res = ITKSTATUS_OK;
    // 注册FrameTrigger事件。
    //
    // register FrameTrigger event.
    res = ItkDevRegisterCallback(g_hCamera, "FrameTrigger", FrameTriggerCallback, g_hCamera);
    CHECK(res);

    // 注册FrameTriggerIgnored事件。
    //
    // register FrameTriggerIgnored event.
    res = ItkDevRegisterCallback(g_hCamera, "FrameTriggerIgnored", FrameTriggerIgnoredCallback, g_hCamera);
    CHECK(res);

    // 注册LineTriggerIgnored事件。
    //
    // register LineTriggerIgnored event.
    res = ItkDevRegisterCallback(g_hCamera, "LineTriggerIgnored", LineTriggerIgnoredCallback, g_hCamera);
    CHECK(res);

    // 注册MessageChannel事件。
    //
    // register MessageChannel event.
    res = ItkDevRegisterCallback(g_hCamera, "MessageChannel", MessageChannelCallback, g_hCamera);
    CHECK(res);
}

void UnregisterDeviceCallback(ITKDEVICE hDev)
{
    ITKSTATUS res = ITKSTATUS_OK;
    // 取消注册FrameTrigger事件。
    //
    // unregister FrameTrigger event.
    res = ItkDevUnregisterCallback(g_hCamera, "FrameTrigger");
    CHECK(res);

    // 取消注册FrameTriggerIgnored事件。
    //
    // unregister FrameTriggerIgored event.
    res = ItkDevUnregisterCallback(g_hCamera, "FrameTriggerIgnored");
    CHECK(res);

    // 取消注册LineTriggerIgnored事件。
    //
    // unregister LineTriggerIgnored event.
    res = ItkDevUnregisterCallback(g_hCamera, "LineTriggerIgnored");
    CHECK(res);

    // 取消注册MessageChannel事件。
    //
    // unregister MessageChannel event.
    res = ItkDevUnregisterCallback(g_hCamera, "MessageChannel");
    CHECK(res);
}

void __stdcall FrameTriggerCallback(void* context, ITKEVENTINFO eventInfo)
{
    uint32_t  type = 0;
    uint64_t  countstamp = 0;
    ITKDEVICE hDev = (ITKDEVICE)context;

    // 获取事件类型参数值。
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // 获取事件发生时间戳参数值。
    //
    // Get the event time stamp parameter value.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_HOST_TIME_STAMP, &countstamp);
    CHECK(res);

// 判断类型位数并打印信息。
//
// Determine the type bit and print information.
#if __STDC_VERSION__ >= 199901L
    printf("FrameTriggerCallback, type: %08X, time: %lld.\n", type, countstamp);
#else
    printf("\nFrameTriggerCallback, type: %08X, time: %I64d.\n", type, countstamp);
#endif
}

void __stdcall FrameTriggerIgnoredCallback(void* context, ITKEVENTINFO eventInfo)
{
    uint32_t  type = 0;
    uint64_t  countstamp = 0;
    ITKDEVICE hDev = (ITKDEVICE)context;

    // 获取事件类型参数值。
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // 获取事件发生时间戳参数值。
    //
    // Get the event time stamp parameter value.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_HOST_TIME_STAMP, &countstamp);
    CHECK(res);

// 判断类型位数并打印信息。
//
// Determine the type bit and print information.
#if __STDC_VERSION__ >= 199901L
    printf("FrameTriggerIgnoredCallback, type: %08X, time: %lld.\n", type, countstamp);
#else
    printf("\nFrameTriggerIgnoredCallback, type: %08X, time: %I64d.\n", type, countstamp);
#endif
}

void __stdcall LineTriggerIgnoredCallback(void* context, ITKEVENTINFO eventInfo)
{
    uint32_t  type = 0;
    uint64_t  countstamp = 0;
    ITKDEVICE hDev = (ITKDEVICE)context;

    // 获取事件类型参数值。
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // 获取事件发生时间戳参数值。
    //
    // Get the event time stamp parameter value.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_HOST_TIME_STAMP, &countstamp);
    CHECK(res);

// 判断类型位数并打印信息。
//
// Determine the type bit and print information.
#if __STDC_VERSION__ >= 199901L
    printf("LineTriggerIgnoredCallback, type: %08X, time: %lld.\n", type, countstamp);
#else
    printf("\nLineTriggerIgnoredCallback, type: %08X, time: %I64d.\n", type, countstamp);
#endif
}

void __stdcall MessageChannelCallback(void* context, ITKEVENTINFO eventInfo)
{
    uint32_t  type = 0, feature_name_length = 128;
    uint64_t  countstamp = 0;
    char      feature_name[128] = { 0 };
    ITKDEVICE hDev = (ITKDEVICE)context;

    // 获取事件类型参数值。
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // 获取事件名称参数值，名称是已打开通知模式的事件名称之一。
    //
    // Get the event name parameter value, The name is one of the event names with an open notification mode.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_FEATURE_NAME, feature_name);
    CHECK(res);
    int nType = (int)strlen(feature_name);

    // 获取事件发生时间戳参数值。
    //
    // Get the event time stamp parameter value.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_HOST_TIME_STAMP, &countstamp);
    CHECK(res);

// 判断类型位数并打印信息。
//
// Determine the type bit and print information.
#if __STDC_VERSION__ >= 199901L
    printf("OnMessageChannelCallback, feature: %s, type: %0d, time: %lld.\n", feature_name, nType, countstamp);
#else
    printf("OnMessageChannelCallback, feature: %s, type: %0d, time: %I64d.\n", feature_name, nType, countstamp);
#endif
}

// 用户输入回车键后退出程序。
//
// Users enter Enter to exit the program.
void pressEnterToExit(void)
{
    fprintf(stderr, "\nPress enter to exit.\n");
    while (getchar() != '\n')
        ;
}
