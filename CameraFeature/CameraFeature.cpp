// CameraFeature.cpp

/* @brief����ʾ�����û���ʾ��η������������
 * @note��������ʾ�������б�������ܣ�����ģʽ���������͡�����ֵ��ȡ���ܣ��Լ�����ֵ���ù��ܡ�
 *
 * @brief��This example shows users how to access camera features.
 * @note��The program demonstrates feature list retrieval function, access mode, data type, feature value getting function,
 * and feature value setting function. */

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

/* @brief��ö����������豸������
 *
 * @brief��Enumerate all camera device features. */
void enumAllFeatures();

/* @brief������32λ��������ֵ��
 * @param[in] featureName���������ơ�
 * @param[in] nValue������ֵ��
 *
 * @brief��Set Int32 type feature value.
 * @param[in] featureName��Feature name.
 * @param[in] nValue��Feature value. */
void setFeatureInt32(const char *featureName, int32_t nValue);

/* @brief����ȡ32λ��������ֵ��
 * @param[in] featureName���������ơ�
 * @param[out] pValue������ֵ��
 *
 * @brief��Get Int32 type feature value.
 * @param[in] featureName��Feature name.
 * @param[out] pValue��Feature value. */
void getFeatureInt32(const char *featureName, int32_t *pValue);

/* @brief������64λ��������ֵ��
 * @param[in] featureName���������ơ�
 * @param[in] nValue������ֵ��
 *
 * @brief��Set Int64 type feature value.
 * @param[in] featureName��Feature name.
 * @param[in] nValue��Feature value. */
void setFeatureInt64(const char *featureName, int64_t nValue);

/* @brief����ȡ64λ��������ֵ��
 * @param[in] featureName���������ơ�
 * @param[out] pValue������ֵ��
 *
 * @brief��Get Int64 type feature value.
 * @param[in] featureName��Feature name.
 * @param[out] pValue��Feature value. */
void getFeatureInt64(const char *featureName, int64_t *pValue);

/* @brief�����ø�������������ֵ��
 * @param[in] featureName���������ơ�
 * @param[in] fValue������ֵ��
 *
 * @brief��Set double type feature value.
 * @param[in] featureName��Feature name.
 * @param[in] fValue��Feature value. */
void setFeatureDouble(const char *featureName, double fValue);

/* @brief����ȡ��������������ֵ��
 * @param[in] featureName���������ơ�
 * @param[out] pValue������ֵ��
 *
 * @brief��Get double type feature value.
 * @param[in] featureName��Feature name.
 * @param[out] pValue��Feature value. */
void getFeatureDouble(const char *featureName, double *pValue);

/* @brief�����ò�����������ֵ��
 * @param[in] featureName���������ơ�
 * @param[in] bValue������ֵ��
 *
 * @brief��Set boolean type feature value.
 * @param[in] featureName��Feature name.
 * @param[in] bValue��Feature value. */
void setFeatureBool(const char *featureName, bool bValue);

/* @brief����ȡ������������ֵ��
 * @param[in] featureName���������ơ�
 * @param[out] pValue������ֵ��
 *
 * @brief��Get boolean type feature value.
 * @param[in] featureName��Feature name.
 * @param[out] pValue��Feature value. */
void getFeatureBool(const char *featureName, bool *pValue);

/* @brief�������ַ�����������ֵ��
 * @param[in] featureName���������ơ�
 * @param[in] strValue������ֵ��
 *
 * @brief��Set string type feature value.
 * @param[in] featureName��Feature name.
 * @param[in] strValue��Feature value. */
void setFeatureStr(const char *featureName, const char *strValue);

/* @brief����ȡ�ַ�����������ֵ��
 * @param[in] featureName���������ơ�
 * @param[out] strValue������ֵ��
 * @param[in,out] pValueLen������ֵ���ȡ�
 *
 * @brief��Get string type feature value.
 * @param[in] featureName��Feature name.
 * @param[out] strValue��Feature value.
 * @param[in,out] pValueLen��The length of feature value. */
void getFeatureStr(const char *featureName, char *strValue, uint32_t *pValueLen);

/* @brief������ö����������ֵ��
 * @param[in] featureName���������ơ�
 * @param[in] strValue������ֵ��
 *
 * @brief��Set enum type feature value.
 * @param[in] featureName��Feature name.
 * @param[in] strValue��Feature value. */
void setFeatureEnum(const char *featureName, const char *strValue);

