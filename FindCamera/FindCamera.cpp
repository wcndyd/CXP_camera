// FindCamera.cpp

/* @brief：本示例向用户演示如何搜索可用相机设备。
 *
 * @brief：This example shows users how to find available cameras. */

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

int main(void)
{
    // IKapC 函数返回值。
    //
    // Return value of IKapC functions.
    ITKSTATUS res = ITKSTATUS_OK;

    // 可用设备的数量。
    //
    // The number of available devices.
    uint32_t numDevices = 0;

    printf("Itek Console Find Camera Example (C version)\n");

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
        printf(
            "Device Full Name:%s\n Friendly Name:%s\n Vendor Name:%s\n Model Name:%s\n Serial Name:%s\n Device Class:%s\n "
            "Device Version:%s\n User Defined Name:%s\n\n",
            di.FullName, di.FriendlyName, di.VendorName, di.ModelName, di.SerialNumber, di.DeviceClass, di.DeviceVersion,
            di.UserDefinedName);
    }

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
    fprintf(stderr, "Error Code:%08x\n", errc);
    ItkManTerminate();
    pressEnterToExit();
    exit(EXIT_FAILURE);
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