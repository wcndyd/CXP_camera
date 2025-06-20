#include "GeneralGrabWithGrabber.h"

void SetSoftTriggerWithGrabber(pItkCamera pCam)
{
    ITKSTATUS res = ITKSTATUS_OK;
    int       ret = IK_RTN_OK;
    if (pCam->g_bSoftTriggerUsed) {
        if (strcmp(pCam->g_devInfo.DeviceClass, "CoaXPress") == 0) {
            res = ItkDevSetDouble(pCam->g_hCamera, "ExposureTime", 80);
            res = ItkDevFromString(pCam->g_hCamera, "TriggerMode", "On");
            res = ItkDevFromString(pCam->g_hCamera, "ExposureMode", "TriggerPulse");
            ret = IKapSetInfo(pCam->g_hBoard, IKP_IMAGE_HEIGHT, 1000);
            CHECK_IKAPBOARD(ret);

            ret = IKapSetInfo(pCam->g_hBoard, IKP_CXP_TRIGGER_OUTPUT_SELECTOR, 1);  // Integration Signal1
            CHECK_IKAPBOARD(ret);

            ret = IKapSetInfo(pCam->g_hBoard, IKP_INTEGRATION_TRIGGER_SOURCE, 9);  // Software Trigger
            CHECK_IKAPBOARD(ret);

            ret = IKapSetInfo(pCam->g_hBoard, IKP_INTEGRATION_METHOD, 4);  // Method5
            CHECK_IKAPBOARD(ret);

            ret = IKapSetInfo(pCam->g_hBoard, IKP_INTEGRATION_PARAM2, 5000);
            CHECK_IKAPBOARD(ret);

            ret = IKapSetInfo(pCam->g_hBoard, IKP_CXP_FRAME_BURST_COUNT, 1050);
            CHECK_IKAPBOARD(ret);

            ret = IKapSetInfo(pCam->g_hBoard, IKP_CXP_FRAME_BURST_PERIOD, 100000);
            CHECK_IKAPBOARD(ret);

            ret = IKapSetInfo(pCam->g_hBoard, IKP_SOFTWARE_TRIGGER_WIDTH, 5000);
            CHECK_IKAPBOARD(ret);

        } else if (strcmp(pCam->g_devInfo.DeviceClass, "CameraLink") == 0) {
            res = ItkDevFromString(pCam->g_hCamera, "SynchronizationMode", "ExternalPulse");
            res = ItkDevFromString(pCam->g_hCamera, "InputLineTriggerSource", "CC1");  // InputLineTriggerSource
            res = ItkDevSetDouble(pCam->g_hCamera, "LinePeriodTime", 50);
            ret = IKapSetInfo(pCam->g_hBoard, IKP_IMAGE_HEIGHT, 2000);
            ret = IKapSetInfo(pCam->g_hBoard, IKP_CC1_SOURCE, 5);                  // Software Trigger
            ret = IKapSetInfo(pCam->g_hBoard, IKP_SOFTWARE_TRIGGER_SYNC_MODE, 0);  // Off
            ret = IKapSetInfo(pCam->g_hBoard, IKP_SOFTWARE_TRIGGER_PERIOD, 80);    // softTriggerPeriod
            ret = IKapSetInfo(pCam->g_hBoard, IKP_SOFTWARE_TRIGGER_COUNT, 2050);   // softTriggerCount

        } else {
            res = ItkDevFromString(pCam->g_hCamera, "TriggerSelector", "FrameStart");
            res = ItkDevFromString(pCam->g_hCamera, "TriggerMode", "On");
            res = ItkDevFromString(pCam->g_hCamera, "TriggerSource", "Software");
        }
    } else {
        if (strcmp(pCam->g_devInfo.DeviceClass, "CoaXPress") == 0) {
            res = ItkDevFromString(pCam->g_hCamera, "ExposureMode", "Timed");
        } else if (strcmp(pCam->g_devInfo.DeviceClass, "CameraLink") == 0) {
            res = ItkDevFromString(pCam->g_hCamera, "SynchronizationMode", "InternalFreeRun");
        } else {
            res = ItkDevFromString(pCam->g_hCamera, "TriggerSelector", "FrameStart");
            res = ItkDevFromString(pCam->g_hCamera, "TriggerMode", "Off");
        }
    }
}

