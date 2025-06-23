#include "ConfigureCamera.h"



void ConfigureCamera(pItkCamera pCam, int index, const char* deviceClass)
{
    ITKSTATUS res = ITKSTATUS_OK;
    int       ret = IK_RTN_OK;

    uint32_t numCameras = 0;

    /// \~chinese 枚举可用相机的数量。在打开相机前，必须调用 ItkManGetDeviceCount() 函数				\~english Enumerate
    /// the number of available cameras. Before opening the camera, ItkManGetDeviceCount() function must be called.
    res = ItkManGetDeviceCount(&numCameras);
    CHECK_IKAPC(res);

    if (numCameras == 0 || pCam == NULL) {
        printf("No camera.\n");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    uint32_t    i = 0;
    ITKDEV_INFO di;

    /// \~chinese 打开采集卡连接的相机				          \~english Open grabber linked camera
    for (; i < numCameras; i++) {
        /// \~chinese 获取相机设备信息				              \~english Get camera device information
        res = ItkManGetDeviceInfo(i, &di);

        if (pCam->g_SerialNumber != NULL) {
            /// \~chinese 当设备序列号正确时				          \~english When the serial number is proper
            if (strcmp(di.SerialNumber, pCam->g_SerialNumber) == 0 && strcmp(di.SerialNumber, "") != 0) {
                printf("Using camera: serial: %s, name: %s, interface: %s.\n", di.SerialNumber, di.FullName, di.DeviceClass);
                res = ItkDevOpen(i, ITKDEV_VAL_ACCESS_MODE_EXCLUSIVE, &pCam->g_hCamera);
                CHECK_IKAPC(res);
                break;
            }
        }
        else if (index >= 0) {
            /// \~chinese 当设备序号正确时				               \~english When the index is proper
            if (i == index) {
                printf("Using camera: serial: %s, name: %s, interface: %s.\n", di.SerialNumber, di.FullName, di.DeviceClass);
                res = ItkDevOpen(i, ITKDEV_VAL_ACCESS_MODE_EXCLUSIVE, &pCam->g_hCamera);
                CHECK_IKAPC(res);
                pCam->g_index = i;
                break;
            }
        }
        else if (deviceClass != NULL) {
            /// \~chinese 当设备类型正确时				               \~english When the device class is proper
            if (strcmp(di.DeviceClass, deviceClass) == 0) {
                printf("Using camera: serial: %s, name: %s, interface: %s.\n", di.SerialNumber, di.FullName, di.DeviceClass);
                res = ItkDevOpen(i, ITKDEV_VAL_ACCESS_MODE_EXCLUSIVE, &pCam->g_hCamera);
                CHECK_IKAPC(res);
                break;
            }
        }
        else {
            printf("Please set proper attribute to open camera.\n");
            ItkManTerminate();
            pressEnterToExit();
            exit(EXIT_FAILURE);
        }
    }


    if (i >= numCameras) {
        printf("Cannot find proper camera.\n");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    // \~chinese 设置相机的超时时间				\~english set timeout of camera
    uint32_t timeout = 20000;
    res = ItkDevSetPrm(pCam->g_hCamera, ITKDEV_PRM_HEARTBEAT_TIMEOUT, &timeout);
    CHECK(res);

    // \~chinese 获取特征名称列表并配置参数设置方法				\~english Get list of features' name and select parameter
    // configure method
    GetAllConfigureMethods(pCam);

    pCam->g_devInfo = di;
    ITK_CXP_DEV_INFO cxp_cam_info = {};
    ITK_GVB_DEV_INFO gvb_cam_info = {};
    ITK_CL_DEV_INFO  cl_cam_info = {};
    void* info = NULL;
    if (strcmp(di.DeviceClass, "CoaXPress") == 0) {
        /// \~chinese 获取CoaXPress相机设备信息				    \~english Get CoaXPress camera device information
        res = ItkManGetCXPDeviceInfo(i, &cxp_cam_info);
        CHECK_IKAPC(res);
        info = &cxp_cam_info;
    }
    else if (strcmp(di.DeviceClass, "GigEVisionBoard") == 0) {
        /// \~chinese 获取GigEVision相机设备信息				    \~english Get GigEVision camera device information
        res = ItkManGetGVBDeviceInfo(i, &gvb_cam_info);
        CHECK_IKAPC(res);
        info = &gvb_cam_info;

    }
    else if (strcmp(di.DeviceClass, "CameraLink") == 0) {
        /// \~chinese 获取CameraLink相机设备信息				    \~english Get CameraLink camera device information
        res = ItkManGetCLDeviceInfo(i, &cl_cam_info);
        CHECK_IKAPC(res);
        info = &cl_cam_info;
    }
    else {
        return;
    }

    pCam->g_hBoard = IKapOpenWithSpecificInfo(info);
    if (pCam->g_hBoard == INVALID_HANDLE_VALUE)
        CHECK_IKAPBOARD(IKStatus_OpenBoardFail);
}

void GetAllConfigureMethods(pItkCamera pCam)
{
    ITKSTATUS  res = ITKSTATUS_OK;
    ITKFEATURE pFea = NULL;

    // width
    if (ItkDevAllocFeature(pCam->g_hCamera, "RegionEnable", &pFea) == ITKSTATUS_OK) {
        if (ItkDevAllocFeature(pCam->g_hCamera, "RegionWidth", &pFea) == ITKSTATUS_OK) {
            pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG] = ITEK_CAMERA_WIDTH_CONFIG_METHOD_1;
        }
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "ROIEnable", &pFea) == ITKSTATUS_OK) {
        if (ItkDevAllocFeature(pCam->g_hCamera, "ROIWidth", &pFea) == ITKSTATUS_OK) {
            pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG] = ITEK_CAMERA_WIDTH_CONFIG_METHOD_2;
        }
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "RegionWidth", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG] = ITEK_CAMERA_WIDTH_CONFIG_METHOD_3;

    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "ROIWidth", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG] = ITEK_CAMERA_WIDTH_CONFIG_METHOD_4;
    }

    // height
    if (ItkDevAllocFeature(pCam->g_hCamera, "RegionEnable", &pFea) == ITKSTATUS_OK) {
        if (ItkDevAllocFeature(pCam->g_hCamera, "RegionHeight", &pFea) == ITKSTATUS_OK) {
            pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG] = ITEK_CAMERA_HEIGHT_CONFIG_METHOD_1;
        }
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "ROIEnable", &pFea) == ITKSTATUS_OK) {
        if (ItkDevAllocFeature(pCam->g_hCamera, "ROIHeight", &pFea) == ITKSTATUS_OK) {
            pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG] = ITEK_CAMERA_HEIGHT_CONFIG_METHOD_2;
        }
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "RegionHeight", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG] = ITEK_CAMERA_HEIGHT_CONFIG_METHOD_3;

    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "ROIHeight", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG] = ITEK_CAMERA_HEIGHT_CONFIG_METHOD_4;
    }

    // exposure time
    if (ItkDevAllocFeature(pCam->g_hCamera, "ExposureTimeRaw", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG] = ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_1;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "AreaMode", &pFea) == ITKSTATUS_OK) {
        if (ItkDevAllocFeature(pCam->g_hCamera, "ExposureTime", &pFea) == ITKSTATUS_OK) {
            pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG] = ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_2;

        }
        else if (ItkDevAllocFeature(pCam->g_hCamera, "AreaExposureTime", &pFea) == ITKSTATUS_OK) {
            pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG] = ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_3;
        }
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "ExposureTime", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG] = ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_4;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "Area", &pFea) == ITKSTATUS_OK) {
        if (ItkDevAllocFeature(pCam->g_hCamera, "AreaExposureTime", &pFea) == ITKSTATUS_OK) {
            pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG] = ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_5;
        }
    }

    // digital Gain
    if (ItkDevAllocFeature(pCam->g_hCamera, "DigitalGain", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_DIGITAL_GAIN_CONFIG] = ITEK_CAMERA_DIGITAL_GAIN_CONFIG_METHOD_1;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "Gain", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_DIGITAL_GAIN_CONFIG] = ITEK_CAMERA_DIGITAL_GAIN_CONFIG_METHOD_2;
    }

    // line Rate
    if (ItkDevAllocFeature(pCam->g_hCamera, "LinePeriodTime", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_LINE_RATE_CONFIG] = ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_1;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "AcquisitionLinePeriod", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_LINE_RATE_CONFIG] = ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_2;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "AcquisitionLineRate", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_LINE_RATE_CONFIG] = ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_3;
    }

    // frame Rate
    if (ItkDevAllocFeature(pCam->g_hCamera, "AcquisitionFramePeriodRaw", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_FRAME_RATE_CONFIG] = ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_1;

    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "AcquisitionFramePeriod", &pFea) == ITKSTATUS_OK) {
        uint32_t type = ITKFEATURE_VAL_TYPE_UNDEFINED;
        ItkFeatureGetType(pFea, &type);
        if (ITKFEATURE_VAL_TYPE_INT64 == type) {
            pCam->g_configMethod[ITEK_CAMERA_FRAME_RATE_CONFIG] = ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_2;
        }
        else if (ITKFEATURE_VAL_TYPE_DOUBLE == type) {
            pCam->g_configMethod[ITEK_CAMERA_FRAME_RATE_CONFIG] = ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_3;
        }

    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "FramePeriodTime", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_FRAME_RATE_CONFIG] = ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_4;
    }
    else if (
        ItkDevAllocFeature(pCam->g_hCamera, "AcquisitionFrameRateEnable", &pFea) == ITKSTATUS_OK &&
        ItkDevAllocFeature(pCam->g_hCamera, "AcquisitionFrameRate", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_FRAME_RATE_CONFIG] = ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_5;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "AcquisitionFrameRate", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_FRAME_RATE_CONFIG] = ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_6;
    }

    // line Trigger
    if (ItkDevAllocFeature(pCam->g_hCamera, "TriggerSelector", &pFea) == ITKSTATUS_OK) {
        if (ItkDevAllocFeature(pCam->g_hCamera, "TriggerMode", &pFea) == ITKSTATUS_OK &&
            ItkDevAllocFeature(pCam->g_hCamera, "ExposureMode", &pFea) == ITKSTATUS_OK) {
            pCam->g_configMethod[ITEK_CAMERA_LINE_TRIGGER_CONFIG] = ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_1;
        }

    }
    else if (
        ItkDevAllocFeature(pCam->g_hCamera, "TriggerMode", &pFea) == ITKSTATUS_OK &&
        ItkDevAllocFeature(pCam->g_hCamera, "ExposureMode", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_LINE_TRIGGER_CONFIG] = ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_2;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "SynchronizationMode", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_LINE_TRIGGER_CONFIG] = ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_3;
    }

    // frame Trigger
    if (ItkDevAllocFeature(pCam->g_hCamera, "TriggerSelector", &pFea) == ITKSTATUS_OK) {
        if (ItkDevAllocFeature(pCam->g_hCamera, "TriggerMode", &pFea) == ITKSTATUS_OK &&
            ItkDevAllocFeature(pCam->g_hCamera, "ExposureMode", &pFea) == ITKSTATUS_OK) {
            pCam->g_configMethod[ITEK_CAMERA_FRAME_TRIGGER_CONFIG] = ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_1;
        }
    }
    else if (
        ItkDevAllocFeature(pCam->g_hCamera, "FrameTriggerEnable", &pFea) == ITKSTATUS_OK &&
        ItkDevAllocFeature(pCam->g_hCamera, "FrameTriggerType", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_FRAME_TRIGGER_CONFIG] = ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_2;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "ExposureMode", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_FRAME_TRIGGER_CONFIG] = ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_3;
    }

    // trigger source
    if (ItkDevAllocFeature(pCam->g_hCamera, "FrameTriggerSource", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_TRIGGER_SOURCE_CONFIG] = ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_1;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "ExternalTriggerSource", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_TRIGGER_SOURCE_CONFIG] = ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_2;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "TriggerSource", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_TRIGGER_SOURCE_CONFIG] = ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_3;
    }

    // trigger polarity
    if (ItkDevAllocFeature(pCam->g_hCamera, "FrameTriggerPolarity", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_TRIGGER_POLARITY_CONFIG] = ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_1;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "ExternalTriggerActivation", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_TRIGGER_POLARITY_CONFIG] = ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_2;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "LinePolarity", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_TRIGGER_POLARITY_CONFIG] = ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_3;
    }
    else if (ItkDevAllocFeature(pCam->g_hCamera, "TriggerActivation", &pFea) == ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_TRIGGER_POLARITY_CONFIG] = ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_4;
    }

    // multi exposure
    if (ItkDevAllocFeature(pCam->g_hCamera, "MultiExposureTimeCount", &pFea) ==
        ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_MULTI_EXPOSURE_CONFIG] =
            ITEK_CAMERA_MULTI_EXPOSURE_CONFIG_METHOD_1;
    }

    // chunk data
    if (ItkDevAllocFeature(pCam->g_hCamera, "ChunkModeActive", &pFea) ==
        ITKSTATUS_OK &&
        ItkDevAllocFeature(pCam->g_hCamera, "ChunkSelector", &pFea) ==
        ITKSTATUS_OK &&
        ItkDevAllocFeature(pCam->g_hCamera, "ChunkEnable", &pFea) ==
        ITKSTATUS_OK) {
        pCam->g_configMethod[ITEK_CAMERA_CHUNK_DATA_CONFIG] =
            ITEK_CAMERA_CHUNK_DATA_CONFIG_METHOD_1;
    }
}

