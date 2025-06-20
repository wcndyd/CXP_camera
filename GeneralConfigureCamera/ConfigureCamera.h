#pragma once
#include "../GeneralGrabWithGrabber/ItkCamera.h"
#include "../GeneralGrabWithGrabber/util.h"

/* @brief：配置相机设备。
 * @param[in out] pCam：相机结构体指针。
 * @param[in] index：要打开相机的序号。
 * @param[in] deviceClass：要打开相机的类型。
 *
 * @brief：Configure camera device.
 * @param[in out] pCam：Camera struct pointer.
 * @param[in] deviceClass：class of camera to open. */
void ConfigureCamera(pItkCamera pCam, int index = -1, const char* deviceClass = NULL);

void GetAllConfigureMethods(pItkCamera pCam);

ITKSTATUS SetWidth(pItkCamera pCam, int64_t width);

ITKSTATUS GetWidth(pItkCamera pCam, int64_t* width);

ITKSTATUS SetHeight(pItkCamera pCam, int64_t height);

ITKSTATUS GetHeight(pItkCamera pCam, int64_t* height);

ITKSTATUS SetPixelFormat(pItkCamera pCam, const char* pixelFormat);

ITKSTATUS GetPixelFormat(pItkCamera pCam, char* pixelFormat, uint32_t* count);

ITKSTATUS SetExposureTime(pItkCamera pCam, double exposureTime);

ITKSTATUS GetExposureTime(pItkCamera pCam, double* exposureTime);

ITKSTATUS SetDigitalGain(pItkCamera pCam, double digitalGain);

ITKSTATUS GetDigitalGain(pItkCamera pCam, double* digitalGain);

ITKSTATUS SetPixelGain(pItkCamera pCam, const char* pixelGain);

ITKSTATUS GetPixelGain(pItkCamera pCam, char* pixelGain, uint32_t* len);

ITKSTATUS SetLineRate(pItkCamera pCam, double lineRate);

ITKSTATUS GetLineRate(pItkCamera pCam, double* lineRate);

ITKSTATUS SetFrameRate(pItkCamera pCam, double frameRate);

ITKSTATUS GetFrameRate(pItkCamera pCam, double* frameRate);

ITKSTATUS SetFrameTrigger(pItkCamera pCam, const char* status, bool level);

ITKSTATUS SetLineTrigger(pItkCamera pCam, const char* status, bool level);

ITKSTATUS SetTriggerSource(pItkCamera pCam, const char* triggerSource);

ITKSTATUS GetTriggerSource(pItkCamera pCam, char* triggerSource, uint32_t* len);

ITKSTATUS SetTriggerPolarity(pItkCamera pCam, const char* triggerPolarity);

ITKSTATUS GetTriggerPolarity(pItkCamera pCam, char* triggerPolarity, uint32_t* len);

ITKSTATUS SetMultiExposureTime(pItkCamera pCam,char* multiExposureTime);

ITKSTATUS GetMultiExposureTime(pItkCamera pCam, char* multiExposureTime, uint32_t* bufLen);

ITKSTATUS SetChunkData(pItkCamera pCam,bool benable);

ITKSTATUS GetChunkData(pItkCamera pCam,bool* pbenable);

