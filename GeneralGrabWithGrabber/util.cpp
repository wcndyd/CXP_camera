#include "util.h"
#include <string.h>

/// \~chinese  打印错误信息后退出程序		        \~english Print error message and exit the program
void printErrorAndExit(ITKSTATUS errc)
{
    fprintf(stderr, "Error Code:%08X\n", errc);
    ItkManTerminate();
    pressEnterToExit();
    exit(EXIT_FAILURE);
}

/// \~chinese 打印 IKapBoard 错误信息后退出程序				\~english Print IKapBoard error message and exit the program
void printIKapBoardErrorAndExit()
{
    IKAPERRORINFO errc;
    IKapGetLastError(&errc, 1);

    fprintf(stderr, "IKapBoard:Index: %d, error code:%04x\n", errc.uBoardIndex, errc.uErrorCode);

    pressEnterToExit();
    exit(EXIT_FAILURE);
}

/// \~chinese 打印 IKapC 错误信息后退出程序				    \~english Print IKapC error message and exit the program
void printIKapCErrorAndExit(ITKSTATUS errc)
{
    fprintf(stderr, "IKapC error code: %08X", errc);
    ItkManTerminate();
    pressEnterToExit();
    exit(EXIT_FAILURE);
}

void printCameraInfo(uint32_t numCameras)
{
    ITKSTATUS res = ITKSTATUS_OK;
    ITKDEV_INFO di;
    ITK_CXP_DEV_INFO cxp_cam_info = {};
    ITK_GVB_DEV_INFO gvb_cam_info = {};
    ITK_CL_DEV_INFO  cl_cam_info = {};

    printf("Camera Index: \t serial: \t name: \t interface: \t\n");
    for (uint32_t i = 0; i < numCameras; i++)
    {
        res = ItkManGetDeviceInfo(i,&di);
        if (res==ITKSTATUS_OK)
        {
            printf("%d \t %s \t %s \t %s \t\n",i,di.SerialNumber,di.FullName,di.DeviceClass);
        }

        if (strcmp(di.DeviceClass, "CoaXPress") == 0) {
            res = ItkManGetCXPDeviceInfo(i, &cxp_cam_info);
            if (res==ITKSTATUS_OK)
            {
                printf("\t cxpInfo: BoardIndex:%d \t MasterPort:%d \t SlaveCount:%d \t CameraId:%d \t Topology:%d \t\n",cxp_cam_info.BoardIndex,cxp_cam_info.MasterPort,cxp_cam_info.SlaveCount,cxp_cam_info.CameraId,cxp_cam_info.Topology);
                printf("\t ");
                for (int i = 0; i < 7; i++)
                {
                  printf("SlavePort[%d]:%d \t ", i, cxp_cam_info.SlavePort[i]);
                }
                printf("\n");
            }
        } else if (strcmp(di.DeviceClass, "GigEVisionBoard") == 0) {
            res = ItkManGetGVBDeviceInfo(i, &gvb_cam_info);
            if (res == ITKSTATUS_OK)
            {
                printf("\t gvbInfo: BoardIndex:%d \t MasterPort:%d \t MAC:%s \t Ip:%s \t SubNetMask:%s \t GateWay:%s \t SlaveCount:%d \t EffectPixel:%d \t\n",gvb_cam_info.BoardIndex,gvb_cam_info.MasterPort,gvb_cam_info.MAC,gvb_cam_info.Ip,gvb_cam_info.SubNetMask,gvb_cam_info.GateWay,gvb_cam_info.SlaveCount,gvb_cam_info.EffectPixel);
                printf("\t ");
                for (int i = 0; i < 3; i++)
                {
                    printf("SlavePort[%d]:%d \t ", i, gvb_cam_info.SlavePort[i]);
                }
                printf("\n");
            }
        }
        else if (strcmp(di.DeviceClass, "CameraLink") == 0) {
            res = ItkManGetCLDeviceInfo(i, &cl_cam_info);
            if (res == ITKSTATUS_OK)
            {
                printf("\t clInfo: HostInterface:%d \t BoardIndex:%d \t SerialPort:%s \t\n",cl_cam_info.HostInterface,cl_cam_info.BoardIndex,cl_cam_info.SerialPort);
            }
        }
    }
}
