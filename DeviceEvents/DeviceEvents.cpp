// DeviceEvents.cpp

/* @brief����ʾ�����û���ʾ���ע�����FrameTrigger�¼���FrameTriggerIgnored�¼���LineTriggerIgnored�¼���MessageChannel�¼���
 * @note�����в������֧��FrameTrigger�¼���FrameTriggerIgnored�¼���LineTriggerIgnored�¼���MessageChannel�¼�����������˵����ȷ�����֧�ָù��ܣ�MessageChannel�¼�������Ҫ���¼�ѡ������ѡ����Ҫ��ص��¼��������¼�֪ͨģʽ�򿪡�
 *
 * @brief��This example shows the user how to register the camera FrameTrigger event, FrameTriggerIgnored event, LineTriggerIgnored event, MessageChannel event.
 * @note��Only some cameras support FrameTrigger, FrameTriggerIgnored, LineTriggerIgnored, and MessageChannel events. Please consult the camera instructions to ensure that the camera supports this function, For MessageChannel Events, please select the event to be monitored at the event selector and turn the event notification mode on.
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

/* @brief��ע���豸�¼��ص���
 * @param[in] hDev���豸�����
 *
 * @brief��register device events callback.
 * @param[in] hDev��device handle. */
void RegisterDeviceCallback(ITKDEVICE hDev);

/* @brief��ȡ��ע���豸�¼��ص���
 * @param[in] hDev���豸�����
 *
 * @brief��unregister device events callback.
 * @param[in] hDev��device handle. */
void UnregisterDeviceCallback(ITKDEVICE hDev);

/* @brief����������ע��Ϊһ���ص���������FrameTrigger����ֵ�ı�ʱ�����������á�
 * @param[in] context�����������
 * @param[in] eventInfo���¼���Ϣ�����
 *
 * @brief��This function is registered as a callback function. When FrameTrigger feature value is changed, the function will be called.
 * @param[in] context��Input parameter.
 * @param[in] eventInfo��Event information handle. */
void __stdcall FrameTriggerCallback(void* context, ITKEVENTINFO eventInfo);

/* @brief����������ע��Ϊһ���ص���������FrameTriggerIgnored����ֵ�ı�ʱ�����������á�
 * @param[in] context�����������
 * @param[in] eventInfo���¼���Ϣ�����
 *
 * @brief��This function is registered as a callback function. When FrameTriggerIgnored feature value is changed, the function will be called.
 * @param[in] context��Input parameter.
 * @param[in] eventInfo��Event information handle. */
void __stdcall FrameTriggerIgnoredCallback(void* context, ITKEVENTINFO eventInfo);

/* @brief����������ע��Ϊһ���ص���������LineTriggerIgnored����ֵ�ı�ʱ�����������á�
 * @param[in] context�����������
 * @param[in] eventInfo���¼���Ϣ�����
 *
 * @brief��This function is registered as a callback function. When LineTriggerIgnored feature value is changed, the function will be called.
 * @param[in] context��Input parameter.
 * @param[in] eventInfo��Event information handle. */
void __stdcall LineTriggerIgnoredCallback(void* context, ITKEVENTINFO eventInfo);

/* @brief����������ע��Ϊһ���ص���������MessageChannel�¼�������ʱ�����������á�
 * @param[in] context�����������
 * @param[in] eventInfo���¼���Ϣ�����
 *
 * @brief��This function is registered as a callback function. When message channel event is triggered, the function will be called.
 * @param[in] context��Input parameter.
 * @param[in] eventInfo��Event information handle. */
void __stdcall MessageChannelCallback(void* context, ITKEVENTINFO eventInfo);

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
    int tmp = scanf_s("%d", &devIndex);

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

    // ע�����FrameTrigger��FrameTriggerIgnored��LineTriggerIgnored��MessageChannel�¼���
    //
    // register FrameTrigger��FrameTriggerIgnored��LineTriggerIgnored��MessageChannel events.
    RegisterDeviceCallback(g_hCamera);

    { int getCharTmp = getchar(); }

    // ȡ��ע�����FrameTrigger��FrameTriggerIgnored��LineTriggerIgnored��MessageChannel�¼���
    //
    // unregister FrameTrigger��FrameTriggerIgnored��LineTriggerIgnored��MessageChannel events.
    UnregisterDeviceCallback(g_hCamera);


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

