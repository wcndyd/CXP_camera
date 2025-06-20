#pragma once

#include "../GeneralGrabWithGrabber/ItkCamera.h"

/* @brief�����ô�����
 * @param[in] pCam������ṹ��ָ�롣
 *
 * @brief��Configure trigger.
 * @param[in] pCam��Camera struct pointer. */
void SetSoftTriggerWithoutGrabber(pItkCamera pCam);

/* @brief�������������ͻ�������
 *
 * @brief��Create data stream and buffer. */
void CreateStreamAndBuffer(pItkCamera pCam);