void ConfigureFrameGrabber(pItkCamera pCam)
{
    ITKSTATUS res = ITKSTATUS_OK;
    int       ret = IK_RTN_OK;
    char      configFilename[MAX_PATH];

    if (pCam->g_bLoadGrabberConfig) {
        if (GetOption(configFilename) == false) {
            printf("Fail to get configuration, using default setting!\n");
        } else {
            ret = IKapLoadConfigurationFromFile(pCam->g_hBoard, configFilename);
            CHECK_IKAPBOARD(ret);
        }
    }

    ret = IKapSetInfo(pCam->g_hBoard, IKP_FRAME_COUNT, pCam->g_bufferCount);
    CHECK_IKAPBOARD(ret);

    int timeout = -1;
    ret         = IKapSetInfo(pCam->g_hBoard, IKP_TIME_OUT, timeout);
    CHECK_IKAPBOARD(ret);

    int grab_mode = IKP_GRAB_NON_BLOCK;
    ret           = IKapSetInfo(pCam->g_hBoard, IKP_GRAB_MODE, grab_mode);
    CHECK_IKAPBOARD(ret);

    int transfer_mode = IKP_FRAME_TRANSFER_SYNCHRONOUS_NEXT_EMPTY_WITH_PROTECT;
    ret               = IKapSetInfo(pCam->g_hBoard, IKP_FRAME_TRANSFER_MODE, transfer_mode);
    CHECK_IKAPBOARD(ret);


	/***************************/
	/// \~chinese  展示如果使用用户申请的内存地址作为Buffer的内存地址，注意不要忘记释放该内存	       \~english Show how to using the memory address as the memory address of Buffer,be careful not to forget to release the memory
	/// \~chinese  创建用于设置Buffer地址的内存		       \~english Create the memory that the user requests for setting the Buffer address

	/*int              nFrameSize = 0;
	ret = IKapGetInfo(pCam->g_hBoard, IKP_FRAME_SIZE, &nFrameSize);
	CHECK_IKAPBOARD(ret);

	pCam->g_user_buffer = (char*)malloc(nFrameSize);
	if (pCam->g_user_buffer == NULL)
	{
	pressEnterToExit();
	exit(EXIT_FAILURE);
	}*/

	// \~chinese 将序号为0的Buffer的内存地址改为用户申请的大小合适的内存地址，序号为1~g_bufferCount-1的Buffer同理。		\~english The memory address of Buffer with index number 0 is changed to the appropriate size memory address applied by the user, just as with Buffer with index number 1~g_bufferCount-1.
	/*ret = IKapSetBufferAddress(pCam->g_hBoard, 0, pCam->g_user_buffer);
	CHECK_IKAPBOARD(ret);*/
	/***************************/
}

void StartGrabImage(pItkCamera pCam)
{
    ITKSTATUS res = ITKSTATUS_OK;
    int       ret = IK_RTN_OK;

    if (strcmp(pCam->g_devInfo.DeviceClass, "CameraLink") != 0) {
        res = ItkDevExecuteCommand(pCam->g_hCamera, "AcquisitionStop");
        CHECK_IKAPC(res);
    }

    ret = IKapStartGrab(pCam->g_hBoard, 0);
    CHECK_IKAPBOARD(ret);

    if (strcmp(pCam->g_devInfo.DeviceClass, "CameraLink") != 0) {
        res = ItkDevExecuteCommand(pCam->g_hCamera, "AcquisitionStart");
        CHECK_IKAPC(res);
    }
}

/// \~chinese 选择用户配置文件				                \~english Select configuration file
bool GetOption(char* lpConfigFileName)
{
    OPENFILENAME iniofn;
    wchar_t      gcMsgBuf[MAX_PATH]        = _T("\0");
    wchar_t      lpTempFileName[_MAX_PATH] = _T("\0");

    GetModuleFileName(NULL, gcMsgBuf, MAX_PATH);
    PathRemoveFileSpec(gcMsgBuf);

    iniofn.lStructSize       = sizeof(OPENFILENAME);
    iniofn.hwndOwner         = NULL;
    iniofn.lpstrFilter       = _T("Configuration File(*.vlcf)\0*.vlcf\0");
    iniofn.lpstrCustomFilter = NULL;
    iniofn.nFilterIndex      = 1;
    iniofn.lpstrFile         = lpTempFileName;
    iniofn.nMaxFile          = MAX_PATH;
    iniofn.lpstrFileTitle    = NULL;
    iniofn.nMaxFileTitle     = MAX_PATH;
    iniofn.lpstrInitialDir   = gcMsgBuf;
    iniofn.lpstrTitle        = _T("The 'Configuration File' of the purpose read is chosen.");
    iniofn.Flags             = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    iniofn.lpstrDefExt       = _T("vlcf");

    if (GetOpenFileName(&iniofn) == FALSE)
        return false;

    WideCharToMultiByte(
        CP_OEMCP, 0, lpTempFileName, (int)_tcsclen(lpTempFileName) + 1, lpConfigFileName, _MAX_PATH, NULL, NULL);

    return true;
}