void RegisterDeviceCallback(ITKDEVICE hDev)
{
    ITKSTATUS res = ITKSTATUS_OK;
    // ע��FrameTrigger�¼���
    //
    // register FrameTrigger event.
    res = ItkDevRegisterCallback(g_hCamera, "FrameTrigger", FrameTriggerCallback, g_hCamera);
    CHECK(res);

    // ע��FrameTriggerIgnored�¼���
    //
    // register FrameTriggerIgnored event.
    res = ItkDevRegisterCallback(g_hCamera, "FrameTriggerIgnored", FrameTriggerIgnoredCallback, g_hCamera);
    CHECK(res);

    // ע��LineTriggerIgnored�¼���
    //
    // register LineTriggerIgnored event.
    res = ItkDevRegisterCallback(g_hCamera, "LineTriggerIgnored", LineTriggerIgnoredCallback, g_hCamera);
    CHECK(res);

    // ע��MessageChannel�¼���
    //
    // register MessageChannel event.
    res = ItkDevRegisterCallback(g_hCamera, "MessageChannel", MessageChannelCallback, g_hCamera);
    CHECK(res);
}

void UnregisterDeviceCallback(ITKDEVICE hDev)
{
    ITKSTATUS res = ITKSTATUS_OK;
    // ȡ��ע��FrameTrigger�¼���
    //
    // unregister FrameTrigger event.
    res = ItkDevUnregisterCallback(g_hCamera, "FrameTrigger");
    CHECK(res);

    // ȡ��ע��FrameTriggerIgnored�¼���
    //
    // unregister FrameTriggerIgored event.
    res = ItkDevUnregisterCallback(g_hCamera, "FrameTriggerIgnored");
    CHECK(res);

    // ȡ��ע��LineTriggerIgnored�¼���
    //
    // unregister LineTriggerIgnored event.
    res = ItkDevUnregisterCallback(g_hCamera, "LineTriggerIgnored");
    CHECK(res);

    // ȡ��ע��MessageChannel�¼���
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

    // ��ȡ�¼����Ͳ���ֵ��
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // ��ȡ�¼�����ʱ�������ֵ��
    //
    // Get the event time stamp parameter value.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_HOST_TIME_STAMP, &countstamp);
    CHECK(res);

// �ж�����λ������ӡ��Ϣ��
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

    // ��ȡ�¼����Ͳ���ֵ��
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // ��ȡ�¼�����ʱ�������ֵ��
    //
    // Get the event time stamp parameter value.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_HOST_TIME_STAMP, &countstamp);
    CHECK(res);

// �ж�����λ������ӡ��Ϣ��
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

    // ��ȡ�¼����Ͳ���ֵ��
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // ��ȡ�¼�����ʱ�������ֵ��
    //
    // Get the event time stamp parameter value.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_HOST_TIME_STAMP, &countstamp);
    CHECK(res);

// �ж�����λ������ӡ��Ϣ��
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

    // ��ȡ�¼����Ͳ���ֵ��
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // ��ȡ�¼����Ʋ���ֵ���������Ѵ�֪ͨģʽ���¼�����֮һ��
    //
    // Get the event name parameter value, The name is one of the event names with an open notification mode.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_FEATURE_NAME, feature_name);
    CHECK(res);
    int nType = (int)strlen(feature_name);

    // ��ȡ�¼�����ʱ�������ֵ��
    //
    // Get the event time stamp parameter value.
    res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_HOST_TIME_STAMP, &countstamp);
    CHECK(res);

// �ж�����λ������ӡ��Ϣ��
//
// Determine the type bit and print information.
#if __STDC_VERSION__ >= 199901L
    printf("OnMessageChannelCallback, feature: %s, type: %0d, time: %lld.\n", feature_name, nType, countstamp);
#else
    printf("OnMessageChannelCallback, feature: %s, type: %0d, time: %I64d.\n", feature_name, nType, countstamp);
#endif
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
