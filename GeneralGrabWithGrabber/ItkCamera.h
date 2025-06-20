#pragma once

#include <malloc.h>
#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "IKapBoard.h"
#include "IKapC.h"

#define ITEK_CAMERA_WIDTH_CONFIG            0
#define ITEK_CAMERA_HEIGHT_CONFIG           1
#define ITEK_CAMERA_PIXEL_FORMAT_CONFIG     2
#define ITEK_CAMERA_EXPOSURE_TIME_CONFIG    3
#define ITEK_CAMERA_EXPOSURE_MODE_CONFIG    4
#define ITEK_CAMERA_DIGITAL_GAIN_CONFIG     5
#define ITEK_CAMERA_PIXEL_GAIN_CONFIG       6
#define ITEK_CAMERA_LINE_RATE_CONFIG        7
#define ITEK_CAMERA_FRAME_RATE_CONFIG       8
#define ITEK_CAMERA_LINE_TRIGGER_CONFIG     9
#define ITEK_CAMERA_FRAME_TRIGGER_CONFIG    10
#define ITEK_CAMERA_TRIGGER_SOURCE_CONFIG   11
#define ITEK_CAMERA_TRIGGER_POLARITY_CONFIG 12
#define ITEK_CAMERA_MULTI_EXPOSURE_CONFIG   13
#define ITEK_CAMERA_CHUNK_DATA_CONFIG       14
#define ITEK_CAMERA_CONFIG_TOTAL_COUNT      15

#define ITEK_CAMERA_WIDTH_CONFIG_METHOD_1 1  // RegionEnable ReginWidth
#define ITEK_CAMERA_WIDTH_CONFIG_METHOD_2 2  // ROIEnable ROIWidth
#define ITEK_CAMERA_WIDTH_CONFIG_METHOD_3 3  // ReginWidth
#define ITEK_CAMERA_WIDTH_CONFIG_METHOD_4 4  // ROIWidth

#define ITEK_CAMERA_HEIGHT_CONFIG_METHOD_1 1  // RegionEnable ReginHeight
#define ITEK_CAMERA_HEIGHT_CONFIG_METHOD_2 2  // ROIEnable ROIHeight
#define ITEK_CAMERA_HEIGHT_CONFIG_METHOD_3 3  // ReginHeight
#define ITEK_CAMERA_HEIGHT_CONFIG_METHOD_4 4  // ROIHeight

#define ITEK_CAMERA_PIXEL_FORMAT_CONFIG_METHOD_1 1

#define ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_1 1  // ExposureTimeRaw
#define ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_2 2  // AreaMode (true) ExposureTime AreaExposureTime
#define ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_3 3  // AreaMode (true) AreaExposureTime
#define ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_4 4  // ExposureTime
#define ITEK_CAMERA_EXPOSURE_TIME_CONFIG_METHOD_5 5  // Area (true) AreaExposureTime

#define ITEK_CAMERA_EXPOSURE_MODE_CONFIG_METHOD_1 1
#define ITEK_CAMERA_EXPOSURE_MODE_CONFIG_METHOD_2 2

#define ITEK_CAMERA_DIGITAL_GAIN_CONFIG_METHOD_1 1  // Digital Gain
#define ITEK_CAMERA_DIGITAL_GAIN_CONFIG_METHOD_2 2  // Gain

#define ITEK_CAMERA_PIXEL_GAIN_CONFIG_METHOD_1 1

#define ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_1 1  // LinePeriodTime
#define ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_2 2  // AcquisitionLinePeriod
#define ITEK_CAMERA_LINE_RATE_CONFIG_METHOD_3 3  // AcquisitionLineRate

#define ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_1 1  // AcquisitionFramePeriodRaw
#define ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_2 2  // AcquisitionFramePeriod(int64)
#define ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_3 3  // AcquisitionFramePeriod(double)
#define ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_4 4  // FramePeriodTime
#define ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_5 5  // AcquisitionFrameRateEnable AcquisitionFrameRate
#define ITEK_CAMERA_FRAME_RATE_CONFIG_METHOD_6 6  // AcquisitionFrameRate

#define ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_1 1  // TriggerSelector TriggerMode ExposureMode
#define ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_2 2  // TriggerMode ExposureMode
#define ITEK_CAMERA_LINE_TRIGGER_CONFIG_METHOD_3 3  // SynchronizationMode

#define ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_1 1  // TriggerSelector TriggerMode ExposureMode
#define ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_2 2  // FrameTriggerEnable
#define ITEK_CAMERA_FRAME_TRIGGER_CONFIG_METHOD_3 3  // ExposureMode

#define ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_1 1  // FrameTriggerSource
#define ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_2 2  // ExternalTriggerSource
#define ITEK_CAMERA_TRIGGER_SOURCE_CONFIG_METHOD_3 3  // TriggerSource

#define ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_1 1  // FrameTriggerPolarity
#define ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_2 2  // ExternalTriggerActivation
#define ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_3 3  // LinePolarity
#define ITEK_CAMERA_TRIGGER_POLARITY_CONFIG_METHOD_4 4  // TriggerActivation

#define ITEK_CAMERA_MULTI_EXPOSURE_CONFIG_METHOD_1 1 // MultiExposureTimeCount

#define ITEK_CAMERA_CHUNK_DATA_CONFIG_METHOD_1 1 // ChunkModeActive ChunkSelector ChunkEnable

typedef struct _ItkCamera {
    /// \~chinese �������÷�ʽ�б�	\~english List of method of parameter configure
    uint32_t g_configMethod[ITEK_CAMERA_CONFIG_TOTAL_COUNT];

    /// \~chinese ͼ�񻺳��������֡��				\~english The number of frames requested by buffer
    int g_bufferCount = 5;

    /// \~chinese ����ͼ����ļ���					\~english File name of image
    char g_saveFileName[128];

    /// \~chinese Ҫ����������к�					\~english SerialNumber of camera to open
    const char *g_SerialNumber = "111";

    /// \~chinese ϣ���ɼ���֡��						\~english Number of frames wanted
    uint32_t g_grabCount = ITKSTREAM_CONTINUOUS;

    /// \~chinese �Ƿ�������						\~english Whether enable softTrigger or not
    uint8_t g_bSoftTriggerUsed = 0;

    /// \~chinese �Ƿ���زɼ��������ļ�				\~english Whether load grabber configure file or not
    uint8_t g_bLoadGrabberConfig = 0;

    /// \~chinese ����豸���						\~english Camera device handle
    ITKDEVICE g_hCamera = NULL;

    /// \~chinese ���������						\~english Data stream handle
    ITKSTREAM g_hStream = NULL;

    /// \~chinese ����豸��Ϣ						\~english Camera device info
    ITKDEV_INFO g_devInfo;

    /// \~chinese �ɼ����豸���						\~english Frame grabber device handle
    HANDLE g_hBoard = INVALID_HANDLE_VALUE;

    /// \~chinese ����������						\~english Buffer data
    char *g_bufferData = NULL;

	/// \~chinese �û������Buffer�ڴ�						\~english The Buffer of memory requested by the user
	//char *g_user_buffer = NULL;

    /// \~chinese �Ƿ���chunkdata						\~english enable chunkdata or not
    bool benableChunkData = false;

    /// \~chinese ������						\~english Index of Camera
    int g_index = -1;
} ItkCamera, *pItkCamera;
