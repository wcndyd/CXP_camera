// CameraFeature.cpp

/* @brief：本示例向用户演示如何访问相机特征。
 * @note：程序演示了特征列表检索功能，访问模式、数据类型、特征值获取功能，以及特征值设置功能。
 *
 * @brief：This example shows users how to access camera features.
 * @note：The program demonstrates feature list retrieval function, access mode, data type, feature value getting function,
 * and feature value setting function. */

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

/* @brief：枚举所有相机设备特征。
 *
 * @brief：Enumerate all camera device features. */
void enumAllFeatures();

/* @brief：设置32位整型特征值。
 * @param[in] featureName：特征名称。
 * @param[in] nValue：特征值。
 *
 * @brief：Set Int32 type feature value.
 * @param[in] featureName：Feature name.
 * @param[in] nValue：Feature value. */
void setFeatureInt32(const char *featureName, int32_t nValue);

/* @brief：获取32位整型特征值。
 * @param[in] featureName：特征名称。
 * @param[out] pValue：特征值。
 *
 * @brief：Get Int32 type feature value.
 * @param[in] featureName：Feature name.
 * @param[out] pValue：Feature value. */
void getFeatureInt32(const char *featureName, int32_t *pValue);

/* @brief：设置64位整型特征值。
 * @param[in] featureName：特征名称。
 * @param[in] nValue：特征值。
 *
 * @brief：Set Int64 type feature value.
 * @param[in] featureName：Feature name.
 * @param[in] nValue：Feature value. */
void setFeatureInt64(const char *featureName, int64_t nValue);

/* @brief：获取64位整型特征值。
 * @param[in] featureName：特征名称。
 * @param[out] pValue：特征值。
 *
 * @brief：Get Int64 type feature value.
 * @param[in] featureName：Feature name.
 * @param[out] pValue：Feature value. */
void getFeatureInt64(const char *featureName, int64_t *pValue);

/* @brief：设置浮点数类型特征值。
 * @param[in] featureName：特征名称。
 * @param[in] fValue：特征值。
 *
 * @brief：Set double type feature value.
 * @param[in] featureName：Feature name.
 * @param[in] fValue：Feature value. */
void setFeatureDouble(const char *featureName, double fValue);

/* @brief：获取浮点数类型特征值。
 * @param[in] featureName：特征名称。
 * @param[out] pValue：特征值。
 *
 * @brief：Get double type feature value.
 * @param[in] featureName：Feature name.
 * @param[out] pValue：Feature value. */
void getFeatureDouble(const char *featureName, double *pValue);

/* @brief：设置布尔类型特征值。
 * @param[in] featureName：特征名称。
 * @param[in] bValue：特征值。
 *
 * @brief：Set boolean type feature value.
 * @param[in] featureName：Feature name.
 * @param[in] bValue：Feature value. */
void setFeatureBool(const char *featureName, bool bValue);

/* @brief：获取布尔类型特征值。
 * @param[in] featureName：特征名称。
 * @param[out] pValue：特征值。
 *
 * @brief：Get boolean type feature value.
 * @param[in] featureName：Feature name.
 * @param[out] pValue：Feature value. */
void getFeatureBool(const char *featureName, bool *pValue);

/* @brief：设置字符串类型特征值。
 * @param[in] featureName：特征名称。
 * @param[in] strValue：特征值。
 *
 * @brief：Set string type feature value.
 * @param[in] featureName：Feature name.
 * @param[in] strValue：Feature value. */
void setFeatureStr(const char *featureName, const char *strValue);

/* @brief：获取字符串类型特征值。
 * @param[in] featureName：特征名称。
 * @param[out] strValue：特征值。
 * @param[in,out] pValueLen：特征值长度。
 *
 * @brief：Get string type feature value.
 * @param[in] featureName：Feature name.
 * @param[out] strValue：Feature value.
 * @param[in,out] pValueLen：The length of feature value. */
void getFeatureStr(const char *featureName, char *strValue, uint32_t *pValueLen);

/* @brief：设置枚举类型特征值。
 * @param[in] featureName：特征名称。
 * @param[in] strValue：特征值。
 *
 * @brief：Set enum type feature value.
 * @param[in] featureName：Feature name.
 * @param[in] strValue：Feature value. */
void setFeatureEnum(const char *featureName, const char *strValue);

/* @brief：获取枚举类型特征值。
 * @param[in] featureName：特征名称。
 * @param[out] strValue：特征值。
 * @param[in,out] pValueLen：特征值长度。
 *
 * @brief：Get enum type feature value.
 * @param[in] featureName：Feature name.
 * @param[out] strValue：Feature value.
 * @param[in,out] pValueLen：The length of feature value. */
void getFeatureEnum(const char *featureName, char *strValue, uint32_t *pValueLen);

/* @brief：设置命令类型特征值。
 * @param[in] featureName：特征名称。
 *
 * @brief：Set command type feature value.
 * @param[in] featureName：Feature name. */
