/******************** (C) COPYRIGHT 2013 CSENN *********************************
* File Name          : csenn_eXosip.c
* Author             : www.csenn.com
* Date               : 2013/05/08
**************************************************************START OF FILE****/
#include "csenn_eXosip.h"

int g_register_id = 0;/*注册ID/用来更新注册或取消注册*/
int g_call_id = 0;/*INVITE连接ID/用来分辨不同的INVITE连接，每个时刻只允许有一个INVITE连接*/
int g_did_realPlay = 0;/*会话ID/用来分辨不同的会话：实时视音频点播*/
int g_did_backPlay = 0;/*会话ID/用来分辨不同的会话：历史视音频回放*/
int g_did_fileDown = 0;/*会话ID/用来分辨不同的会话：视音频文件下载*/
struct eXosip_t* g_p_eXosip_context = NULL;

int  csenn_eXosip_init(void);/*初始化eXosip*/
int  csenn_eXosip_register(int expires);/*注册eXosip，手动处理服务器返回的401状态*/
int  csenn_eXosip_unregister(void);/*注销eXosip*/
void csenn_eXosip_sendEventAlarm(char *alarm_time);/*报警事件通知和分发：发送报警通知*/
void csenn_eXosip_sendFileEnd(void);/*历史视音频回放：发送文件结束*/
void csenn_eXosip_paraseMsgBody(eXosip_event_t *p_event);/*解析MESSAGE的XML消息体*/
void csenn_eXosip_paraseInviteBody(eXosip_event_t *p_event);/*解析INVITE的SDP消息体，同时保存全局INVITE连接ID和全局会话ID*/
void csenn_eXosip_paraseInfoBody(eXosip_event_t *p_event);/*解析INFO的RTSP消息体*/
void csenn_eXosip_printEvent(eXosip_event_t *p_event);/*检测并打印事件*/
void csenn_eXosip_processEvent(void);/*消息循环处理*/
void csenn_eXosip_launch(void);/*启动并注册eXosip*/

/*初始化eXosip*/
int csenn_eXosip_init(void)
{
    int ret = 0;
    g_p_eXosip_context = eXosip_malloc();
    if(NULL == g_p_eXosip_context)
    {
        return -1;
    }

    ret = eXosip_init(g_p_eXosip_context);/*初始化eXosip和osip协议栈*/
    if(0 != ret)
    {
        printf("Couldn't initialize eXosip!\r\n");
        return -1;
    }
    printf("eXosip_init success!\r\n");

    ret = eXosip_listen_addr(g_p_eXosip_context, IPPROTO_UDP, NULL, 5060, AF_INET, 0);
    if(0 != ret)/*传输层初始化失败*/
    {
        eXosip_quit(g_p_eXosip_context);
        osip_free(g_p_eXosip_context);
        printf("eXosip_listen_addr error!\r\n");
        return -1;
    }
    printf("eXosip_listen_addr success!\r\n");

    return 0;
}

/*注册eXosip，手动处理服务器返回的401状态*/
int csenn_eXosip_register(int expires)/*expires/注册消息过期时间，单位为秒*/
{
    int ret = 0;
    eXosip_event_t *je = NULL;
    osip_message_t *reg = NULL;
    char from[100];/*sip:主叫用户名@被叫IP地址*/
    char proxy[100];/*sip:被叫IP地址:被叫IP端口*/

    memset(from, 0, 100);
    memset(proxy, 0, 100);
    sprintf(from, "sip:%s@%s", device_info.ipc_id, device_info.server_ip);
    sprintf(proxy, "sip:%s:%s", device_info.server_ip, device_info.server_port);

    /*发送不带认证信息的注册请求*/
retry:
    eXosip_lock(g_p_eXosip_context);
    g_register_id = eXosip_register_build_initial_register(g_p_eXosip_context, from, proxy, NULL, expires, &reg);
    osip_message_set_authorization(reg, "Capability algorithm=\"H:MD5\"");
    if(0 > g_register_id)
    {
        eXosip_lock(g_p_eXosip_context);
        printf("eXosip_register_build_initial_register error!\r\n");
        return -1;
    }
    printf("eXosip_register_build_initial_register success!\r\n");

    ret = eXosip_register_send_register(g_p_eXosip_context, g_register_id, reg);
    eXosip_unlock(g_p_eXosip_context);
    if(0 != ret)
    {
        printf("eXosip_register_send_register no authorization error!\r\n");
        return -1;
    }
    printf("eXosip_register_send_register no authorization success!\r\n");

    printf("g_register_id=%d\r\n", g_register_id);

    for(;;)
    {
        je = eXosip_event_wait(g_p_eXosip_context, 0, 50);/*侦听消息的到来*/
        if(NULL == je)/*没有接收到消息*/
        {
            continue;
        }
        if(EXOSIP_REGISTRATION_FAILURE == je->type)/*注册失败*/
        {
            printf("<EXOSIP_REGISTRATION_FAILURE>\r\n");
            printf("je->rid=%d\r\n", je->rid);
            /*收到服务器返回的注册失败/401未认证状态*/
            if((NULL != je->response) && (401 == je->response->status_code))
            {
                reg = NULL;
                /*发送携带认证信息的注册请求*/
                eXosip_lock(g_p_eXosip_context);
                eXosip_clear_authentication_info(g_p_eXosip_context);/*清除认证信息*/
                eXosip_add_authentication_info(g_p_eXosip_context,device_info.ipc_id, device_info.ipc_id, device_info.ipc_pwd, "MD5", NULL);/*添加主叫用户的认证信息*/
                eXosip_register_build_register(g_p_eXosip_context,je->rid, expires, &reg);
                ret = eXosip_register_send_register(g_p_eXosip_context,je->rid, reg);
                eXosip_unlock(g_p_eXosip_context);
                if(0 != ret)
                {
                    printf("eXosip_register_send_register authorization error!\r\n");
                    return -1;
                }
                printf("eXosip_register_send_register authorization success!\r\n");
            }
            else/*真正的注册失败*/
            {
                printf("EXOSIP_REGISTRATION_FAILURE error!\r\n");
                goto retry;/*重新注册*/
            }
        }
        else if(EXOSIP_REGISTRATION_SUCCESS == je->type)
        {
            /*收到服务器返回的注册成功*/
            printf("<EXOSIP_REGISTRATION_SUCCESS>\r\n");
            g_register_id = je->rid;/*保存注册成功的注册ID*/
            printf("g_register_id=%d\r\n", g_register_id);
            break;
        }
    }

    return 0;
}
/*注销eXosip*/
int csenn_eXosip_unregister(void)
{
    return csenn_eXosip_register(0);
}

