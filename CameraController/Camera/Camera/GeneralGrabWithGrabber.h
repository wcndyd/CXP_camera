#pragma once

#include "Camera.h"
#include "util.h"

/* @brief：选择用户配置文件。
 * @param[in] lpConfigFileName：配置文件名。
 *
 * @brief：Select configuration file.
 * @param[in] lpConfigFileName：Configuration file name. */
bool GetOption(char* lpConfigFileName);

/* @brief：配置触发。
 * @param[in] pCam：相机结构体指针。
 *
 * @brief：Configure trigger.
 * @param[in] pCam：Camera struct pointer. */
void SetSoftTriggerWithGrabber(pItkCamera pCam);

/* @brief：配置采集卡设备。
 * @param[in] pCam：相机结构体指针。
 *
 * @brief：Configure frame grabber device.
 * @param[in] pCam：Camera struct pointer. */
void ConfigureFrameGrabber(pItkCamera pCam);

/* @brief：开始图像采集。
 * @param[in] pCam：相机结构体指针。
 *
 * @brief：Start grabbing images.
 * @param[in] hCamera：Camera device handle.
 * @param[in] pCam：Camera struct pointer. */
void StartGrabImage(pItkCamera pCam);