// FindCamera.cpp

/* @brief����ʾ�����û���ʾ���������������豸��
 *
 * @brief��This example shows users how to find available cameras. */

#include <Windows.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "IKapC.h"
#pragma comment(lib, "IKapC.lib")

/* @brief���жϺ����Ƿ�ɹ����á�
 * @param[in] errc����������ֵ��
 *
 * @brief��Determine whether the function is called successfully.
 * @param[in] errc��Function return value. */
#define CHECK(errc)                                                                                                         \
    if (ITKSTATUS_OK != errc)                                                                                               \
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

int main(void)
{
    // IKapC ��������ֵ��
    //
    // Return value of IKapC functions.
    ITKSTATUS res = ITKSTATUS_OK;

    // �����豸��������
    //
    // The number of available devices.
    uint32_t numDevices = 0;

    printf("Itek Console Find Camera Example (C version)\n");

    // ��ʼ�� IKapC ���л�����
    //
    // Initialize IKapC runtime environment.
    res = ItkManInitialize();
    CHECK(res);

    // ö�ٿ����豸���������ڴ��豸ǰ��������� ItkManGetDeviceCount() ������
    //
    // Enumerate the number of available devices. Before opening the device, ItkManGetDeviceCount() function must be called.
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
        printf(
            "Device Full Name:%s\n Friendly Name:%s\n Vendor Name:%s\n Model Name:%s\n Serial Name:%s\n Device Class:%s\n "
            "Device Version:%s\n User Defined Name:%s\n\n",
            di.FullName, di.FriendlyName, di.VendorName, di.ModelName, di.SerialNumber, di.DeviceClass, di.DeviceVersion,
            di.UserDefinedName);
    }

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
void printErrorAndExit(ITKSTATUS errc)
{
    fprintf(stderr, "Error Code:%08x\n", errc);
    ItkManTerminate();
    pressEnterToExit();
    exit(EXIT_FAILURE);
}

// �û�����س������˳�����
//
// Users enter Enter to exit the program.
void pressEnterToExit(void)
{
    fprintf(stderr, "\nPress enter to exit.\n");
    while (getchar() != '\n')
        ;
}