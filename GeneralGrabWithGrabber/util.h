#pragma once
#include <IKapBoard.h>
#include <IKapC.h>
#include <stdio.h>
#include <stdlib.h>

/// \~chinese 用户输入回车键后退出程序				        \~english Users enter Enter to exit the program
inline void pressEnterToExit(void)
{
    fprintf(stderr, "\nPress enter to exit.\n");
    while (getchar() != '\n')
        ;
}

/* @brief：判断函数是否成功调用。
 * @param[in] errc：函数返回值。
 *
 * @brief：Determine whether the function is called successfully.
 * @param[in] errc：Function return value. */
#define CHECK(errc)                                                                                                         \
    if (ITKSTATUS_OK != errc)                                                                                               \
    printErrorAndExit(errc)

/* @brief：判断 IKapBoard 函数是否成功调用。
 * @param[in] errc：函数返回值。
 *
 * @brief：Determine whether the IKapBoard function is called successfully.
 * @param[in] errc：Function return value. */
#define CHECK_IKAPBOARD(errc)                                                                                               \
    if (errc != 1)                                                                                                          \
    printIKapBoardErrorAndExit()

/* @brief：判断 IKapC 函数是否成功调用。
 * @param[in] errc：函数返回值。
 *
 * @brief：Determine whether the IKapC function is called successfully.
 * @param[in] errc：Function return value. */
#define CHECK_IKAPC(errc)                                                                                                   \
    if (errc != ITKSTATUS_OK)                                                                                               \
    printIKapCErrorAndExit(errc)

/* @brief：打印错误信息后退出程序。
 * @param[in] errc：函数返回值。
 *
 * @brief：Print error message and exit the program.
 * @param[in] errc：Function return value. */
void printErrorAndExit(ITKSTATUS errc);

/* @brief：打印 IKapBoard 错误信息后退出程序。
 *
 * @brief：Print IKapBoard error message and exit the program. */
void printIKapBoardErrorAndExit();

/* @brief：打印 IKapC 错误信息后退出程序。
 * @param[in] errc：函数返回值。
 *
 * @brief：Print IKapC error message and exit the program.
 * @param[in] errc：Function return value. */
void printIKapCErrorAndExit(ITKSTATUS errc);

/* @brief：打印相机信息。
 * @param[in] numCameras：枚举到的可用相机个数。
 *
 * @brief：Print cameras information.
 * @param[in] numCameras：The number of available cameras enumerated. */
void printCameraInfo(uint32_t numCameras);