void setFeatureCommand(const char *featureName);

/* @brief：本函数被注册为一个回调函数。当特征值改变时，函数被调用。
 * @param[in] hDev：设备句柄。
 *
 * @brief：This function is registered as a callback function. When feature value is changed, the function will be called.
 * @param[in] hDev：Device handle. */
void demonstrateFeatureValueChanged(ITKDEVICE hDev);

/* @brief：本函数被注册为一个回调函数。当特征值改变时，函数被调用。
 * @param[in] context：输入参数。
 * @param[in] eventInfo：事件信息句柄。
 *
 * @brief：This function is registered as a callback function. When feature value is changed, the function will be called.
 * @param[in] context：Input parameter.
 * @param[in] eventInfo：Event information handle. */
void __stdcall featureValueChangeCallback(void *context, ITKEVENTINFO eventInfo);

/* @brief：本函数被注册为一个回调函数。当连接的设备掉线时，函数被调用。
 * @param[in] hDev：设备句柄。
 *
 * @brief：This function is registered as a callback function. When the connected device goes offline, the function will be
 * called.
 * @param[in] hDev：Device handle. */
void demonstrateCameraRemove(ITKDEVICE hDev);

/* @brief：本函数被注册为一个回调函数。当连接的设备掉线时，函数被调用。
 * @param[in] context：输入参数。
 * @param[in] eventInfo：事件信息句柄。
 *
 * @brief：This function is registered as a callback function. When the connected device goes offline, the function will be
 * called.
 * @param[in] context：Input parameter.
 * @param[in] eventInfo：Event information handle. */
void _stdcall removalCallbackFunction(void *context, ITKEVENTINFO eventInfo);

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
    int scanTmp=scanf_s("%d", &devIndex);

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

    // 枚举所有相机设备特征。
    //
    // Enumerate all camera device features.
    enumAllFeatures();

    // 本函数被注册为一个回调函数。当特征值改变时，函数被调用。
    //
    // This function is registered as a callback function. When feature value is changed, the function will be called.
    demonstrateFeatureValueChanged(g_hCamera);

    // 本函数被注册为一个回调函数。当连接的设备掉线时，函数被调用。
    //
    // This function is registered as a callback function. When the connected device goes offline, the function will be
    // called.
    demonstrateCameraRemove(g_hCamera);

    { int getCharTmp = getchar(); }
    int64_t v;
    getFeatureInt64("SensorWidth",&v );
    printf("width:%d\n", &v);

 
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

// 枚举所有相机设备特征。
//
// Enumerate all camera device features.
void enumAllFeatures()
{
    ITKSTATUS res = ITKSTATUS_OK;

    // 设备特征数量。
    //
    // The number of device features.
    uint32_t nFeatureCount = 0;

    // 特征名称。
    //
    // Feature name.
    char featureName[128];

    // 特征名称长度。
    //
    // The length of feature name.
    uint32_t featureNameLen = 128;

    // 特征信息。
    //
    // The info of feature.
	ITK_FEATURE_BASE_INFO info;

    // 特征访问模式。
    //
    // The type of feature.
    uint32_t featureAccessMode = ITKFEATURE_VAL_ACCESS_MODE_UNDEFINED;

    // 获取设备特征数量。
    //
    // Get the number of device features.
    res = ItkDevGetFeatureCount(g_hCamera, &nFeatureCount);
    CHECK(res);

    for (uint32_t i = 0; i < nFeatureCount; i++) {
        featureNameLen = 128;

        // 获取特征名称。
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

        // 获取特征类型。
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

// 设置32位整型特征值。
//
// Set Int32 type feature value.
void setFeatureInt32(const char *featureName, int32_t nValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevSetInt32(g_hCamera, featureName, nValue);
    CHECK(res);
}

// 获取32位整型特征值。
//
// Get Int32 type feature value.
void getFeatureInt32(const char *featureName, int32_t *pValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevGetInt32(g_hCamera, featureName, pValue);
    CHECK(res);
}

// 设置64位整型特征值。
//
// Set Int64 type feature value.
void setFeatureInt64(const char *featureName, int64_t nValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevSetInt64(g_hCamera, featureName, nValue);
    CHECK(res);
}

// 获取64位整型特征值。
//
// Get Int64 type feature value.
void getFeatureInt64(const char *featureName, int64_t *pValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevGetInt64(g_hCamera, featureName, pValue);
    CHECK(res);
}

// 设置浮点数类型特征值。
//
// Set double type feature value.
void setFeatureDouble(const char *featureName, double fValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevSetDouble(g_hCamera, featureName, fValue);
    CHECK(res);
}

// 获取浮点数类型特征值。
//
// Get double type feature value.
void getFeatureDouble(const char *featureName, double *pValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevGetDouble(g_hCamera, featureName, pValue);
    CHECK(res);
}

// 设置布尔类型特征值。
//
// Set boolean type feature value.
void setFeatureBool(const char *featureName, bool bValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevSetBool(g_hCamera, featureName, bValue);
    CHECK(res);
}

// 获取布尔类型特征值。
//
// Get boolean type feature value.
void getFeatureBool(const char *featureName, bool *pValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevGetBool(g_hCamera, featureName, pValue);
    CHECK(res);
}

// 设置字符串类型特征值。
//
// Set string type feature value.
void setFeatureStr(const char *featureName, const char *strValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevFromString(g_hCamera, featureName, strValue);
    CHECK(res);
}

// 获取字符串类型特征值。
//
// Get string type feature value.
void getFeatureStr(const char *featureName, char *strValue, uint32_t *pValueLen)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevToString(g_hCamera, featureName, strValue, pValueLen);
    CHECK(res);
}

// 设置枚举类型特征值。
//
// Set enum type feature value.
void setFeatureEnum(const char *featureName, const char *strValue)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevFromString(g_hCamera, featureName, strValue);
    CHECK(res);
}

