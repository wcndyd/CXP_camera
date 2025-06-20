// ParametrizeCamera_LoadAndSave.cpp

/* @brief：本示例向用户演示如何加载和保存相机配置，演示了通过ItkDevSaveConfigurationToFile将相机属性导出到文件（例如Features.ccf）中；通过ItkDevLoadConfigurationFromFile从文件（例如Features.ccf）中导入相机属性。
 * @note：
 *
 * @brief：This example demonstrates to the user how to load and save the camera configuration, demonstrating exporting the camera attributes to a file (e. g. Features.ccf) via ItkDevSaveConfigurationToFile; importing the camera attributes from a file (e. g. Features.ccf) via ItkDevLoadConfigurationFromFile.
 * @note：
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
    int scanTmp = scanf_s("%d", &devIndex);

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

    printf("Start save the camera features to the file\n");
    printf("Wait......\n");

    // 保存相机配置文件
    //
    // save the camera configuration file
    res = ItkDevSaveConfigurationToFile(g_hCamera, (char *)"Features.ccf");
    CHECK(res);

    printf("Finish save the camera features to the file\n\n");

    printf("Start load the camera features from the file\n");
    printf("Wait......\n");

    // 加载相机配置文件
    //
    // load the camera configuration file
    res = ItkDevLoadConfigurationFromFile(g_hCamera, (char *)"Features.ccf");
    CHECK(res);

    printf("Finish load the camera features from the file\n");

    { int getCharTmp = getchar(); }

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

// 用户输入回车键后退出程序。
//
// Users enter Enter to exit the program.
void pressEnterToExit(void)
{
    fprintf(stderr, "\nPress enter to exit.\n");
    while (getchar() != '\n')
        ;
}