/*报警事件通知和分发：发送报警通知*/
void csenn_eXosip_sendEventAlarm(char *alarm_time)
{
    if(0 == strcmp(device_status.status_guard, "OFFDUTY"))/*当前布防状态为：OFFDUTY，不发送报警消息*/
    {
        printf("device_status.status_guard=OFFDUTY\r\n");
    }
    else
    {
        osip_message_t *rqt_msg = NULL;
        char to[100];/*sip:主叫用户名@被叫IP地址*/
        char from[100];/*sip:被叫IP地址:被叫IP端口*/
        char xml_body[4096];

        memset(to, 0, 100);
        memset(from, 0, 100);
        memset(xml_body, 0, 4096);

        sprintf(to, "sip:%s@%s", device_info.ipc_id, device_info.server_ip);
        sprintf(from, "sip:%s:%s", device_info.server_ip, device_info.server_port);
        eXosip_message_build_request(g_p_eXosip_context,&rqt_msg, "MESSAGE", to, from, NULL);/*构建"MESSAGE"请求*/
        snprintf(xml_body, 4096, "<?xml version=\"1.0\"?>\r\n"
                                 "<Notify>\r\n"
                                 "<CmdType>Alarm</CmdType>\r\n"/*命令类型*/
                                 "<SN>1</SN>\r\n"/*命令序列号*/
                                 "<DeviceID>%s</DeviceID>\r\n"/*报警设备编码/报警中心编码*/
                                 "<AlarmPriority>3</AlarmPriority>\r\n"/*报警级别/1为一级警情/2为二级警情/3为三级警情/4为四级警情*/
                                 "<AlarmMethod>2</AlarmMethod>\r\n"/*报警方式/1为电话报警/2为设备报警/3为短信报警/4为GPS报警/5为视频报警/6为设备故障报警/7其它报警*/
                                 "<AlarmTime>%s</AlarmTime>\r\n"/*报警时间/格式：2012-12-18T16:23:32*/
                                 "</Notify>\r\n",
                                 device_info.ipc_id,
                                 alarm_time);
        osip_message_set_body(rqt_msg, xml_body, strlen(xml_body));
        osip_message_set_content_type(rqt_msg, "Application/MANSCDP+xml");
        eXosip_message_send_request(g_p_eXosip_context,rqt_msg);/*回复"MESSAGE"请求*/
        printf("csenn_eXosip_sendEventAlarm success!\r\n");

        strcpy(device_status.status_guard, "ALARM");/*设置布防状态为"ALARM"*/
    }
}
/*历史视音频回放：发送文件结束*/
void csenn_eXosip_sendFileEnd(void)
{
    if(0 != g_did_backPlay)/*当前会话为：历史视音频回放*/
    {
        osip_message_t *rqt_msg = NULL;
        char to[100];/*sip:主叫用户名@被叫IP地址*/
        char from[100];/*sip:被叫IP地址:被叫IP端口*/
        char xml_body[4096];

        memset(to, 0, 100);
        memset(from, 0, 100);
        memset(xml_body, 0, 4096);

        sprintf(to, "sip:%s@%s", device_info.ipc_id, device_info.server_ip);
        sprintf(from, "sip:%s:%s", device_info.server_ip, device_info.server_port);
        eXosip_message_build_request(g_p_eXosip_context,&rqt_msg, "MESSAGE", to, from, NULL);/*构建"MESSAGE"请求*/
        snprintf(xml_body, 4096, "<?xml version=\"1.0\"?>\r\n"
                                 "<Notify>\r\n"
                                 "<CmdType>MediaStatus</CmdType>\r\n"
                                 "<SN>8</SN>\r\n"
                                 "<DeviceID>%s</DeviceID>\r\n"
                                 "<NotifyType>121</NotifyType>\r\n"
                                 "</Notify>\r\n",
                                 device_info.ipc_id);
        osip_message_set_body(rqt_msg, xml_body, strlen(xml_body));
        osip_message_set_content_type(rqt_msg, "Application/MANSCDP+xml");
        eXosip_message_send_request(g_p_eXosip_context,rqt_msg);/*回复"MESSAGE"请求*/
        printf("csenn_eXosip_sendFileEnd success!\r\n");
    }
}