/* @brief����ȡö����������ֵ��
 * @param[in] featureName���������ơ�
 * @param[out] strValue������ֵ��
 * @param[in,out] pValueLen������ֵ���ȡ�
 *
 * @brief��Get enum type feature value.
 * @param[in] featureName��Feature name.
 * @param[out] strValue��Feature value.
 * @param[in,out] pValueLen��The length of feature value. */
void getFeatureEnum(const char *featureName, char *strValue, uint32_t *pValueLen);

/* @brief������������������ֵ��
 * @param[in] featureName���������ơ�
 *
 * @brief��Set command type feature value.
 * @param[in] featureName��Feature name. */
void setFeatureCommand(const char *featureName);

/* @brief����������ע��Ϊһ���ص�������������ֵ�ı�ʱ�����������á�
 * @param[in] hDev���豸�����
 *
 * @brief��This function is registered as a callback function. When feature value is changed, the function will be called.
 * @param[in] hDev��Device handle. */
void demonstrateFeatureValueChanged(ITKDEVICE hDev);

/* @brief����������ע��Ϊһ���ص�������������ֵ�ı�ʱ�����������á�
 * @param[in] context�����������
 * @param[in] eventInfo���¼���Ϣ�����
 *
 * @brief��This function is registered as a callback function. When feature value is changed, the function will be called.
 * @param[in] context��Input parameter.
 * @param[in] eventInfo��Event information handle. */
void __stdcall featureValueChangeCallback(void *context, ITKEVENTINFO eventInfo);

/* @brief����������ע��Ϊһ���ص������������ӵ��豸����ʱ�����������á�
 * @param[in] hDev���豸�����
 *
 * @brief��This function is registered as a callback function. When the connected device goes offline, the function will be
 * called.
 * @param[in] hDev��Device handle. */
void demonstrateCameraRemove(ITKDEVICE hDev);

/* @brief����������ע��Ϊһ���ص������������ӵ��豸����ʱ�����������á�
 * @param[in] context�����������
 * @param[in] eventInfo���¼���Ϣ�����
 *
 * @brief��This function is registered as a callback function. When the connected device goes offline, the function will be
 * called.
 * @param[in] context��Input parameter.
 * @param[in] eventInfo��Event information handle. */
void _stdcall removalCallbackFunction(void *context, ITKEVENTINFO eventInfo);

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
    int scanTmp=scanf_s("%d", &devIndex);

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

    // ö����������豸������
    //
    // Enumerate all camera device features.
    enumAllFeatures();

    // ��������ע��Ϊһ���ص�������������ֵ�ı�ʱ�����������á�
    //
    // This function is registered as a callback function. When feature value is changed, the function will be called.
    demonstrateFeatureValueChanged(g_hCamera);

    // ��������ע��Ϊһ���ص������������ӵ��豸����ʱ�����������á�
    //
    // This function is registered as a callback function. When the connected device goes offline, the function will be
    // called.
    demonstrateCameraRemove(g_hCamera);

    { int getCharTmp = getchar(); }
    int64_t v;
    getFeatureInt64("SensorWidth",&v );
    printf("width:%d\n", &v);

 
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

