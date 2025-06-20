// main.cpp

/* @brief：本示例向用户演示如何运用IKapC库和IKapBoard库，对相机进行配置。
 *
 * @brief：This example shows users how to use IKapC and IKapBoard libraries to
 * configure camera. */

#include <malloc.h>
#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "../GeneralConfigureCamera/ConfigureCamera.h"

#pragma comment(lib, "IKapBoard.lib")
#pragma comment(lib, "IKapC.lib")
#pragma comment(lib, "shlwapi.lib")

int main()
{
    /// \~chinese IKapC 函数返回值				    \~english Return value of IKapC functions
    ITKSTATUS res = ITKSTATUS_OK;

    /// \~chinese IKapBoard 函数返回值				\~english Return value of IKapBoard functions
    int ret = IK_RTN_OK;

    /// \~chinese 初始化 IKapC 运行环境				\~english Initialize IKapC runtime environment
    res = ItkManInitialize();
    CHECK_IKAPC(res);

    ItkCamera cam;

    cam.g_bufferCount = 10;

    sprintf_s(cam.g_saveFileName, "D:\\CImage.tif");

    cam.g_SerialNumber = NULL;

    cam.g_bSoftTriggerUsed = 0;

    cam.g_bLoadGrabberConfig = 0;

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

    /// \~chinese 打开序号为0的相机并配置相机设备		\~english Open camera with 0 index and configure camera device
    int tmpIndex = -1;
    printf("total device count: %d, the index of  which you want to open is:", numDevices);
    scanf_s("%d", &tmpIndex);
    ConfigureCamera(&cam, tmpIndex);

    res = SetWidth(&cam, 1024);
    CHECK_IKAPC(res);

    int64_t width = 0;
    res           = GetWidth(&cam, &width);
    CHECK_IKAPC(res);
    printf("%d camera's width is %lld.\n",tmpIndex,width);

    res            = SetHeight(&cam, 1024);
    int64_t height = 0;
    res            = GetHeight(&cam, &height);
    CHECK_IKAPC(res);
    printf("%d camera's height is %lld.\n", tmpIndex, height);

    res = SetExposureTime(&cam, 100.0);
    CHECK_IKAPC(res);

    double exposure = 0;
    res             = GetExposureTime(&cam, &exposure);
    CHECK_IKAPC(res);
    printf("%d camera's exposureTime is %f.\n", tmpIndex, exposure);

    /// \~chinese 关闭采集卡设备				        \~english Close frame grabber device
    if (cam.g_hBoard != INVALID_HANDLE_VALUE) {
        ret = IKapClose(cam.g_hBoard);
        CHECK_IKAPBOARD(ret);
    }

    /// \~chinese 关闭相机设备				        \~english Close camera device
    res = ItkDevClose(cam.g_hCamera);
    CHECK_IKAPC(res);

    /// \~chinese 释放 IKapC 运行环境				\~english Release IKapC runtime environment
    ItkManTerminate();

    pressEnterToExit();
    return 0;
}