// 获取枚举类型特征值。
//
// Get enum type feature value.
void getFeatureEnum(const char *featureName, char *strValue, uint32_t *pValueLen)
{
    ITKSTATUS res = ITKSTATUS_OK;

    // 枚举信息。
    //
    // Enumeration info.
	ITK_FEATURE_ENUM_ENTRY_INFO enumInfo;

    // 枚举项数量。
    //
    // The number of enumeration.
    uint32_t featureValueCount = 0;

    // 获取枚举项数量。
    //
    // Get the number enumerations.
    res = ItkDevGetEnumCount(g_hCamera, featureName, &featureValueCount);
    CHECK(res);

    for (uint32_t i = 0; i < featureValueCount; i++) {
        // 获取枚举项信息。
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

    // 设置枚举类型特征值。
    //
    // Set enum type feature value.
    res = ItkDevFromString(g_hCamera, featureName, strValue);
    CHECK(res);
}

// 设置命令类型特征值。
//
// Set command type feature value.
void setFeatureCommand(const char *featureName)
{
    ITKSTATUS res = ITKSTATUS_OK;
    res           = ItkDevExecuteCommand(g_hCamera, featureName);
    CHECK(res);
}

// 本函数被注册为一个回调函数。当特征值改变时，函数被调用。
//
// This function is registered as a callback function. When feature value is changed, the function will be called.
void demonstrateFeatureValueChanged(ITKDEVICE hDev)
{
    ITKSTATUS res;

    // 注册回调函数。
    //
    // Register callback function.
    res = ItkDevRegisterCallback(hDev, "FeatureValueChanged", featureValueChangeCallback, hDev);
    CHECK(res);
}

// 本函数被注册为一个回调函数。当连接的设备掉线时，函数被调用。
//
// This function is registered as a callback function. When the connected device goes offline, the function will be called.
void demonstrateCameraRemove(ITKDEVICE hDev)
{
    ITKSTATUS res;

    // 注册回调函数。
    //
    // Register callback function.
    res = ItkDevRegisterCallback(hDev, "DeviceRemove", removalCallbackFunction, hDev);
    CHECK(res);
}

// 本函数被注册为一个回调函数。当特征值改变时，函数被调用。
//
// This function is registered as a callback function. When feature value is changed, the function will be called.
void __stdcall featureValueChangeCallback(void *context, ITKEVENTINFO eventInfo)
{
    uint32_t  type = 0, feature_name_length = 128;
    uint64_t  countstamp        = 0;
    char      feature_name[128] = {0};
    ITKDEVICE hDev              = (ITKDEVICE)context;

    // 获取事件类型参数值。
    //
    // Get the event type parameter value.
    ITKSTATUS res = ItkEventInfoGetPrm(eventInfo, ITKEVENTINFO_PRM_TYPE, &type);
    CHECK(res);

    // 获取事件名称参数值。
    //
    // Get the event name parameter value.
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
    printf("OnFeatureValueChangeCallback, feature: %s, type: %0d, time: %lld.\n", feature_name, nType, countstamp);
#else
    printf("OnFeatureValueChangeCallback, feature: %s, type: %0d, time: %I64d.\n", feature_name, nType, countstamp);
#endif
}

// 本函数被注册为一个回调函数。当连接的设备掉线时，函数被调用。
//
// This function is registered as a callback function. When the connected device goes offline, the function will be called.
void _stdcall removalCallbackFunction(void *context, ITKEVENTINFO eventInfo)
{
    uint32_t  type       = 0;
    uint64_t  countstamp = 0;
    ITKDEVICE hDev       = (ITKDEVICE)context;

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
    printf("removalCallbackFunction, type: %08X, time: %lld.\n", type, countstamp);
#else
    printf("\nremovalCallbackFunction, type: %08X, time: %I64d.\n", type, countstamp);
#endif
}