// ö����������豸������
//
// Enumerate all camera device features.
void enumAllFeatures()
{
    ITKSTATUS res = ITKSTATUS_OK;

    // �豸����������
    //
    // The number of device features.
    uint32_t nFeatureCount = 0;

    // �������ơ�
    //
    // Feature name.
    char featureName[128];

    // �������Ƴ��ȡ�
    //
    // The length of feature name.
    uint32_t featureNameLen = 128;

    // ������Ϣ��
    //
    // The info of feature.
	ITK_FEATURE_BASE_INFO info;

    // ��������ģʽ��
    //
    // The type of feature.
    uint32_t featureAccessMode = ITKFEATURE_VAL_ACCESS_MODE_UNDEFINED;

    // ��ȡ�豸����������
    //
    // Get the number of device features.
    res = ItkDevGetFeatureCount(g_hCamera, &nFeatureCount);
    CHECK(res);

    for (uint32_t i = 0; i < nFeatureCount; i++) {
        featureNameLen = 128;

        // ��ȡ�������ơ�
        //
        // Get the name of feature.
        res = ItkDevGetFeatureName(g_hCamera, i, featureName, &featureNameLen);
        CHECK(res);

        res = ItkDevGetAccessMode(g_hCamera, featureName, &featureAccessMode);
        CHECK(res);

        char *strAccessMode = "ACCESS_MODE_UNDEFINED";
        switch (featureAccessMode) {
            case ITKFEATURE_VAL_ACCESS_MODE_RW:
                strAccessMode = "RW";
                break;
            case ITKFEATURE_VAL_ACCESS_MODE_RO:
                strAccessMode = "RO";
                break;
            case ITKFEATURE_VAL_ACCESS_MODE_WO:
                strAccessMode = "WO";
                break;
            case ITKFEATURE_VAL_ACCESS_MODE_NI:
                strAccessMode = "NI";
                break;
            case ITKFEATURE_VAL_ACCESS_MODE_NA:
                strAccessMode = "NA";
                break;
            default:
                break;
        }

        // ��ȡ�������͡�
        //
        // Get the type of feature.
		res = ItkDevGetFeatureInfo(g_hCamera, featureName, &info);
        CHECK(res);

        char *strType = "Unknown";
		switch (info.Type) {
            case ITKFEATURE_VAL_TYPE_INT32:
                strType = "INT32";
                break;
            case ITKFEATURE_VAL_TYPE_INT64:
                strType = "INT64";
                break;
            case ITKFEATURE_VAL_TYPE_FLOAT:
                strType = "FLOAT";
                break;
            case ITKFEATURE_VAL_TYPE_DOUBLE:
                strType = "DOUBLE";
                break;
            case ITKFEATURE_VAL_TYPE_BOOL:
                strType = "BOOL";
                break;
            case ITKFEATURE_VAL_TYPE_ENUM:
                strType = "ENUM";
                break;
            case ITKFEATURE_VAL_TYPE_STRING:
                strType = "STRING";
                break;
            case ITKFEATURE_VAL_TYPE_COMMAND:
                strType = "COMMAND";
                break;
            case ITKFEATURE_VAL_TYPE_CATEGORY:
                strType = "CATEGORY";
                break;
            case ITKFEATURE_VAL_TYPE_REGISTER:
                strType = "REGISTER";
                break;
            default:
                break;
        }
        printf("Feature Name : %s  access mode : %s  feature type : %s\n", featureName, strAccessMode, strType);
    }
}

// ����32λ��������ֵ��
//
// Set Int32 type feature value.
void setFeatureInt32(const char *featureName, int32_t nValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevSetInt32(g_hCamera, featureName, nValue);
    CHECK(res);
}

// ��ȡ32λ��������ֵ��
//
// Get Int32 type feature value.
void getFeatureInt32(const char *featureName, int32_t *pValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevGetInt32(g_hCamera, featureName, pValue);
    CHECK(res);
}

// ����64λ��������ֵ��
//
// Set Int64 type feature value.
void setFeatureInt64(const char *featureName, int64_t nValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevSetInt64(g_hCamera, featureName, nValue);
    CHECK(res);
}

// ��ȡ64λ��������ֵ��
//
// Get Int64 type feature value.
void getFeatureInt64(const char *featureName, int64_t *pValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevGetInt64(g_hCamera, featureName, pValue);
    CHECK(res);
}

// ���ø�������������ֵ��
//
// Set double type feature value.
void setFeatureDouble(const char *featureName, double fValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevSetDouble(g_hCamera, featureName, fValue);
    CHECK(res);
}

// ��ȡ��������������ֵ��
//
// Get double type feature value.
void getFeatureDouble(const char *featureName, double *pValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevGetDouble(g_hCamera, featureName, pValue);
    CHECK(res);
}

// ���ò�����������ֵ��
//
// Set boolean type feature value.
void setFeatureBool(const char *featureName, bool bValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevSetBool(g_hCamera, featureName, bValue);
    CHECK(res);
}

// ��ȡ������������ֵ��
//
// Get boolean type feature value.
void getFeatureBool(const char *featureName, bool *pValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevGetBool(g_hCamera, featureName, pValue);
    CHECK(res);
}

// �����ַ�����������ֵ��
//
// Set string type feature value.
void setFeatureStr(const char *featureName, const char *strValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevFromString(g_hCamera, featureName, strValue);
    CHECK(res);
}

// ��ȡ�ַ�����������ֵ��
//
// Get string type feature value.
void getFeatureStr(const char *featureName, char *strValue, uint32_t *pValueLen)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevToString(g_hCamera, featureName, strValue, pValueLen);
    CHECK(res);
}

// ����ö����������ֵ��
//
// Set enum type feature value.
void setFeatureEnum(const char *featureName, const char *strValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevFromString(g_hCamera, featureName, strValue);
    CHECK(res);
}