/*解析MESSAGE的XML消息体*/
void csenn_eXosip_paraseMsgBody(eXosip_event_t *p_event)
{
    /*与请求相关的变量*/
    osip_body_t *p_rqt_body = NULL;
    char *p_xml_body = NULL;
    char *p_str_begin = NULL;
    char *p_str_end = NULL;
    char xml_cmd_type[20];
    char xml_cmd_sn[10];
    char xml_device_id[30];
    char xml_command[30];
    /*与回复相关的变量*/
    osip_message_t *rsp_msg = NULL;
    char to[100];/*sip:主叫用户名@被叫IP地址*/
    char from[100];/*sip:被叫IP地址:被叫IP端口*/
    char rsp_xml_body[4096];

    memset(xml_cmd_type, 0, 20);
    memset(xml_cmd_sn, 0, 10);
    memset(xml_device_id, 0, 30);
    memset(xml_command, 0, 30);
    memset(to, 0, 100);
    memset(from, 0, 100);
    memset(rsp_xml_body, 0, 4096);

    sprintf(to, "sip:%s@%s", device_info.ipc_id, device_info.server_ip);
    sprintf(from, "sip:%s:%s", device_info.server_ip, device_info.server_port);
    eXosip_message_build_request(g_p_eXosip_context,&rsp_msg, "MESSAGE", to, from, NULL);/*构建"MESSAGE"请求*/

    osip_message_get_body(p_event->request, 0, &p_rqt_body);/*获取接收到请求的XML消息体*/
    if(NULL == p_rqt_body)
    {
        printf("osip_message_get_body null!\r\n");
        return;
    }
    p_xml_body = p_rqt_body->body;
    printf("osip_message_get_body success!\r\n");

    printf("**********CMD START**********\r\n");
    p_str_begin = strstr(p_xml_body, "<CmdType>");/*查找字符串"<CmdType>"*/
    p_str_end = strstr(p_xml_body, "</CmdType>");
    memcpy(xml_cmd_type, p_str_begin + 9, p_str_end - p_str_begin - 9);/*保存<CmdType>到xml_cmd_type*/
    printf("<CmdType>:%s\r\n", xml_cmd_type);

    p_str_begin = strstr(p_xml_body, "<SN>");/*查找字符串"<SN>"*/
    p_str_end = strstr(p_xml_body, "</SN>");
    memcpy(xml_cmd_sn, p_str_begin + 4, p_str_end - p_str_begin - 4);/*保存<SN>到xml_cmd_sn*/
    printf("<SN>:%s\r\n", xml_cmd_sn);

    p_str_begin = strstr(p_xml_body, "<DeviceID>");/*查找字符串"<DeviceID>"*/
    p_str_end = strstr(p_xml_body, "</DeviceID>");
    memcpy(xml_device_id, p_str_begin + 10, p_str_end - p_str_begin - 10);/*保存<DeviceID>到xml_device_id*/
    printf("<DeviceID>:%s\r\n", xml_device_id);
    printf("***********CMD END***********\r\n");

    if(0 == strcmp(xml_cmd_type, "DeviceControl"))/*设备控制*/
    {
        printf("**********CONTROL START**********\r\n");
        /*向左、向右、向上、向下、放大、缩小、停止遥控*/
        p_str_begin = strstr(p_xml_body, "<PTZCmd>");/*查找字符串"<PTZCmd>"*/
        if(NULL != p_str_begin)
        {
            p_str_end = strstr(p_xml_body, "</PTZCmd>");
            memcpy(xml_command, p_str_begin + 8, p_str_end - p_str_begin - 8);/*保存<PTZCmd>到xml_command*/
            printf("<PTZCmd>:%s\r\n", xml_command);
            goto DeviceControl_Next;
        }
        /*开始手动录像、停止手动录像*/
        p_str_begin = strstr(p_xml_body, "<RecordCmd>");/*查找字符串"<RecordCmd>"*/
        if(NULL != p_str_begin)
        {
            p_str_end = strstr(p_xml_body, "</RecordCmd>");
            memcpy(xml_command, p_str_begin + 11, p_str_end - p_str_begin - 11);/*保存<RecordCmd>到xml_command*/
            printf("<RecordCmd>:%s\r\n", xml_command);
            goto DeviceControl_Next;
        }
        /*布防、撤防*/
        p_str_begin = strstr(p_xml_body, "<GuardCmd>");/*查找字符串"<GuardCmd>"*/
        if(NULL != p_str_begin)
        {
            p_str_end = strstr(p_xml_body, "</GuardCmd>");
            memcpy(xml_command, p_str_begin + 10, p_str_end - p_str_begin - 10);/*保存<GuardCmd>到xml_command*/
            printf("<GuardCmd>:%s\r\n", xml_command);
            goto DeviceControl_Next;
        }
        /*报警复位：30秒内不再触发报警*/
        p_str_begin = strstr(p_xml_body, "<AlarmCmd>");/*查找字符串"<AlarmCmd>"*/
        if(NULL != p_str_begin)
        {
            p_str_end = strstr(p_xml_body, "</AlarmCmd>");
            memcpy(xml_command, p_str_begin + 10, p_str_end - p_str_begin - 10);/*保存<AlarmCmd>到xml_command*/
            printf("<AlarmCmd>:%s\r\n", xml_command);
            goto DeviceControl_Next;
        }
        /*设备远程启动*/
        p_str_begin = strstr(p_xml_body, "<TeleBoot>");/*查找字符串"<TeleBoot>"*/
        if(NULL != p_str_begin)
        {
            p_str_end = strstr(p_xml_body, "</TeleBoot>");
            memcpy(xml_command, p_str_begin + 10, p_str_end - p_str_begin - 10);/*保存<TeleBoot>到xml_command*/
            printf("<TeleBoot>:%s\r\n", xml_command);
            goto DeviceControl_Next;
        }
    DeviceControl_Next:
        printf("***********CONTROL END***********\r\n");
        if(0 == strcmp(xml_command, "A50F01021F0000D6"))/*向左*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_RMT_LEFT);
        }
        else if(0 == strcmp(xml_command, "A50F01011F0000D5"))/*向右*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_RMT_RIGHT);
        }
        else if(0 == strcmp(xml_command, "A50F0108001F00DC"))/*向上*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_RMT_UP);
        }
        else if(0 == strcmp(xml_command, "A50F0104001F00D8"))/*向下*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_RMT_DOWN);
        }
        else if(0 == strcmp(xml_command, "A50F0110000010D5"))/*放大*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_RMT_LARGE);
        }
        else if(0 == strcmp(xml_command, "A50F0120000010E5"))/*缩小*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_RMT_SMALL);
        }
        else if(0 == strcmp(xml_command, "A50F0100000000B5"))/*停止遥控*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_RMT_STOP);
        }
        else if(0 == strcmp(xml_command, "Record"))/*开始手动录像*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_REC_START);
        }
        else if(0 == strcmp(xml_command, "StopRecord"))/*停止手动录像*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_REC_STOP);
        }
        else if(0 == strcmp(xml_command, "SetGuard"))/*布防*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_GUD_START);
            strcpy(device_status.status_guard, "ONDUTY");/*设置布防状态为"ONDUTY"*/
        }
        else if(0 == strcmp(xml_command, "ResetGuard"))/*撤防*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_GUD_STOP);
            strcpy(device_status.status_guard, "OFFDUTY");/*设置布防状态为"OFFDUTY"*/
        }
        else if(0 == strcmp(xml_command, "ResetAlarm"))/*报警复位*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_ALM_RESET);
        }
        else if(0 == strcmp(xml_command, "Boot"))/*设备远程启动*/
        {
            csenn_eXosip_callback.csenn_eXosip_deviceControl(EXOSIP_CTRL_TEL_BOOT);
        }
        else
        {
            printf("unknown device control command!\r\n");
        }
        snprintf(rsp_xml_body, 4096, "<?xml version=\"1.0\"?>\r\n"
                                     "<Response>\r\n"
                                     "<CmdType>DeviceControl</CmdType>\r\n"/*命令类型*/
                                     "<SN>%s</SN>\r\n"/*命令序列号*/
                                     "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统编码*/
                                     "<Result>OK</Result>\r\n"/*执行结果标志*/
                                     "</Response>\r\n",
                                     xml_cmd_sn,
                                     xml_device_id);
    }
    else if(0 == strcmp(xml_cmd_type, "Alarm"))/*报警事件通知和分发：报警通知响应*/
    {
        printf("**********ALARM START**********\r\n");
        /*报警通知响应*/
        printf("local eventAlarm response success!\n");
        printf("***********ALARM END***********\r\n");
        return;
    }
    else if(0 == strcmp(xml_cmd_type, "Catalog"))/*网络设备信息查询：设备目录查询*/
    {
        printf("**********CATALOG START**********\r\n");
        /*设备目录查询*/
        printf("***********CATALOG END***********\r\n");
        snprintf(rsp_xml_body, 4096, "<?xml version=\"1.0\"?>\r\n"
                                     "<Response>\r\n"
                                     "<CmdType>Catalog</CmdType>\r\n"/*命令类型*/
                                     "<SN>%s</SN>\r\n"/*命令序列号*/
                                     "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                                     "<SumNum>1</SumNum>\r\n"/*查询结果总数*/
                                     "<DeviceList Num=\"1\">\r\n"/*设备目录项列表*/
                                     "<Item>\r\n"
                                     "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                                     "<Name>%s</Name>\r\n"/*设备/区域/系统名称*/
                                     "<Manufacturer>%s</Manufacturer>\r\n"/*设备厂商*/
                                     "<Model>%s</Model>\r\n"/*设备型号*/
                                     "<Owner>Owner1</Owner>\r\n"/*设备归属*/
                                     "<CivilCode>CivilCode1</CivilCode>\r\n"/*行政区域*/
                                     "<Block>Block1</Block>\r\n"/*警区*/
                                     "<Address>Address1</Address>\r\n"/*安装地址*/
                                     "<Parental>0</Parental>\r\n"/*是否有子设备*/
                                     "<ParentID>%s</ParentID>\r\n"/*父设备/区域/系统ID*/
                                     "<SafetyWay>0</SafetyWay>\r\n"/*信令安全模式/0为不采用/2为S/MIME签名方式/3为S/MIME加密签名同时采用方式/4为数字摘要方式*/
                                     "<RegisterWay>1</RegisterWay>\r\n"/*注册方式/1为符合sip3261标准的认证注册模式/2为基于口令的双向认证注册模式/3为基于数字证书的双向认证注册模式*/
                                     "<CertNum>CertNum1</CertNum>\r\n"/*证书序列号*/
                                     "<Certifiable>0</Certifiable>\r\n"/*证书有效标识/0为无效/1为有效*/
                                     "<ErrCode>400</ErrCode>\r\n"/*无效原因码*/
                                     "<EndTime>2099-12-31T23:59:59</EndTime>\r\n"/*证书终止有效期*/
                                     "<Secrecy>0</Secrecy>\r\n"/*保密属性/0为不涉密/1为涉密*/
                                     "<IPAddress>%s</IPAddress>\r\n"/*设备/区域/系统IP地址*/
                                     "<Port>%s</Port>\r\n"/*设备/区域/系统端口*/
                                     "<Password>Password1</Password>\r\n"/*设备口令*/
                                     "<Status>OK</Status>\r\n"/*设备状态*/
                                     "<Longitude>171.3</Longitude>\r\n"/*经度*/
                                     "<Latitude>34.2</Latitude>\r\n"/*纬度*/
                                     "</Item>\r\n"
                                     "</DeviceList>\r\n"
                                     "</Response>\r\n",
                                     xml_cmd_sn,
                                     xml_device_id,
                                     xml_device_id,
                                     device_info.device_name,
                                     device_info.device_manufacturer,
                                     device_info.device_model,
                                     xml_device_id,
                                     device_info.ipc_ip,
                                     device_info.ipc_port);
    }
    else if(0 == strcmp(xml_cmd_type, "DeviceInfo"))/*网络设备信息查询：设备信息查询*/
    {
        printf("**********DEVICE INFO START**********\r\n");
        /*设备信息查询*/
        printf("***********DEVICE INFO END***********\r\n");
        snprintf(rsp_xml_body, 4096, "<?xml version=\"1.0\"?>\r\n"
                                     "<Response>\r\n"
                                     "<CmdType>DeviceInfo</CmdType>\r\n"/*命令类型*/
                                     "<SN>%s</SN>\r\n"/*命令序列号*/
                                     "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                                     "<Result>OK</Result>\r\n"/*查询结果*/
                                     "<Manufacturer>%s</Manufacturer>\r\n"/*设备生产商*/
                                     "<Model>%s</Model>\r\n"/*设备型号*/
                                     "<Firmware>%s</Firmware>\r\n"/*设备固件版本*/
                                     "</Response>\r\n",
                                     xml_cmd_sn,
                                     xml_device_id,
                                     device_info.device_manufacturer,
                                     device_info.device_model,
                                     device_info.device_firmware);
    }
    else if(0 == strcmp(xml_cmd_type, "DeviceStatus"))/*网络设备信息查询：设备状态查询*/
    {
        printf("**********DEVICE STATUS START**********\r\n");
        /*设备状态查询*/
        printf("***********DEVICE STATUS END***********\r\n");
        char xml_status_guard[10];
        strcpy(xml_status_guard, device_status.status_guard);/*保存当前布防状态*/
        csenn_eXosip_callback.csenn_eXosip_getDeviceStatus(&device_status);/*获取设备当前状态*/
        strcpy(device_status.status_guard, xml_status_guard);/*恢复当前布防状态*/
        snprintf(rsp_xml_body, 4096, "<?xml version=\"1.0\"?>\r\n"
                                     "<Response>\r\n"
                                     "<CmdType>DeviceStatus</CmdType>\r\n"/*命令类型*/
                                     "<SN>%s</SN>\r\n"/*命令序列号*/
                                     "<DeviceID>%s</DeviceID>\r\n"/*目标设备/区域/系统的编码*/
                                     "<Result>OK</Result>\r\n"/*查询结果标志*/
                                     "<Online>%s</Online>\r\n"/*是否在线/ONLINE/OFFLINE*/
                                     "<Status>%s</Status>\r\n"/*是否正常工作*/
                                     "<Encode>%s</Encode>\r\n"/*是否编码*/
                                     "<Record>%s</Record>\r\n"/*是否录像*/
                                     "<DeviceTime>%s</DeviceTime>\r\n"/*设备时间和日期*/
                                     "<Alarmstatus Num=\"1\">\r\n"/*报警设备状态列表*/
                                     "<Item>\r\n"
                                     "<DeviceID>%s</DeviceID>\r\n"/*报警设备编码*/
                                     "<DutyStatus>%s</DutyStatus>\r\n"/*报警设备状态*/
                                     "</Item>\r\n"
                                     "</Alarmstatus>\r\n"
                                     "</Response>\r\n",
                                     xml_cmd_sn,
                                     xml_device_id,
                                     device_status.status_online,
                                     device_status.status_ok,
                                     device_info.device_encode,
                                     device_info.device_record,
                                     device_status.status_time,
                                     xml_device_id,
                                     device_status.status_guard);
    }
    else if(0 == strcmp(xml_cmd_type, "RecordInfo"))/*设备视音频文件检索*/
    {
        /*录像文件检索*/
        char xml_file_path[30];
        char xml_start_time[30];
        char xml_end_time[30];
        char xml_recorder_id[30];
        char item_start_time[30];
        char item_end_time[30];
        char rsp_item_body[4096];
        int  record_list_num = 0;
        int  record_list_ret = 0;

        memset(xml_file_path, 0, 30);
        memset(xml_start_time, 0, 30);
        memset(xml_end_time, 0, 30);
        memset(xml_recorder_id, 0, 30);
        memset(item_start_time, 0, 30);
        memset(item_end_time, 0, 30);
        memset(rsp_item_body, 0, 4096);
        printf("**********RECORD INFO START**********\r\n");
        p_str_begin = strstr(p_xml_body, "<FilePath>");/*查找字符串"<FilePath>"*/
        p_str_end = strstr(p_xml_body, "</FilePath>");
        memcpy(xml_file_path, p_str_begin + 10, p_str_end - p_str_begin - 10);/*保存<FilePath>到xml_file_path*/
        printf("<FilePath>:%s\r\n", xml_file_path);

        p_str_begin = strstr(p_xml_body, "<StartTime>");/*查找字符串"<StartTime>"*/
        p_str_end = strstr(p_xml_body, "</StartTime>");
        memcpy(xml_start_time, p_str_begin + 11, p_str_end - p_str_begin - 11);/*保存<StartTime>到xml_start_time*/
        printf("<StartTime>:%s\r\n", xml_start_time);

        p_str_begin = strstr(p_xml_body, "<EndTime>");/*查找字符串"<EndTime>"*/
        p_str_end = strstr(p_xml_body, "</EndTime>");
        memcpy(xml_end_time, p_str_begin + 9, p_str_end - p_str_begin - 9);/*保存<EndTime>到xml_end_time*/
        printf("<EndTime>:%s\r\n", xml_end_time);

        p_str_begin = strstr(p_xml_body, "<RecorderID>");/*查找字符串"<RecorderID>"*/
        p_str_end = strstr(p_xml_body, "</RecorderID>");
        memcpy(xml_recorder_id, p_str_begin + 12, p_str_end - p_str_begin - 12);/*保存<RecorderID>到xml_recorder_id*/
        printf("<RecorderID>:%s\r\n", xml_recorder_id);
        printf("***********RECORD INFO END***********\r\n");
        for(;;)
        {
            record_list_ret = csenn_eXosip_callback.csenn_eXosip_getRecordTime(xml_start_time, xml_end_time, item_start_time, item_end_time);
            if(0 > record_list_ret)
            {
                break;
            }
            else
            {
                char temp_body[1024];
                memset(temp_body, 0, 1024);
                snprintf(temp_body, 1024, "<Item>\r\n"
                                          "<DeviceID>%s</DeviceID>\r\n"/*设备/区域编码*/
                                          "<Name>%s</Name>\r\n"/*设备/区域名称*/
                                          "<FilePath>%s</FilePath>\r\n"/*文件路径名*/
                                          "<Address>Address1</Address>\r\n"/*录像地址*/
                                          "<StartTime>%s</StartTime>\r\n"/*录像开始时间*/
                                          "<EndTime>%s</EndTime>\r\n"/*录像结束时间*/
                                          "<Secrecy>0</Secrecy>\r\n"/*保密属性/0为不涉密/1为涉密*/
                                          "<Type>time</Type>\r\n"/*录像产生类型*/
                                          "<RecorderID>%s</RecorderID>\r\n"/*录像触发者ID*/
                                          "</Item>\r\n",
                                          xml_device_id,
                                          device_info.device_name,
                                          xml_file_path,
                                          item_start_time,
                                          item_end_time,
                                          xml_recorder_id);
                strcat(rsp_item_body, temp_body);
                record_list_num++;
                if(0 == record_list_ret)
                {
                    break;
                }
            }
        }
        if(0 >= record_list_num)/*未检索到任何设备视音频文件*/
        {
            return;
        }
        snprintf(rsp_xml_body, 4096, "<?xml version=\"1.0\"?>\r\n"
                                     "<Response>\r\n"
                                     "<CmdType>RecordInfo</CmdType>\r\n"/*命令类型*/
                                     "<SN>%s</SN>\r\n"/*命令序列号*/
                                     "<DeviceID>%s</DeviceID>\r\n"/*设备/区域编码*/
                                     "<Name>%s</Name>\r\n"/*设备/区域名称*/
                                     "<SumNum>%d</SumNum>\r\n"/*查询结果总数*/
                                     "<RecordList Num=\"%d\">\r\n"/*文件目录项列表*/
                                     "%s\r\n"
                                     "</RecordList>\r\n"
                                     "</Response>\r\n",
                                     xml_cmd_sn,
                                     xml_device_id,
                                     device_info.device_name,
                                     record_list_num,
                                     record_list_num,
                                     rsp_item_body);
    }
    else/*CmdType为不支持的类型*/
    {
        printf("**********OTHER TYPE START**********\r\n");
        printf("***********OTHER TYPE END***********\r\n");
        return;
    }
    osip_message_set_body(rsp_msg, rsp_xml_body, strlen(rsp_xml_body));
    osip_message_set_content_type(rsp_msg, "Application/MANSCDP+xml");
    eXosip_message_send_request(g_p_eXosip_context,rsp_msg);/*回复"MESSAGE"请求*/
    printf("eXosip_message_send_request success!\r\n");
}
/*解析INVITE的SDP消息体，同时保存全局INVITE连接ID和全局会话ID*/
void csenn_eXosip_paraseInviteBody(eXosip_event_t *p_event)
{
    sdp_message_t *sdp_msg = NULL;
    char *media_sever_ip = NULL;
    char *media_sever_port = NULL;

    sdp_msg = eXosip_get_remote_sdp(g_p_eXosip_context,p_event->did);
    if(sdp_msg == NULL)
    {
        printf("eXosip_get_remote_sdp NULL!\r\n");
        return;
    }
    printf("eXosip_get_remote_sdp success!\r\n");

    g_call_id = p_event->cid;/*保存全局INVITE连接ID*/
    /*实时点播*/
    if(0 == strcmp(sdp_msg->s_name, "Play"))
    {
        g_did_realPlay = p_event->did;/*保存全局会话ID：实时视音频点播*/
    }
    /*回放*/
    else if(0 == strcmp(sdp_msg->s_name, "Playback"))
    {
        g_did_backPlay = p_event->did;/*保存全局会话ID：历史视音频回放*/
    }
    /*下载*/
    else if(0 == strcmp(sdp_msg->s_name, "Download"))
    {
        g_did_fileDown = p_event->did;/*保存全局会话ID：视音频文件下载*/
    }
    /*从SIP服务器发过来的INVITE请求的o字段或c字段中获取媒体服务器的IP地址与端口*/
    media_sever_ip = sdp_message_o_addr_get(sdp_msg);/*媒体服务器IP地址*/
    media_sever_port = sdp_message_m_port_get(sdp_msg, 0);/*媒体服务器IP端口*/
    printf("%s->%s:%s\r\n", sdp_msg->s_name, media_sever_ip, media_sever_port);
    csenn_eXosip_callback.csenn_eXosip_mediaControl(sdp_msg->s_name, media_sever_ip, media_sever_port);
}
/*解析INFO的RTSP消息体*/
void csenn_eXosip_paraseInfoBody(eXosip_event_t *p_event)
{
    osip_body_t *p_msg_body = NULL;
    char *p_rtsp_body = NULL;
    char *p_str_begin = NULL;
    char *p_str_end = NULL;
    char *p_strstr = NULL;
    char rtsp_scale[10];
    char rtsp_range_begin[10];
    char rtsp_range_end[10];
    char rtsp_pause_time[10];

    memset(rtsp_scale, 0, 10);
    memset(rtsp_range_begin, 0, 10);
    memset(rtsp_range_end, 0, 10);
    memset(rtsp_pause_time, 0, 10);

    osip_message_get_body(p_event->request, 0, &p_msg_body);
    if(NULL == p_msg_body)
    {
        printf("osip_message_get_body null!\r\n");
        return;
    }
    p_rtsp_body = p_msg_body->body;
    printf("osip_message_get_body success!\r\n");

    p_strstr = strstr(p_rtsp_body, "PLAY");
    if(NULL != p_strstr)/*查找到字符串"PLAY"*/
    {
        /*播放速度*/
        p_str_begin = strstr(p_rtsp_body, "Scale:");/*查找字符串"Scale:"*/
        p_str_end = strstr(p_rtsp_body, "Range:");
        memcpy(rtsp_scale, p_str_begin + 6, p_str_end - p_str_begin - 6);/*保存Scale到rtsp_scale*/
        printf("PlayScale:%s\r\n", rtsp_scale);
        /*播放范围*/
        p_str_begin = strstr(p_rtsp_body, "npt=");/*查找字符串"npt="*/
        p_str_end = strstr(p_rtsp_body, "-");
        memcpy(rtsp_range_begin, p_str_begin + 4, p_str_end - p_str_begin - 4);/*保存RangeBegin到rtsp_range_begin*/
        printf("PlayRangeBegin:%s\r\n", rtsp_range_begin);
        p_str_begin = strstr(p_rtsp_body, "-");/*查找字符串"-"*/
        strcpy(rtsp_range_end, p_str_begin + 1);/*保存RangeEnd到rtsp_range_end*/
        printf("PlayRangeEnd:%s\r\n", rtsp_range_end);
        csenn_eXosip_callback.csenn_eXosip_playControl("PLAY", rtsp_scale, NULL, rtsp_range_begin, rtsp_range_end);
        return;
    }

    p_strstr = strstr(p_rtsp_body, "PAUSE");
    if(NULL != p_strstr)/*查找到字符串"PAUSE"*/
    {
        /*暂停时间*/
        p_str_begin = strstr(p_rtsp_body, "PauseTime:");/*查找字符串"PauseTime:"*/
        strcpy(rtsp_pause_time, p_str_begin + 10);/*保存PauseTime到rtsp_pause_time*/
        printf("PauseTime:%3s\r\n", rtsp_pause_time);
        csenn_eXosip_callback.csenn_eXosip_playControl("PAUSE", NULL, rtsp_pause_time, NULL, NULL);
        return;
    }

    printf("can`t find string PLAY or PAUSE!");
}

