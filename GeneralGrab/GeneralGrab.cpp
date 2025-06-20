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
/// \~chinese 创建数据流和缓冲区				\~english Create data stream and buffer
void CreateStreamAndBuffer(pItkCamera pCam)
{
    ITKSTATUS res = ITKSTATUS_OK;

    /// \~chinese  数据流数量				     \~english The number of data stream
    uint32_t streamCount = 0;

    /// \~chinese  获取数据流数量			     \~english Get the number of data stream
    res = ItkDevGetStreamCount(pCam->g_hCamera, &streamCount);
    CHECK(res);

    if (streamCount == 0) {
        fprintf(stderr, "Camera does not have image stream channel.");
        ItkManTerminate();
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

    /// \~chinese  申请数据流资源		            \~english Allocate data stream source
    res = ItkDevAllocStreamEx(pCam->g_hCamera, 0, pCam->g_bufferCount, &pCam->g_hStream);
    CHECK(res);

    ITKBUFFER hBuffer = NULL;

    /// \~chinese  获取序号为0的buffer		            \~english get buffer of index 0
    res = ItkStreamGetBuffer(pCam->g_hStream, 0, &hBuffer);
    CHECK(res);

    /// \~chinese  获取buffer的信息		            \~english get buffer info
    ITK_BUFFER_INFO bufferInfo = { 0 };
    res = ItkBufferGetInfo(hBuffer, &bufferInfo);
    CHECK(res);

    /// \~chinese  创建缓冲区数据存储		       \~english Create buffer data saving
    pCam->g_bufferData = (char*)malloc(bufferInfo.ImageSize);
    if (pCam->g_bufferData == NULL) {
        pressEnterToExit();
        exit(EXIT_FAILURE);
    }

	/***************************/
	/// \~chinese  展示如果使用用户申请的内存地址作为Buffer的内存地址，注意不要忘记释放该内存	       \~english Show how to using the memory address as the memory address of Buffer,be careful not to forget to release the memory
	/// \~chinese  创建用于设置Buffer地址的内存		       \~english Create the memory that the user requests for setting the Buffer address
	/*pCam->g_user_buffer = (char*)malloc(bufferInfo.TotalSize);
	if (pCam->g_user_buffer == NULL)
	{
		pressEnterToExit();
		exit(EXIT_FAILURE);
	}*/

	// \~chinese 将序号为0的Buffer的内存地址改为用户申请的大小合适的内存地址，序号为1~g_bufferCount-1的Buffer同理。		\~english The memory address of Buffer with index number 0 is changed to the appropriate size memory address applied by the user, just as with Buffer with index number 1~g_bufferCount-1.
	/*res = ItkBufferSetPrm(hBuffer, ITKBUFFER_PRM_ADDRESS, &pCam->g_user_buffer);
	CHECK(res);*/
	/***************************/

}