ITKSTATUS SetWidth(pItkCamera pCam, int64_t width)
{
    ITKSTATUS  res = ITKSTATUS_OK;
    int        ret = IK_RTN_OK;
    ITKFEATURE pFea = NULL;
    uint32_t   accMode = ITKFEATURE_VAL_ACCESS_MODE_NA;
    ItkDevGetAccessMode(pCam->g_hCamera, "Width", &accMode);
    if (ItkDevAllocFeature(pCam->g_hCamera, "Width", &pFea) == ITKSTATUS_OK && ITKFEATURE_VAL_ACCESS_MODE_RW == accMode) {
        return ItkDevSetInt64(pCam->g_hCamera, "Width", width);
    }
    switch (pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG]) {
    case ITEK_CAMERA_WIDTH_CONFIG_METHOD_1:
        res = ItkDevSetBool(pCam->g_hCamera, "RegionEnable", true);
        if (ITKSTATUS_OK != res)
            return res;
        res = ItkDevSetInt64(pCam->g_hCamera, "RegionWidth", width);
        if (ITKSTATUS_OK != res)
            return res;

        break;
    case ITEK_CAMERA_WIDTH_CONFIG_METHOD_2:
        res = ItkDevSetBool(pCam->g_hCamera, "ROIEnable", true);
        if (ITKSTATUS_OK != res)
            return res;
        res = ItkDevSetInt64(pCam->g_hCamera, "ROIWidth", width);
        if (ITKSTATUS_OK != res)
            return res;

        break;
    case ITEK_CAMERA_WIDTH_CONFIG_METHOD_3:
        res = ItkDevSetInt64(pCam->g_hCamera, "RegionWidth", width);
        if (ITKSTATUS_OK != res)
            return res;

        break;
    case ITEK_CAMERA_WIDTH_CONFIG_METHOD_4:
        res = ItkDevSetInt64(pCam->g_hCamera, "ROIWidth", width);
        if (ITKSTATUS_OK != res)
            return res;

        break;
    default:
        return ITKSTATUS_INVALID_ARG;
        break;
    }

    if (pCam->g_hBoard != INVALID_HANDLE_VALUE) {
        ret = IKapSetInfo(pCam->g_hBoard, IKP_IMAGE_WIDTH, (int)width);
        if (ret != IK_RTN_OK) {
            return ITKSTATUS_INVALID_ARG;
        }
    }
    return res;
}