// ��ȡö����������ֵ��
//
// Get enum type feature value.
void getFeatureEnum(const char *featureName, char *strValue, uint32_t *pValueLen)
{
    ITKSTATUS res = ITKSTATUS_OK;

    // ö����Ϣ��
    //
    // Enumeration info.
	ITK_FEATURE_ENUM_ENTRY_INFO enumInfo;

    // ö����������
    //
    // The number of enumeration.
    uint32_t featureValueCount = 0;

    // ��ȡö����������
    //
    // Get the number enumerations.
    res = ItkDevGetEnumCount(g_hCamera, featureName, &featureValueCount);
    CHECK(res);

    for (uint32_t i = 0; i < featureValueCount; i++) {
        // ��ȡö������Ϣ��
        //
        // Get the name enumeration.
		memset(&enumInfo, 0, sizeof(ITK_FEATURE_ENUM_ENTRY_INFO));
		res = ItkDevGetEnumEntryFeatureInfo(g_hCamera, featureName, (uint64_t)i, &enumInfo);
        CHECK(res);
		if (enumInfo.AccessMode == ITKFEATURE_VAL_ACCESS_MODE_RW ||
			enumInfo.AccessMode == ITKFEATURE_VAL_ACCESS_MODE_RO ||
			enumInfo.AccessMode == ITKFEATURE_VAL_ACCESS_MODE_WO)
			printf("Enum string %d value: %s - displayName: %s\n", i, enumInfo.ValueStr, enumInfo.DisplayName);
    }

    // ����ö����������ֵ��
    //
    // Set enum type feature value.
    res = ItkDevFromString(g_hCamera, featureName, strValue);
    CHECK(res);
}

// ����������������ֵ��
//
// Set command type feature value.
void setFeatureCommand(const char *featureName)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevExecuteCommand(g_hCamera, featureName);
    CHECK(res);
}

// ��������ע��Ϊһ���ص�������������ֵ�ı�ʱ�����������á�
//
// This function is registered as a callback function. When feature value is changed, the function will be called.
void demonstrateFeatureValueChanged(ITKDEVICE hDev)
{
    ITKSTATUS res;

    // ע��ص�������
    //
    // Register callback function.
    res = ItkDevRegisterCallback(hDev, "FeatureValueChanged", featureValueChangeCallback, hDev);
    CHECK(res);
}

// ��������ע��Ϊһ���ص������������ӵ��豸����ʱ�����������á�
//
// This function is registered as a callback function. When the connected device goes offline, the function will be called.
void demonstrateCameraRemove(ITKDEVICE hDev)
{
    ITKSTATUS res;

    // ע��ص�������
    //
    // Register callback function.
    res = ItkDevRegisterCallback(hDev, "DeviceRemove", removalCallbackFunction, hDev);
    CHECK(res);
}

// ��������ע��Ϊһ���ص�������������ֵ�ı�ʱ�����������á�
//
// This function is registered as a callback function. When feature value is changed, the function will be called.
void __stdcall featureValueChangeCallback(void *context, ITKEVENTINFO eventInfo)
{
    uint32_t  type = 0, feature_name_length = 128;
    uint64_t  countstamp        = 0;
    char      feature_name[128] = {0};
    ITKDEVICE hDev              = (ITKDEVICE)context;

    // ��ȡ�¼����Ͳ���ֵ��
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // ��ȡ�¼����Ʋ���ֵ��
    //
    // Get the event name parameter value.
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
    printf("OnFeatureValueChangeCallback, feature: %s, type: %0d, time: %lld.\n", feature_name, nType, countstamp);
#else
    printf("OnFeatureValueChangeCallback, feature: %s, type: %0d, time: %I64d.\n", feature_name, nType, countstamp);
#endif
}

// ��������ע��Ϊһ���ص������������ӵ��豸����ʱ�����������á�
//
// This function is registered as a callback function. When the connected device goes offline, the function will be called.
void _stdcall removalCallbackFunction(void *context, ITKEVENTINFO eventInfo)
{
    uint32_t  type       = 0;
    uint64_t  countstamp = 0;
    ITKDEVICE hDev       = (ITKDEVICE)context;

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
    printf("removalCallbackFunction, type: %08X, time: %lld.\n", type, countstamp);
#else
    printf("\nremovalCallbackFunction, type: %08X, time: %I64d.\n", type, countstamp);
#endif
}