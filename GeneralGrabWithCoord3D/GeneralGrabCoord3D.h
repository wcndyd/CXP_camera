#pragma once

#include "../GeneralGrabWithGrabber/ItkCamera.h"

/* @brief：配置触发。
 * @param[in] pCam：相机结构体指针。
 *
 * @brief：Configure trigger.
 * @param[in] pCam：Camera struct pointer. */
void SetSoftTriggerWithoutGrabber(pItkCamera pCam);

/* @brief：创建数据流和缓冲区。
 *
 * @brief：Create data stream and buffer. */
void CreateStreamAndBuffer(pItkCamera pCam);