ITKSTATUS GetWidth(pItkCamera pCam, int64_t* width)
{
    return ItkDevGetInt64(pCam->g_hCamera, "Width", width);
}

ITKSTATUS SetHeight(pItkCamera pCam, int64_t height)
{
    ITKSTATUS  res = ITKSTATUS_OK;
    int        ret = IK_RTN_OK;
    ITKFEATURE pFea = NULL;
    uint32_t   accMode = ITKFEATURE_VAL_ACCESS_MODE_NA;
    ItkDevGetAccessMode(pCam->g_hCamera, "Height", &accMode);
    if (ItkDevAllocFeature(pCam->g_hCamera, "Height", &pFea) == ITKSTATUS_OK && ITKFEATURE_VAL_ACCESS_MODE_RW == accMode) {
        return ItkDevSetInt64(pCam->g_hCamera, "Height", height);
    }

    switch (pCam->g_configMethod[ITEK_CAMERA_WIDTH_CONFIG]) {
    case ITEK_CAMERA_HEIGHT_CONFIG_METHOD_1:
        res = ItkDevSetBool(pCam->g_hCamera, "RegionEnable", true);
        if (ITKSTATUS_OK != res)
            return res;
        res = ItkDevSetInt64(pCam->g_hCamera, "RegionHeight", height);
        if (ITKSTATUS_OK != res)
            return res;

        break;
    case ITEK_CAMERA_HEIGHT_CONFIG_METHOD_2:
        res = ItkDevSetBool(pCam->g_hCamera, "ROIEnable", true);
        if (ITKSTATUS_OK != res)
            return res;
        res = ItkDevSetInt64(pCam->g_hCamera, "ROIHeight", height);
        if (ITKSTATUS_OK != res)
            return res;

        break;
    case ITEK_CAMERA_HEIGHT_CONFIG_METHOD_3:
        res = ItkDevSetInt64(pCam->g_hCamera, "RegionHeight", height);
        if (ITKSTATUS_OK != res)
            return res;

        break;
    case ITEK_CAMERA_HEIGHT_CONFIG_METHOD_4:
        res = ItkDevSetInt64(pCam->g_hCamera, "ROIHeight", height);
        if (ITKSTATUS_OK != res)
            return res;

        break;
    default:
        return ITKSTATUS_INVALID_ARG;
        break;
    }
    if (pCam->g_hBoard != INVALID_HANDLE_VALUE) {
        ret = IKapSetInfo(pCam->g_hBoard, IKP_IMAGE_HEIGHT, (int)height);
        if (ret != IK_RTN_OK) {
            return ITKSTATUS_INVALID_ARG;
        }
    }
    return res;
}

