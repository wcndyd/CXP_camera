// ParametrizeCamera_LoadAndSave.cpp

/* @brief����ʾ�����û���ʾ��μ��غͱ���������ã���ʾ��ͨ��ItkDevSaveConfigurationToFile��������Ե������ļ�������Features.ccf���У�ͨ��ItkDevLoadConfigurationFromFile���ļ�������Features.ccf���е���������ԡ�
 * @note��
 *
 * @brief��This example demonstrates to the user how to load and save the camera configuration, demonstrating exporting the camera attributes to a file (e. g. Features.ccf) via ItkDevSaveConfigurationToFile; importing the camera attributes from a file (e. g. Features.ccf) via ItkDevLoadConfigurationFromFile.
 * @note��
 */

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

// ����豸�����
//
// Camera device handle.
ITKDEVICE g_hCamera;

/* @brief���û�����س������˳�����
 *
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
    ITKSTATUS res;

    // �����豸��������
    //
    // The number of available devices.
    uint32_t numDevices = 0;

    // ��Ҫ�򿪵��豸��������
    //
    // The index of device to be opened.
    uint32_t devIndex = 0;

    printf("Itek Console Camera Features Example(C version)\n");

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
    printf("total device count: %d, the index of  which you want to open is:", numDevices);
    int scanTmp = scanf_s("%d", &devIndex);

    if (devIndex >= numDevices)
    {
        fprintf(stderr, "index: %d is more than %d.\n", devIndex, numDevices - 1);
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    // ���豸����ȡ�豸�����
    //
    // Open the device and get the handle of the device.
    res = ItkDevOpen(devIndex, ITKDEV_VAL_ACCESS_MODE_CONTROL, &g_hCamera);
    CHECK(res);

    printf("Start save the camera features to the file\n");
    printf("Wait......\n");

    // ������������ļ�
    //
    // save the camera configuration file
    res = ItkDevSaveConfigurationToFile(g_hCamera, (char *)"Features.ccf");
    CHECK(res);

    printf("Finish save the camera features to the file\n\n");

    printf("Start load the camera features from the file\n");
    printf("Wait......\n");

    // ������������ļ�
    //
    // load the camera configuration file
    res = ItkDevLoadConfigurationFromFile(g_hCamera, (char *)"Features.ccf");
    CHECK(res);

    printf("Finish load the camera features from the file\n");

    { int getCharTmp = getchar(); }

    //�ر��豸��
    //
    // Close the device.
    res = ItkDevClose(g_hCamera);
    CHECK(res);

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
    fprintf(stderr, "Error Code:%08X\n", errc);
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