/*检测并打印事件*/
void csenn_eXosip_printEvent(eXosip_event_t *p_event)
{
    osip_message_t *clone_event = NULL;
    size_t length = 0;
    char *message = NULL;

    printf("\r\n##############################################################\r\n");
    switch(p_event->type)
    {
        //case EXOSIP_REGISTRATION_NEW:
        //    printf("EXOSIP_REGISTRATION_NEW\r\n");
        //    break;
        case EXOSIP_REGISTRATION_SUCCESS:
            printf("EXOSIP_REGISTRATION_SUCCESS\r\n");
            break;
        case EXOSIP_REGISTRATION_FAILURE:
            printf("EXOSIP_REGISTRATION_FAILURE\r\n");
            break;
        //case EXOSIP_REGISTRATION_REFRESHED:
        //    printf("EXOSIP_REGISTRATION_REFRESHED\r\n");
        //    break;
        //case EXOSIP_REGISTRATION_TERMINATED:
        //    printf("EXOSIP_REGISTRATION_TERMINATED\r\n");
        //    break;
        case EXOSIP_CALL_INVITE:
            printf("EXOSIP_CALL_INVITE\r\n");
            break;
        case EXOSIP_CALL_REINVITE:
            printf("EXOSIP_CALL_REINVITE\r\n");
            break;
        case EXOSIP_CALL_NOANSWER:
            printf("EXOSIP_CALL_NOANSWER\r\n");
            break;
        case EXOSIP_CALL_PROCEEDING:
            printf("EXOSIP_CALL_PROCEEDING\r\n");
            break;
        case EXOSIP_CALL_RINGING:
            printf("EXOSIP_CALL_RINGING\r\n");
            break;
        case EXOSIP_CALL_ANSWERED:
            printf("EXOSIP_CALL_ANSWERED\r\n");
            break;
        case EXOSIP_CALL_REDIRECTED:
            printf("EXOSIP_CALL_REDIRECTED\r\n");
            break;
        case EXOSIP_CALL_REQUESTFAILURE:
            printf("EXOSIP_CALL_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_CALL_SERVERFAILURE:
            printf("EXOSIP_CALL_SERVERFAILURE\r\n");
            break;
        case EXOSIP_CALL_GLOBALFAILURE:
            printf("EXOSIP_CALL_GLOBALFAILURE\r\n");
            break;
        case EXOSIP_CALL_ACK:
            printf("EXOSIP_CALL_ACK\r\n");
            break;
        case EXOSIP_CALL_CANCELLED:
            printf("EXOSIP_CALL_CANCELLED\r\n");
            break;
        //case EXOSIP_CALL_TIMEOUT:
        //    printf("EXOSIP_CALL_TIMEOUT\r\n");
        //    break;
        case EXOSIP_CALL_MESSAGE_NEW:
            printf("EXOSIP_CALL_MESSAGE_NEW\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_PROCEEDING:
            printf("EXOSIP_CALL_MESSAGE_PROCEEDING\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_ANSWERED:
            printf("EXOSIP_CALL_MESSAGE_ANSWERED\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_REDIRECTED:
            printf("EXOSIP_CALL_MESSAGE_REDIRECTED\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_REQUESTFAILURE:
            printf("EXOSIP_CALL_MESSAGE_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_SERVERFAILURE:
            printf("EXOSIP_CALL_MESSAGE_SERVERFAILURE\r\n");
            break;
        case EXOSIP_CALL_MESSAGE_GLOBALFAILURE:
            printf("EXOSIP_CALL_MESSAGE_GLOBALFAILURE\r\n");
            break;
        case EXOSIP_CALL_CLOSED:
            printf("EXOSIP_CALL_CLOSED\r\n");
            break;
        case EXOSIP_CALL_RELEASED:
            printf("EXOSIP_CALL_RELEASED\r\n");
            break;
        case EXOSIP_MESSAGE_NEW:
            printf("EXOSIP_MESSAGE_NEW\r\n");
            break;
        case EXOSIP_MESSAGE_PROCEEDING:
            printf("EXOSIP_MESSAGE_PROCEEDING\r\n");
            break;
        case EXOSIP_MESSAGE_ANSWERED:
            printf("EXOSIP_MESSAGE_ANSWERED\r\n");
            break;
        case EXOSIP_MESSAGE_REDIRECTED:
            printf("EXOSIP_MESSAGE_REDIRECTED\r\n");
            break;
        case EXOSIP_MESSAGE_REQUESTFAILURE:
            printf("EXOSIP_MESSAGE_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_MESSAGE_SERVERFAILURE:
            printf("EXOSIP_MESSAGE_SERVERFAILURE\r\n");
            break;
        case EXOSIP_MESSAGE_GLOBALFAILURE:
            printf("EXOSIP_MESSAGE_GLOBALFAILURE\r\n");
            break;
        //case EXOSIP_SUBSCRIPTION_UPDATE:
        //    printf("EXOSIP_SUBSCRIPTION_UPDATE\r\n");
        //    break;
        //case EXOSIP_SUBSCRIPTION_CLOSED:
        //    printf("EXOSIP_SUBSCRIPTION_CLOSED\r\n");
        //    break;
        case EXOSIP_SUBSCRIPTION_NOANSWER:
            printf("EXOSIP_SUBSCRIPTION_NOANSWER\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_PROCEEDING:
            printf("EXOSIP_SUBSCRIPTION_PROCEEDING\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_ANSWERED:
            printf("EXOSIP_SUBSCRIPTION_ANSWERED\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_REDIRECTED:
            printf("EXOSIP_SUBSCRIPTION_REDIRECTED\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
            printf("EXOSIP_SUBSCRIPTION_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_SERVERFAILURE:
            printf("EXOSIP_SUBSCRIPTION_SERVERFAILURE\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_GLOBALFAILURE:
            printf("EXOSIP_SUBSCRIPTION_GLOBALFAILURE\r\n");
            break;
        case EXOSIP_SUBSCRIPTION_NOTIFY:
            printf("EXOSIP_SUBSCRIPTION_NOTIFY\r\n");
            break;
        //case EXOSIP_SUBSCRIPTION_RELEASED:
        //    printf("EXOSIP_SUBSCRIPTION_RELEASED\r\n");
        //    break;
        case EXOSIP_IN_SUBSCRIPTION_NEW:
            printf("EXOSIP_IN_SUBSCRIPTION_NEW\r\n");
            break;
        //case EXOSIP_IN_SUBSCRIPTION_RELEASED:
        //    printf("EXOSIP_IN_SUBSCRIPTION_RELEASED\r\n");
        //    break;
        case EXOSIP_NOTIFICATION_NOANSWER:
            printf("EXOSIP_NOTIFICATION_NOANSWER\r\n");
            break;
        case EXOSIP_NOTIFICATION_PROCEEDING:
            printf("EXOSIP_NOTIFICATION_PROCEEDING\r\n");
            break;
        case EXOSIP_NOTIFICATION_ANSWERED:
            printf("EXOSIP_NOTIFICATION_ANSWERED\r\n");
            break;
        case EXOSIP_NOTIFICATION_REDIRECTED:
            printf("EXOSIP_NOTIFICATION_REDIRECTED\r\n");
            break;
        case EXOSIP_NOTIFICATION_REQUESTFAILURE:
            printf("EXOSIP_NOTIFICATION_REQUESTFAILURE\r\n");
            break;
        case EXOSIP_NOTIFICATION_SERVERFAILURE:
            printf("EXOSIP_NOTIFICATION_SERVERFAILURE\r\n");
            break;
        case EXOSIP_NOTIFICATION_GLOBALFAILURE:
            printf("EXOSIP_NOTIFICATION_GLOBALFAILURE\r\n");
            break;
        case EXOSIP_EVENT_COUNT:
            printf("EXOSIP_EVENT_COUNT\r\n");
            break;
        default:
            printf("..................\r\n");
            break;
    }
    osip_message_clone(p_event->request, &clone_event);
    osip_message_to_str(clone_event, &message, &length);
    printf("%s\r\n", message);
    printf("##############################################################\r\n\r\n");
}

/*消息循环处理*/
void csenn_eXosip_processEvent(void)
{
    eXosip_event_t *g_event = NULL;/*消息事件*/
    osip_message_t *g_answer = NULL;/*请求的确认型应答*/

    while(1)
    {
        /*等待新消息的到来*/
        g_event = eXosip_event_wait(g_p_eXosip_context,0, 200);/*侦听消息的到来*/
        eXosip_lock(g_p_eXosip_context);
        eXosip_default_action(g_p_eXosip_context,g_event);
        eXosip_automatic_refresh(g_p_eXosip_context);/*Refresh REGISTER and SUBSCRIBE before the expiration delay*/
        eXosip_unlock(g_p_eXosip_context);
        if(NULL == g_event)
        {
            continue;
        }
        csenn_eXosip_printEvent(g_event);
        /*处理感兴趣的消息*/
        switch(g_event->type)
        {
            /*即时消息：通信双方无需事先建立连接*/
            case EXOSIP_MESSAGE_NEW:/*MESSAGE:MESSAGE*/
            {
                printf("\r\n<EXOSIP_MESSAGE_NEW>\r\n");
                if(MSG_IS_MESSAGE(g_event->request))/*使用MESSAGE方法的请求*/
                {
                    /*设备控制*/
                    /*报警事件通知和分发：报警通知响应*/
                    /*网络设备信息查询*/
                    /*设备视音频文件检索*/
                    printf("<MSG_IS_MESSAGE>\r\n");
                    eXosip_lock(g_p_eXosip_context);
                    eXosip_message_build_answer(g_p_eXosip_context,g_event->tid, 200, &g_answer);/*Build default Answer for request*/
                    eXosip_message_send_answer(g_p_eXosip_context,g_event->tid, 200, g_answer);/*按照规则回复200OK*/
                    printf("eXosip_message_send_answer success!\r\n");
                    eXosip_unlock(g_p_eXosip_context);
                    csenn_eXosip_paraseMsgBody(g_event);/*解析MESSAGE的XML消息体，同时保存全局会话ID*/
                }
            }
            break;
            /*即时消息回复的200OK*/
            case EXOSIP_MESSAGE_ANSWERED:/*200OK*/
            {
                /*设备控制*/
                /*报警事件通知和分发：报警通知*/
                /*网络设备信息查询*/
                /*设备视音频文件检索*/
                printf("\r\n<EXOSIP_MESSAGE_ANSWERED>\r\n");
            }
            break;
            /*以下类型的消息都必须事先建立连接*/
            case EXOSIP_CALL_INVITE:/*INVITE*/
            {
                printf("\r\n<EXOSIP_CALL_INVITE>\r\n");
                if(MSG_IS_INVITE(g_event->request))/*使用INVITE方法的请求*/
                {
                    /*实时视音频点播*/
                    /*历史视音频回放*/
                    /*视音频文件下载*/
                    osip_message_t *asw_msg = NULL;/*请求的确认型应答*/
                    char sdp_body[4096];

                    memset(sdp_body, 0, 4096);
                    printf("<MSG_IS_INVITE>\r\n");

                    eXosip_lock(g_p_eXosip_context);
                    if(0 != eXosip_call_build_answer(g_p_eXosip_context,g_event->tid, 200, &asw_msg))/*Build default Answer for request*/
                    {
                        eXosip_call_send_answer(g_p_eXosip_context,g_event->tid, 603, NULL);
                        eXosip_unlock(g_p_eXosip_context);
                        printf("eXosip_call_build_answer error!\r\n");
                        break;
                    }
                    eXosip_unlock(g_p_eXosip_context);
                    snprintf(sdp_body, 4096, "v=0\r\n"/*协议版本*/
                                             "o=%s 0 0 IN IP4 %s\r\n"/*会话源*//*用户名/会话ID/版本/网络类型/地址类型/地址*/
                                             "s=Embedded IPC\r\n"/*会话名*/
                                             "c=IN IP4 %s\r\n"/*连接信息*//*网络类型/地址信息/多点会议的地址*/
                                             "t=0 0\r\n"/*时间*//*开始时间/结束时间*/
                                             "m=video %s RTP/AVP 96\r\n"/*媒体/端口/传送层协议/格式列表*/
                                             "a=sendonly\r\n"/*收发模式*/
                                             "a=rtpmap:96 H264/90000\r\n"/*净荷类型/编码名/时钟速率*/
                                             "a=username:%s\r\n"
                                             "a=password:%s\r\n"
                                             "y=100000001\r\n"
                                             "f=\r\n",
                                             device_info.ipc_id,
                                             device_info.ipc_ip,
                                             device_info.ipc_ip,
                                             device_info.ipc_port,
                                             device_info.ipc_id,
                                             device_info.ipc_pwd);
                    eXosip_lock(g_p_eXosip_context);
                    osip_message_set_body(asw_msg, sdp_body, strlen(sdp_body));/*设置SDP消息体*/
                    osip_message_set_content_type(asw_msg, "application/sdp");
                    eXosip_call_send_answer(g_p_eXosip_context,g_event->tid, 200, asw_msg);/*按照规则回复200OK with SDP*/
                    printf("eXosip_call_send_answer success!\r\n");
                    eXosip_unlock(g_p_eXosip_context);
                }
            }
            break;
            case EXOSIP_CALL_ACK:/*ACK*/
            {
                /*实时视音频点播*/
                /*历史视音频回放*/
                /*视音频文件下载*/
                printf("\r\n<EXOSIP_CALL_ACK>\r\n");/*收到ACK才表示成功建立连接*/
                csenn_eXosip_paraseInviteBody(g_event);/*解析INVITE的SDP消息体，同时保存全局INVITE连接ID和全局会话ID*/
            }
            break;
            case EXOSIP_CALL_CLOSED:/*BEY*/
            {
                /*实时视音频点播*/
                /*历史视音频回放*/
                /*视音频文件下载*/
                printf("\r\n<EXOSIP_CALL_CLOSED>\r\n");
                if(MSG_IS_BYE(g_event->request))
                {
                    printf("<MSG_IS_BYE>\r\n");
                    if((0 != g_did_realPlay) && (g_did_realPlay == g_event->did))/*实时视音频点播*/
                    {
                        /*关闭：实时视音频点播*/
                        printf("realPlay closed success!\r\n");
                        g_did_realPlay = 0;
                    }
                    else if((0 != g_did_backPlay) && (g_did_backPlay == g_event->did))/*历史视音频回放*/
                    {
                        /*关闭：历史视音频回放*/
                        printf("backPlay closed success!\r\n");
                        g_did_backPlay = 0;
                    }
                    else if((0 != g_did_fileDown) && (g_did_fileDown == g_event->did))/*视音频文件下载*/
                    {
                        /*关闭：视音频文件下载*/
                        printf("fileDown closed success!\r\n");
                        g_did_fileDown = 0;
                    }
                    if((0 != g_call_id) && (0 == g_did_realPlay) && (0 == g_did_backPlay) && (0 == g_did_fileDown))/*设置全局INVITE连接ID*/
                    {
                        printf("call closed success!\r\n");
                        g_call_id = 0;
                    }
                }
            }
            break;
            case EXOSIP_CALL_MESSAGE_NEW:/*MESSAGE:INFO*/
            {
                /*历史视音频回放*/
                printf("\r\n<EXOSIP_CALL_MESSAGE_NEW>\r\n");
                if(MSG_IS_INFO(g_event->request))
                {
                    osip_body_t *msg_body = NULL;

                    printf("<MSG_IS_INFO>\r\n");
                    osip_message_get_body(g_event->request, 0, &msg_body);
                    if(NULL != msg_body)
                    {
                        eXosip_call_build_answer(g_p_eXosip_context,g_event->tid, 200, &g_answer);/*Build default Answer for request*/
                        eXosip_call_send_answer(g_p_eXosip_context,g_event->tid, 200, g_answer);/*按照规则回复200OK*/
                        printf("eXosip_call_send_answer success!\r\n");
                        csenn_eXosip_paraseInfoBody(g_event);/*解析INFO的RTSP消息体*/
                    }
                }
            }
            break;
            case EXOSIP_CALL_MESSAGE_ANSWERED:/*200OK*/
            {
                /*历史视音频回放*/
                /*文件结束时发送MESSAGE(File to end)的应答*/
                printf("\r\n<EXOSIP_CALL_MESSAGE_ANSWERED>\r\n");
            }
            break;
            /*其它不感兴趣的消息*/
            default:
            {
                printf("\r\n<OTHER>\r\n");
                printf("eXosip event type:%d\n", g_event->type);
            }
            break;
        }
    }
}

void csenn_eXosip_launch(void)
{
    char eXosip_server_id[30] = "34020000002000000001";
    char eXosip_server_ip[20] = "192.168.1.178";
    char eXosip_server_port[10] = "5060";
    char eXosip_ipc_id[30] = "34020000001320000001";
    char eXosip_ipc_pwd[20] = "12345678";
    char eXosip_ipc_ip[20] = "192.168.1.144";
    char eXosip_ipc_port[10] = "6000";

    char eXosip_device_name[30] = "IPC";
    char eXosip_device_manufacturer[30] = "csenn";
    char eXosip_device_model[30] = "TEST001";
    char eXosip_device_firmware[30] = "V1.0";
    char eXosip_device_encode[10] = "ON";
    char eXosip_device_record[10] = "OFF";

    char eXosip_status_on[10] = "ON";
    char eXosip_status_ok[10] = "OK";
    char eXosip_status_online[10] = "ONLINE";
    char eXosip_status_guard[10] = "OFFDUTY";
    char eXosip_status_time[30] = "2013-05-08T13:12:20";

    device_info.server_id = eXosip_server_id;
    device_info.server_ip = eXosip_server_ip;
    device_info.server_port = eXosip_server_port;
    device_info.ipc_id = eXosip_ipc_id;
    device_info.ipc_pwd = eXosip_ipc_pwd;
    device_info.ipc_ip = eXosip_ipc_ip;
    device_info.ipc_port = eXosip_ipc_port;

    device_info.device_name = eXosip_device_name;
    device_info.device_manufacturer = eXosip_device_manufacturer;
    device_info.device_model = eXosip_device_model;
    device_info.device_firmware = eXosip_device_firmware;
    device_info.device_encode = eXosip_device_encode;
    device_info.device_record = eXosip_device_record;

    device_status.status_on = eXosip_status_on;
    device_status.status_ok = eXosip_status_ok;
    device_status.status_online = eXosip_status_online;
    device_status.status_guard = eXosip_status_guard;
    device_status.status_time = eXosip_status_time;

    csenn_eXosip_callback.csenn_eXosip_getDeviceInfo(&device_info);
    while(csenn_eXosip_init());
    while(csenn_eXosip_register(3600));
    csenn_eXosip_processEvent();
}

/******************* (C) COPYRIGHT 2013 CSENN ******************END OF FILE****/