ITKSTATUS GetHeight(pItkCamera pCam, int64_t* height)
{
    return ItkDevGetInt64(pCam->g_hCamera, "Height", height);
}

ITKSTATUS SetPixelFormat(pItkCamera pCam, const char* pixelFormat)
{
    return ItkDevFromString(pCam->g_hCamera, "PixelFormat", pixelFormat);
}

ITKSTATUS GetPixelFormat(pItkCamera pCam, char* pBuffer, uint32_t* count)
{
    return ItkDevToString(pCam->g_hCamera, "PixelFormat", pBuffer, count);
}

ITKSTATUS SetExposureTime(pItkCamera pCam, double exposureTime)
{
    ITKSTATUS res = ITKSTATUS_OK;
    int64_t   exposureTime_int64 = (int64_t)exposureTime;
    bool      areaMode = false;
    uint32_t  ExposureTimeType = ITKFEATURE_VAL_TYPE_UNDEFINED;
    uint32_t  AreaExposureTimeType = ITKFEATURE_VAL_TYPE_UNDEFINED;
    if (ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_2 == pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG] ||
        ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_4 == pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG]) {
        res = ItkDevGetType(pCam->g_hCamera, "ExposureTime", &ExposureTimeType);
        if (ITKSTATUS_OK != res)
            return res;
    }
    if (ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_3 == pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG] ||
        ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_5 == pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG]) {
        res = ItkDevGetType(pCam->g_hCamera, "AreaExposureTime", &AreaExposureTimeType);
        if (ITKSTATUS_OK != res)
            return res;
    }

    switch (pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG]) {
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_1:
        return ItkDevSetInt64(pCam->g_hCamera, "ExposureTimeRaw", exposureTime_int64);
        break;
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_2:
        res = ItkDevGetBool(pCam->g_hCamera, "AreaMode", &areaMode);
        if (ITKSTATUS_OK != res)
            return res;
        if (false == areaMode)
            return ITKSTATUS_INVALID_ARG;

        if (ITKFEATURE_VAL_TYPE_INT64 == ExposureTimeType) {
            return ItkDevSetInt64(pCam->g_hCamera, "ExposureTime", exposureTime_int64);
        }
        else if (ITKFEATURE_VAL_TYPE_DOUBLE == ExposureTimeType) {
            return ItkDevSetDouble(pCam->g_hCamera, "ExposureTime", exposureTime);
        }
        break;
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_3:
        res = ItkDevGetBool(pCam->g_hCamera, "AreaMode", &areaMode);
        if (ITKSTATUS_OK != res)
            return res;
        if (false == areaMode)
            return ITKSTATUS_INVALID_ARG;

        if (ITKFEATURE_VAL_TYPE_INT64 == AreaExposureTimeType) {
            return ItkDevSetInt64(pCam->g_hCamera, "AreaExposureTime", exposureTime_int64);
        }
        else if (ITKFEATURE_VAL_TYPE_DOUBLE == ExposureTimeType) {
            return ItkDevSetDouble(pCam->g_hCamera, "AreaExposureTime", exposureTime);
        }
        break;
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_4:

        if (ITKFEATURE_VAL_TYPE_INT64 == ExposureTimeType) {
            return ItkDevSetInt64(pCam->g_hCamera, "ExposureTime", exposureTime_int64);
        }
        else if (ITKFEATURE_VAL_TYPE_DOUBLE == ExposureTimeType) {
            return ItkDevSetDouble(pCam->g_hCamera, "ExposureTime", exposureTime);
        }
        break;
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_5:
        res = ItkDevGetBool(pCam->g_hCamera, "Area", &areaMode);
        if (ITKSTATUS_OK != res)
            return res;
        if (false == areaMode)
            return ITKSTATUS_INVALID_ARG;

        if (ITKFEATURE_VAL_TYPE_INT64 == AreaExposureTimeType) {
            return ItkDevSetInt64(pCam->g_hCamera, "AreaExposureTime", exposureTime_int64);
        }
        else if (ITKFEATURE_VAL_TYPE_DOUBLE == ExposureTimeType) {
            return ItkDevSetDouble(pCam->g_hCamera, "AreaExposureTime", exposureTime);
        }
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS GetExposureTime(pItkCamera pCam, double* exposureTime)
{
    ITKSTATUS res = ITKSTATUS_OK;
    if (!exposureTime) {
        return ITKSTATUS_INVALID_ARG;
    }
    uint32_t ExposureTimeType = ITKFEATURE_VAL_TYPE_UNDEFINED;
    uint32_t AreaExposureTimeType = ITKFEATURE_VAL_TYPE_UNDEFINED;
    if (ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_2 == pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG] ||
        ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_4 == pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG]) {
        res = ItkDevGetType(pCam->g_hCamera, "ExposureTime", &ExposureTimeType);
        if (ITKSTATUS_OK != res)
            return res;
    }
    if (ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_3 == pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG] || ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_5 == pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG]) {
        res = ItkDevGetType(pCam->g_hCamera, "AreaExposureTime", &AreaExposureTimeType);
        if (ITKSTATUS_OK != res)
            return res;
    }

    int64_t expTime = 0;
    switch (pCam->g_configMethod[ITEK_CAMERA_EXPOSURE_TIME_CONFIG]) {
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_1:
        res = ItkDevGetInt64(pCam->g_hCamera, "ExposureTimeRaw", &expTime);
        if (res == ITKSTATUS_OK) {
            *exposureTime = (double)expTime;
        }
        break;
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_2:
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_4:
        if (ITKFEATURE_VAL_TYPE_INT64 == ExposureTimeType) {
            res = ItkDevGetInt64(pCam->g_hCamera, "ExposureTime", &expTime);
            if (res == ITKSTATUS_OK) {
                *exposureTime = (double)expTime;
            }
        }
        else if (ITKFEATURE_VAL_TYPE_DOUBLE == ExposureTimeType) {
            res = ItkDevGetDouble(pCam->g_hCamera, "ExposureTime", exposureTime);

        }
        else {
            return ITKSTATUS_INVALID_ARG;
        }
        break;
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_3:
    case ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_5:
        if (ITKFEATURE_VAL_TYPE_INT64 == AreaExposureTimeType) {
            res = ItkDevGetInt64(pCam->g_hCamera, "AreaExposureTime", &expTime);
            if (res == ITKSTATUS_OK) {
                *exposureTime = (double)expTime;
            }
        }
        else if (ITKFEATURE_VAL_TYPE_DOUBLE == AreaExposureTimeType) {
            res = ItkDevGetDouble(pCam->g_hCamera, "AreaExposureTime", exposureTime);
        }
        else {
            return ITKSTATUS_INVALID_ARG;
        }
        break;
    default:
        break;
    }

    return res;
}

