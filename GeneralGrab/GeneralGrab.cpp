#include "GeneralGrab.h"

#include "../GeneralGrabWithGrabber/GeneralGrabWithGrabber.h"

void SetSoftTriggerWithoutGrabber(pItkCamera pCam)
{
    ITKSTATUS res = ITKSTATUS_OK;
    if (pCam->g_bSoftTriggerUsed) {
        res = ItkDevFromString(pCam->g_hCamera, "TriggerSelector", "FrameStart");
        res = ItkDevFromString(pCam->g_hCamera, "TriggerMode", "On");
        res = ItkDevFromString(pCam->g_hCamera, "TriggerSource", "Software");
    } else {
        res = ItkDevFromString(pCam->g_hCamera, "TriggerSelector", "FrameStart");
        res = ItkDevFromString(pCam->g_hCamera, "TriggerMode", "Off");
    }
}
/// \~chinese �����������ͻ�����				\~english Create data stream and buffer
void CreateStreamAndBuffer(pItkCamera pCam)
{
    ITKSTATUS res = ITKSTATUS_OK;

    /// \~chinese  ����������				     \~english The number of data stream
    uint32_t streamCount = 0;

    /// \~chinese  ��ȡ����������			     \~english Get the number of data stream
    res = ItkDevGetStreamCount(pCam->g_hCamera, &streamCount);
    CHECK(res);

    if (streamCount == 0) {
        fprintf(stderr, "Camera does not have image stream channel.");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    /// \~chinese  ������������Դ		            \~english Allocate data stream source
    res = ItkDevAllocStreamEx(pCam->g_hCamera, 0, pCam->g_bufferCount, &pCam->g_hStream);
    CHECK(res);

    ITKBUFFER hBuffer = NULL;

    /// \~chinese  ��ȡ���Ϊ0��buffer		            \~english get buffer of index 0
    res = ItkStreamGetBuffer(pCam->g_hStream, 0, &hBuffer);
    CHECK(res);

    /// \~chinese  ��ȡbuffer����Ϣ		            \~english get buffer info
    ITK_BUFFER_INFO bufferInfo = { 0 };
    res = ItkBufferGetInfo(hBuffer, &bufferInfo);
    CHECK(res);

    /// \~chinese  �������������ݴ洢		       \~english Create buffer data saving
    pCam->g_bufferData = (char*)malloc(bufferInfo.ImageSize);
    if (pCam->g_bufferData == NULL) {
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

	/***************************/
	/// \~chinese  չʾ���ʹ���û�������ڴ��ַ��ΪBuffer���ڴ��ַ��ע�ⲻҪ�����ͷŸ��ڴ�	       \~english Show how to using the memory address as the memory address of Buffer,be careful not to forget to release the memory
	/// \~chinese  ������������Buffer��ַ���ڴ�		       \~english Create the memory that the user requests for setting the Buffer address
	/*pCam->g_user_buffer = (char*)malloc(bufferInfo.TotalSize);
	if (pCam->g_user_buffer == NULL)
	{
		pressEnterToExit();
		exit(EXIT_FAILURE);
	}*/

	// \~chinese �����Ϊ0��Buffer���ڴ��ַ��Ϊ�û�����Ĵ�С���ʵ��ڴ��ַ�����Ϊ1~g_bufferCount-1��Bufferͬ��		\~english The memory address of Buffer with index number 0 is changed to the appropriate size memory address applied by the user, just as with Buffer with index number 1~g_bufferCount-1.
	/*res = ItkBufferSetPrm(hBuffer, ITKBUFFER_PRM_ADDRESS, &pCam->g_user_buffer);
	CHECK(res);*/
	/***************************/

}



