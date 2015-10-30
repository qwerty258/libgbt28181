/******************** (C) COPYRIGHT 2013 CSENN *********************************
* File Name          : csenn_eXosip.c
* Author             : www.csenn.com
* Date               : 2013/05/08
**************************************************************START OF FILE****/
#include <eXosip2/eXosip.h>
#include <osip2/osip_mt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <sys/types.h>

struct _device_info                 // device infomation struct
{
    char *server_id;                // SIP server ID, default: 34020000002000000001
    char *server_ip;                // SIP server IP, default: 192.168.1.178
    char *server_port;              // SIP server port, default: 5060

    char *ipc_id;                   /*媒体流发送者ID*//*默认值：34020000001180000002*/
    char *ipc_pwd;                  /*媒体流发送者密码*//*默认值：12345678*/
    char *ipc_ip;                   /*媒体流发送者IP地址*//*默认值：192.168.1.144*/
    char *ipc_port;                 /*媒体流发送者IP端口*//*默认值：6000*/

    char *device_name;              /*设备/区域/系统名称*//*默认值：IPC*/
    char *device_manufacturer;      /*设备厂商*//*默认值：csenn*/
    char *device_model;             /*设备型号*//*默认值：YF1109*/
    char *device_firmware;          /*设备固件版本*//*默认值：V1.0*/
    char *device_encode;            /*是否编码*//*取值范围：ON/OFF*//*默认值：ON*/
    char *device_record;            /*是否录像*//*取值范围：ON/OFF*//*默认值：OFF*/
}device_info;
struct _device_status               /*设备状态结构体*/
{
    char *status_on;                /*设备打开状态*//*取值范围：ON/OFF*//*默认值：ON*/
    char *status_ok;                /*是否正常工作*//*取值范围：OK/ERROR*//*默认值：OFF*/
    char *status_online;            /*是否在线*//*取值范围：ONLINE/OFFLINE*//*默认值：ONLINE*/
    char *status_guard;             /*布防状态*//*取值范围：ONDUTY/OFFDUTY/ALARM*//*默认值：OFFDUTY*/
    char *status_time;              /*设备日期和时间*//*格式：xxxx-xx-xxTxx:xx:xx*//*默认值：2012-12-18T16:23:32*/
}device_status;
enum _device_control
{
    EXOSIP_CTRL_RMT_LEFT = 1,       /*向左*/
    EXOSIP_CTRL_RMT_RIGHT,          /*向右*/
    EXOSIP_CTRL_RMT_UP,             /*向上*/
    EXOSIP_CTRL_RMT_DOWN,           /*向下*/
    EXOSIP_CTRL_RMT_LARGE,          /*放大*/
    EXOSIP_CTRL_RMT_SMALL,          /*缩小*/
    EXOSIP_CTRL_RMT_STOP,           /*停止遥控*/
    EXOSIP_CTRL_REC_START,          /*开始手动录像*/
    EXOSIP_CTRL_REC_STOP,           /*停止手动录像*/
    EXOSIP_CTRL_GUD_START,          /*布防*/
    EXOSIP_CTRL_GUD_STOP,           /*撤防*/
    EXOSIP_CTRL_ALM_RESET,          /*报警复位*/
    EXOSIP_CTRL_TEL_BOOT,           /*设备远程启动*/
};

/*回调函数*/
struct _csenn_eXosip_callback
{
    /*获取设备信息*/
    /*device_info：设备信息结构体指针*/
    /*返回值：成功时返回0，失败时返回负值*/
    int(*csenn_eXosip_getDeviceInfo)(struct _device_info *device_info);

    /*获取设备状态*/
    /*device_info：设备状态结构体指针*/
    /*返回值：成功时返回0，失败时返回负值*/
    int(*csenn_eXosip_getDeviceStatus)(struct _device_status *device_status);

    /*获取录像文件的起始时间与结束时间*/
    /*时间格式：xxxx-xx-xxTxx:xx:xx*/
    /*period_start：录像时间段起始值*/
    /*period_end：录像时间段结束值*/
    /*start_time：当前返回录像文件的起始时间*/
    /*end_time：当前返回录像文件的结束时间*/
    /*返回值：成功时返回符合时间段条件的剩余录像文件数量，失败时返回负值*/
    int(*csenn_eXosip_getRecordTime)(char *period_start, char *period_end, char *start_time, char *end_time);

    /*设备控制：向左、向右、向上、向下、放大、缩小、停止遥控/开始手动录像、停止手动录像/布防、撤防/报警复位/设备远程启动*/
    /*ctrl_cmd：设备控制命令，_device_control类型的枚举变量*/
    /*返回值：成功时返回0，失败时返回负值*/
    int(*csenn_eXosip_deviceControl)(enum _device_control ctrl_cmd);

    /*媒体控制：实时点播/回放/下载*/
    /*control_type：媒体控制类型，实时点播/Play，回放/Playback，下载/Download*/
    /*media_ip：媒体服务器IP地址*/
    /*media_port：媒体服务器IP端口*/
    /*返回值：成功时返回0，失败时返回负值*/
    int(*csenn_eXosip_mediaControl)(char *control_type, char *media_ip, char *media_port);

    /*播放控制：播放/快放/慢放/暂停*/
    /*control_type：播放控制，播放/快放/慢放/PLAY，暂停/PAUSE*/
    /*play_speed：播放速度，1为播放，大于1为快放，小于1为慢放*/
    /*pause_time：暂停时间，单位为秒*/
    /*range_start：播放范围的起始值*/
    /*range_end：播放范围的结束值*/
    /*返回值：成功时返回0，失败时返回负值*/
    int(*csenn_eXosip_playControl)(char *control_type, char *play_speed, char *pause_time, char *range_start, char *range_end);
}csenn_eXosip_callback;

/*被调函数*/
/*启动并注册eXosip*/
void csenn_eXosip_launch(void);

/*注册eXosip*/
/*expires：重新注册的超时时间，单位为秒*/
/*返回值：成功时返回0，失败时返回负值*/
int csenn_eXosip_register(int expires);

/*注销eXosip*/
/*返回值：成功时返回0，失败时返回负值*/
int csenn_eXosip_unregister(void);

/*发送MESSAGE：报警*/
/*时间格式：xxxx-xx-xxTxx:xx:xx*/
/*alarm_time：报警时间*/
void csenn_eXosip_sendEventAlarm(char *alarm_time);

/*发送MESSAGE：文件结束*/
void csenn_eXosip_sendFileEnd(void);
/******************** (C) COPYRIGHT 2013 CSENN *****************END OF FILE****/