ITKSTATUS SetDigitalGain(pItkCamera pCam, double digitalGain)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_DIGITAL_GAIN_CONFIG]) {
    case ITEK_CAMERA_DIGITAL_GAIN_CONFIG_METHOD_1:
        return ItkDevSetDouble(pCam->g_hCamera, "DigitalGain", digitalGain);
        ;
        break;
    case ITEK_CAMERA_DIGITAL_GAIN_CONFIG_METHOD_2:
        return ItkDevSetDouble(pCam->g_hCamera, "Gain", digitalGain);
        break;
    default:
        break;
    }

    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS GetDigitalGain(pItkCamera pCam, double* digitalGain)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_DIGITAL_GAIN_CONFIG]) {
    case ITEK_CAMERA_DIGITAL_GAIN_CONFIG_METHOD_1:
        return ItkDevGetDouble(pCam->g_hCamera, "DigitalGain", digitalGain);
        break;
    case ITEK_CAMERA_DIGITAL_GAIN_CONFIG_METHOD_2:
        return ItkDevGetDouble(pCam->g_hCamera, "Gain", digitalGain);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS SetPixelGain(pItkCamera pCam, const char* pixelGain)
{
    ITKSTATUS res = ITKSTATUS_OK;
    return ItkDevFromString(pCam->g_hCamera, "PixelGain", pixelGain);
}

ITKSTATUS GetPixelGain(pItkCamera pCam, char* pixelGain, uint32_t* len)
{
    ITKSTATUS res = ITKSTATUS_OK;
    return ItkDevToString(pCam->g_hCamera, "PixelGain", pixelGain, len);
}

ITKSTATUS SetLineRate(pItkCamera pCam, double lineRate)
{
    ITKSTATUS res = ITKSTATUS_OK;
    if (lineRate <= 0)
        return ITKSTATUS_INVALID_ARG;

    double linePeriodTime = 1000000 / lineRate;  // us
    switch (pCam->g_configMethod[ITEK_CAMERA_LINE_RATE_CONFIG]) {
    case ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_1:
        return ItkDevSetDouble(pCam->g_hCamera, "LinePeriodTime", linePeriodTime);
        break;
    case ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_2:
        return ItkDevSetDouble(pCam->g_hCamera, "AcquisitionLinePeriod", linePeriodTime);
        break;
    case ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_3:
        return ItkDevSetDouble(pCam->g_hCamera, "AcquisitionLineRate", lineRate);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS GetLineRate(pItkCamera pCam, double* lineRate)
{
    ITKSTATUS res = ITKSTATUS_OK;
    double    linePeriodTime = 0;
    switch (pCam->g_configMethod[ITEK_CAMERA_LINE_RATE_CONFIG]) {
    case ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_1:
        res = ItkDevGetDouble(pCam->g_hCamera, "LinePeriodTime", &linePeriodTime);
        if (ITKSTATUS_OK != res)
            return res;
        *lineRate = 1000000 / linePeriodTime;
        return res;
        break;
    case ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_2:
        res = ItkDevGetDouble(pCam->g_hCamera, "AcquisitionLinePeriod", &linePeriodTime);
        if (ITKSTATUS_OK != res)
            return res;
        *lineRate = 1000000 / linePeriodTime;
        return res;
        break;
    case ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_3:
        return ItkDevGetDouble(pCam->g_hCamera, "AcquisitionLineRate", lineRate);
        break;
    default:
        break;
    }

    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS SetFrameRate(pItkCamera pCam, double frameRate)
{
    ITKSTATUS res = ITKSTATUS_OK;
    if (frameRate <= 0) {
        return ITKSTATUS_INVALID_ARG;
    }
    double  framePeriod = 1000000 / frameRate;
    int64_t framePeriod_int64 = (int64_t)framePeriod;
    switch (pCam->g_configMethod[ITEK_CAMERA_FRAME_RATE_CONFIG]) {
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_1:
        return ItkDevSetInt64(pCam->g_hCamera, "AcquisitionFramePeriodRaw", framePeriod_int64);
        break;
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_2:
        return ItkDevSetInt64(pCam->g_hCamera, "AcquisitionFramePeriod", framePeriod_int64);
        break;
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_3:
        return ItkDevSetDouble(pCam->g_hCamera, "AcquisitionFramePeriod", framePeriod);
        break;
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_4:
        return ItkDevSetInt64(pCam->g_hCamera, "FramePeriodTime", framePeriod_int64);
        break;
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_5:
        res = ItkDevSetBool(pCam->g_hCamera, "AcquisitionFrameRateEnable", true);
        if (ITKSTATUS_OK != res)
            return res;
        return ItkDevSetDouble(pCam->g_hCamera, "AcquisitionFrameRate", frameRate);
        break;
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_6:
        return ItkDevSetDouble(pCam->g_hCamera, "AcquisitionFrameRate", frameRate);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS GetFrameRate(pItkCamera pCam, double* frameRate)
{
    ITKSTATUS res = ITKSTATUS_OK;
    int64_t   framePeriod_int64 = 0;
    double    framePeriod = 0;
    switch (pCam->g_configMethod[ITEK_CAMERA_FRAME_RATE_CONFIG]) {
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_1:
        res = ItkDevGetInt64(pCam->g_hCamera, "AcquisitionFramePeriodRaw", &framePeriod_int64);
        if (framePeriod_int64 > 0) {
            *frameRate = (double)1000000 / (double)framePeriod_int64;
        }
        return res;
        break;
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_2:
        res = ItkDevGetInt64(pCam->g_hCamera, "AcquisitionFramePeriod", &framePeriod_int64);
        if (framePeriod_int64 > 0) {
            *frameRate = (double)1000000 / (double)framePeriod_int64;
        }
        return res;
        break;
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_3:
        res = ItkDevGetDouble(pCam->g_hCamera, "AcquisitionFramePeriod", &framePeriod);
        if (framePeriod > 0) {
            *frameRate = 1000000 / framePeriod;
        }
        return res;
        break;
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_4:
        res = ItkDevGetInt64(pCam->g_hCamera, "FramePeriodTime", &framePeriod_int64);
        if (framePeriod_int64 > 0) {
            *frameRate = (double)1000000 / (double)framePeriod_int64;
        }
        return res;
        break;
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_5:
    case ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_6:
        return ItkDevGetDouble(pCam->g_hCamera, "AcquisitionFrameRate", frameRate);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS SetFrameTrigger(pItkCamera pCam, const char* status, bool level)
{
    ITKSTATUS res = ITKSTATUS_OK;
    bool      bSet = false;
    if (strcmp(status, "On") == 0) {
        bSet = true;
    }
    if (level) {
        switch (pCam->g_configMethod[ITEK_CAMERA_FRAME_TRIGGER_CONFIG]) {
        case ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_1:
            res = ItkDevFromString(pCam->g_hCamera, "TriggerSelector", "FrameActive");
            if (ITKSTATUS_OK != res)
                return res;
            return ItkDevFromString(pCam->g_hCamera, "TriggerMode", status);
            break;
        case ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_2:
            res = ItkDevSetBool(pCam->g_hCamera, "FrameTriggerEnable", bSet);
            if (ITKSTATUS_OK != res)
                return res;
            return ItkDevFromString(pCam->g_hCamera, "FrameTriggerType", "LevelSensitive");
            break;
        case ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_3:
            if (bSet) {
                return ItkDevFromString(pCam->g_hCamera, "ExposureMode", "TriggerWidth");
            }
            else {
                return ItkDevFromString(pCam->g_hCamera, "ExposureMode", "Timed");
            }
            break;
        default:
            break;
        }
    }
    else {
        switch (pCam->g_configMethod[ITEK_CAMERA_FRAME_TRIGGER_CONFIG]) {
        case ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_1:
            res = ItkDevFromString(pCam->g_hCamera, "TriggerSelector", "FrameStart");
            if (ITKSTATUS_OK != res)
                return res;
            return ItkDevFromString(pCam->g_hCamera, "TriggerMode", status);
            break;
        case ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_2:
            res = ItkDevSetBool(pCam->g_hCamera, "FrameTriggerEnable", bSet);
            if (ITKSTATUS_OK != res)
                return res;
            return ItkDevFromString(pCam->g_hCamera, "FrameTriggerType", "EdgeSensitive");
            break;
        case ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_3:
            if (bSet) {
                return ItkDevFromString(pCam->g_hCamera, "ExposureMode", "TriggerPulse");
            }
            else {
                return ItkDevFromString(pCam->g_hCamera, "ExposureMode", "Timed");
            }
            break;
        default:
            break;
        }
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS SetLineTrigger(pItkCamera pCam, const char* status, bool level)
{
    ITKSTATUS res = ITKSTATUS_OK;
    bool      bSet = false;
    if (strcmp(status, "On") == 0) {
        bSet = true;
    }
    if (level) {
        switch (pCam->g_configMethod[ITEK_CAMERA_LINE_TRIGGER_CONFIG]) {
        case ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_1:
            res = ItkDevFromString(pCam->g_hCamera, "TriggerSelector", "LineStart");
            if (ITKSTATUS_OK != res)
                return res;
        case ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_2:
            res = ItkDevFromString(pCam->g_hCamera, "TriggerMode", status);
            if (ITKSTATUS_OK != res)
                return res;
            if (bSet) {
                return ItkDevFromString(pCam->g_hCamera, "ExposureMode", "TriggerWidth");
            }
            else {
                return ItkDevFromString(pCam->g_hCamera, "ExposureMode", "Timed");
            }
            break;
        case ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_3:
            if (bSet) {
                return ItkDevFromString(pCam->g_hCamera, "SynchronizationMode", "TriggerWidth");
            }
            else {
                return ItkDevFromString(pCam->g_hCamera, "SynchronizationMode", "Timed");
            }
            break;
        default:
            break;
        }
    }
    else {
        switch (pCam->g_configMethod[ITEK_CAMERA_LINE_TRIGGER_CONFIG]) {
        case ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_1:
            res = ItkDevFromString(pCam->g_hCamera, "TriggerSelector", "LineStart");
            if (ITKSTATUS_OK != res)
                return res;
        case ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_2:
            res = ItkDevFromString(pCam->g_hCamera, "TriggerMode", status);
            if (ITKSTATUS_OK != res)
                return res;
            return ItkDevFromString(pCam->g_hCamera, "ExposureMode", "Timed");
            break;
        case ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_3:
            if (bSet) {
                return ItkDevFromString(pCam->g_hCamera, "SynchronizationMode", "TriggerPulse");
            }
            else {
                return ItkDevFromString(pCam->g_hCamera, "SynchronizationMode", "Timed");
            }
            break;
        default:
            break;
        }
    }
    return ITKSTATUS_INVALID_ARG;
}

// ITKSTATUS SetTriggerMode(pItkCamera pCam, const char *triggerSelector,
//                         const char *exposureMode) {
//  ITKSTATUS res = ITKSTATUS_OK;
//  if (triggerSelector) {
//    res = ItkDevFromString(pCam->g_hCamera, "TriggerSelector", triggerSelector);
//    if (res != ITKSTATUS_OK) {
//      return res;
//    }
//  }
//  res = ItkDevFromString(pCam->g_hCamera, "TriggerMode", "On");
//  if (ITKSTATUS_OK == res)
//    return res;
//  res = ItkDevSetBool(pCam->g_hCamera, "FrameTriggerEnable", true);
//  if (ITKSTATUS_OK == res)
//	  return res;
//  res = ItkDevFromString(pCam->g_hCamera, "ExposureMode", exposureMode);
//  if (ITKSTATUS_OK == res)
//	  return res;
//  return ItkDevFromString(pCam->g_hCamera, "SynchronizationMode", exposureMode);
//}
//
// ITKSTATUS GetTriggerMode(pItkCamera pCam, char* triggerSelector, uint32_t* triggerSelectorLen, char*
// TriggerModeOrexposureMode, uint32_t* Len, bool* frameTriggerEnable)
//{
//	ITKSTATUS res = ITKSTATUS_OK;
//	if (triggerSelector)
//	{
//          res = ItkDevToString(pCam->g_hCamera, "TriggerSelector",
//                               triggerSelector, triggerSelectorLen);
//		  if (res!=ITKSTATUS_OK)
//		  {
//			  return res;
//		  }
//	}
//	res = ItkDevToString(pCam->g_hCamera, "TriggerMode", TriggerModeOrexposureMode,Len);
//	if (res==ITKSTATUS_OK)
//	{
//		return res;
//	}
//	if (frameTriggerEnable)
//	{
//	if (ItkDevGetBool(pCam->g_hCamera, "FrameTriggerEnable", frameTriggerEnable)==ITKSTATUS_OK) {
//		return ITKSTATUS_OK;
//	}
//	}
//
//	if (ItkDevToString(pCam->g_hCamera,"ExposureMode", TriggerModeOrexposureMode,Len) == ITKSTATUS_OK ||
// ItkDevToString(pCam->g_hCamera, "SynchronizationMode", TriggerModeOrexposureMode, Len) == ITKSTATUS_OK)
//	{
//		return ITKSTATUS_OK;
//	}
//	return ITKSTATUS_INVALID_ARG;
//}

ITKSTATUS SetTriggerSource(pItkCamera pCam, const char* triggerSource)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_TRIGGER_SOURCE_CONFIG]) {
    case ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_1:
        return ItkDevFromString(pCam->g_hCamera, "FrameTriggerSource", triggerSource);
        break;
    case ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_2:
        return ItkDevFromString(pCam->g_hCamera, "ExternalTriggerSource", triggerSource);
        break;
    case ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_3:
        return ItkDevFromString(pCam->g_hCamera, "TriggerSource", triggerSource);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS GetTriggerSource(pItkCamera pCam, char* triggerSource, uint32_t* len)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_TRIGGER_SOURCE_CONFIG]) {
    case ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_1:
        return ItkDevToString(pCam->g_hCamera, "FrameTriggerSource", triggerSource, len);
        break;
    case ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_2:
        return ItkDevToString(pCam->g_hCamera, "ExternalTriggerSource", triggerSource, len);
        break;
    case ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_3:
        return ItkDevToString(pCam->g_hCamera, "TriggerSource", triggerSource, len);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS SetTriggerPolarity(pItkCamera pCam, const char* triggerPolarity)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_TRIGGER_POLARITY_CONFIG]) {
    case ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_1:
        return ItkDevFromString(pCam->g_hCamera, "FrameTriggerPolarity", triggerPolarity);
        break;
    case ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_2:
        return ItkDevFromString(pCam->g_hCamera, "ExternalTriggerActivation", triggerPolarity);
        break;
    case ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_3:
        return ItkDevFromString(pCam->g_hCamera, "LinePolarity", triggerPolarity);
        break;
    case ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_4:
        return ItkDevFromString(pCam->g_hCamera, "TriggerActivation", triggerPolarity);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS GetTriggerPolarity(pItkCamera pCam, char* triggerPolarity, uint32_t* len)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_TRIGGER_POLARITY_CONFIG]) {
    case ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_1:
        return ItkDevToString(pCam->g_hCamera, "FrameTriggerPolarity", triggerPolarity, len);
        break;
    case ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_2:
        return ItkDevToString(pCam->g_hCamera, "ExternalTriggerActivation", triggerPolarity, len);
        break;
    case ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_3:
        return ItkDevToString(pCam->g_hCamera, "LinePolarity", triggerPolarity, len);
        break;
    case ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_4:
        return ItkDevToString(pCam->g_hCamera, "TriggerActivation", triggerPolarity, len);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS SetMultiExposureTime(pItkCamera pCam, char* multiExposureTime)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_MULTI_EXPOSURE_CONFIG]) {
    case ITEK_CAMERA_MULTI_EXPOSURE_CONFIG_METHOD_1:
        return ItkDevFromString(pCam->g_hCamera, "MultiExposureTimeCount", multiExposureTime);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS GetMultiExposureTime(pItkCamera pCam, char* multiExposureTime, uint32_t* bufLen)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_MULTI_EXPOSURE_CONFIG]) {
    case ITEK_CAMERA_MULTI_EXPOSURE_CONFIG_METHOD_1:
        return ItkDevToString(pCam->g_hCamera, "MultiExposureTimeCount", multiExposureTime, bufLen);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS SetChunkData(pItkCamera pCam, bool benable)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_CHUNK_DATA_CONFIG]) {
    case ITEK_CAMERA_CHUNK_DATA_CONFIG_METHOD_1: {
        res = ItkDevSetBool(pCam->g_hCamera, "ChunkModeActive", benable);
        if (res != ITKSTATUS_OK)
        {
            return res;
        }
        if (benable) {

            uint32_t enumCount = 0;
            res = ItkDevGetEnumCount(pCam->g_hCamera, "ChunkSelector",
                &enumCount);
            if (res != ITKSTATUS_OK) {
                return res;
            }
            if (enumCount == 0) {
                return ITKSTATUS_INVALID_ARG;
            }
            for (int i = 0; i < (int)enumCount; i++) {
                char enumstr[128] = { 0 };
                uint32_t bufLen = sizeof(enumstr);
                res = ItkDevGetEnumString(pCam->g_hCamera, "ChunkSelector", i,
                    enumstr, &bufLen);
                if (res != ITKSTATUS_OK) {
                    return res;
                }
                res =
                    ItkDevFromString(pCam->g_hCamera, "ChunkSelector", enumstr);
                if (res != ITKSTATUS_OK) {
                    return res;
                }
                res = ItkDevSetBool(pCam->g_hCamera, "ChunkEnable", true);
                if (res != ITKSTATUS_OK) {
                    return res;
                }
            }
        }

    }
                                               break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}

ITKSTATUS GetChunkData(pItkCamera pCam, bool* pbenable)
{
    ITKSTATUS res = ITKSTATUS_OK;
    switch (pCam->g_configMethod[ITEK_CAMERA_CHUNK_DATA_CONFIG]) {
    case ITEK_CAMERA_CHUNK_DATA_CONFIG_METHOD_1:
        return ItkDevGetBool(pCam->g_hCamera, "ChunkModeActive", pbenable);
        break;
    default:
        break;
    }
    return ITKSTATUS_INVALID_ARG;
}



