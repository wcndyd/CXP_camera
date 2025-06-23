#pragma once

#include "Camera.h"
#include "util.h"

/* @brief��ѡ���û������ļ���
 * @param[in] lpConfigFileName�������ļ�����
 *
 * @brief��Select configuration file.
 * @param[in] lpConfigFileName��Configuration file name. */
bool GetOption(char* lpConfigFileName);

/* @brief�����ô�����
 * @param[in] pCam������ṹ��ָ�롣
 *
 * @brief��Configure trigger.
 * @param[in] pCam��Camera struct pointer. */
void SetSoftTriggerWithGrabber(pItkCamera pCam);

/* @brief�����òɼ����豸��
 * @param[in] pCam������ṹ��ָ�롣
 *
 * @brief��Configure frame grabber device.
 * @param[in] pCam��Camera struct pointer. */
void ConfigureFrameGrabber(pItkCamera pCam);

/* @brief����ʼͼ��ɼ���
 * @param[in] pCam������ṹ��ָ�롣
 *
 * @brief��Start grabbing images.
 * @param[in] hCamera��Camera device handle.
 * @param[in] pCam��Camera struct pointer. */
void StartGrabImage(pItkCamera pCam);