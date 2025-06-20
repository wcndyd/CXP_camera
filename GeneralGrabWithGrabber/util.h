#pragma once
#include <IKapBoard.h>
#include <IKapC.h>
#include <stdio.h>
#include <stdlib.h>

/// \~chinese �û�����س������˳�����				        \~english Users enter Enter to exit the program
inline void pressEnterToExit(void)
{
    fprintf(stderr, "\nPress enter to exit.\n");
    while (getchar() != '\n')
        ;
}

/* @brief���жϺ����Ƿ�ɹ����á�
 * @param[in] errc����������ֵ��
 *
 * @brief��Determine whether the function is called successfully.
 * @param[in] errc��Function return value. */
#define CHECK(errc)                                                                                                         \
    if (ITKSTATUS_OK != errc)                                                                                               \
    printErrorAndExit(errc)

/* @brief���ж� IKapBoard �����Ƿ�ɹ����á�
 * @param[in] errc����������ֵ��
 *
 * @brief��Determine whether the IKapBoard function is called successfully.
 * @param[in] errc��Function return value. */
#define CHECK_IKAPBOARD(errc)                                                                                               \
    if (errc != 1)                                                                                                          \
    printIKapBoardErrorAndExit()

/* @brief���ж� IKapC �����Ƿ�ɹ����á�
 * @param[in] errc����������ֵ��
 *
 * @brief��Determine whether the IKapC function is called successfully.
 * @param[in] errc��Function return value. */
#define CHECK_IKAPC(errc)                                                                                                   \
    if (errc != ITKSTATUS_OK)                                                                                               \
    printIKapCErrorAndExit(errc)

/* @brief����ӡ������Ϣ���˳�����
 * @param[in] errc����������ֵ��
 *
 * @brief��Print error message and exit the program.
 * @param[in] errc��Function return value. */
void printErrorAndExit(ITKSTATUS errc);

/* @brief����ӡ IKapBoard ������Ϣ���˳�����
 *
 * @brief��Print IKapBoard error message and exit the program. */
void printIKapBoardErrorAndExit();

/* @brief����ӡ IKapC ������Ϣ���˳�����
 * @param[in] errc����������ֵ��
 *
 * @brief��Print IKapC error message and exit the program.
 * @param[in] errc��Function return value. */
void printIKapCErrorAndExit(ITKSTATUS errc);

/* @brief����ӡ�����Ϣ��
 * @param[in] numCameras��ö�ٵ��Ŀ������������
 *
 * @brief��Print cameras information.
 * @param[in] numCameras��The number of available cameras enumerated. */
void printCameraInfo(uint32_t numCameras);
