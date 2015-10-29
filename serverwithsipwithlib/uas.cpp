// uas.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <eXosip2/eXosip.h>
#include <stdio.h>
#include <stdlib.h>
#include <Winsock2.h>
/*
＃include <netinet/in.h>
＃include <sys/socket.h>
＃include <sys/types.h>*/
#pragma comment(lib, "osip2.lib")
#pragma comment(lib, "osipparser2.lib")
#pragma comment(lib, "eXosip.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Dnsapi.lib")
#pragma comment(lib, "ws2_32.lib")
int
main(int argc, char *argv[])
{
    eXosip_event_t *je = NULL;
    osip_message_t *ack = NULL;
    osip_message_t *invite = NULL;
    osip_message_t *answer = NULL;
    sdp_message_t *remote_sdp = NULL;
    int call_id, dialog_id;
    int i, j;
    int id;
    char *sour_call = "sip:24@172.17.0.36";
    char *dest_call = "sip:24@172.17.0.36:5061";//client ip
    char command;
    char tmp[4096];
    char localip[128];
    int pos = 0;
    eXosip_t* p_eXosip_t = eXosip_malloc();
    //初始化sip
    i = eXosip_init(p_eXosip_t);
    if(i != 0)
    {
        printf("Can't initialize eXosip!\n");
        return -1;
    }
    else
    {
        printf("eXosip_init successfully!\n");
    }
    i = eXosip_listen_addr(p_eXosip_t, IPPROTO_UDP, NULL, 5060, AF_INET, 0);
    if(i != 0)
    {
        eXosip_quit(p_eXosip_t);
        fprintf(stderr, "eXosip_listen_addr error!\nCouldn't initialize transport layer!\n");
    }
    for(;;)
    {
        //侦听是否有消息到来
        je = eXosip_event_wait(p_eXosip_t,0, 50);
        //协议栈带有此语句,具体作用未知
        eXosip_lock(p_eXosip_t);
        eXosip_default_action(p_eXosip_t,je);
        eXosip_automatic_refresh(p_eXosip_t);
        eXosip_unlock(p_eXosip_t);
        if(je == NULL)//没有接收到消息
      continue;
        // printf ("the cid is %s, did is %s\n", je->did, je->cid);
        switch(je->type)
        {
            case EXOSIP_MESSAGE_NEW://新的消息到来
                printf(" EXOSIP_MESSAGE_NEW!\n");
                if(MSG_IS_MESSAGE(je->request))//如果接受到的消息类型是MESSAGE
                {
                    {
                        osip_body_t *body;
                        osip_message_get_body(je->request, 0, &body);
                        printf("I get the msg is: %s\n", body->body);
                        //printf ("the cid is %s, did is %s\n", je->did, je->cid);
                    }
                    //按照规则，需要回复OK信息
                    eXosip_message_build_answer(p_eXosip_t,je->tid, 200, &answer);
                    eXosip_message_send_answer(p_eXosip_t,je->tid, 200, answer);
                }
                break;
            case EXOSIP_CALL_INVITE:
                //得到接收到消息的具体信息
                printf("Received a INVITE msg from %s:%s, UserName is %s, password is %s\n", je->request->req_uri->host,
              je->request->req_uri->port, je->request->req_uri->username, je->request->req_uri->password);
                //得到消息体,认为该消息就是SDP格式.
                remote_sdp = eXosip_get_remote_sdp(p_eXosip_t,je->did);
                call_id = je->cid;
                dialog_id = je->did;

                eXosip_lock(p_eXosip_t);
                eXosip_call_send_answer(p_eXosip_t,je->tid, 180, NULL);
                i = eXosip_call_build_answer(p_eXosip_t,je->tid, 200, &answer);
                if(i != 0)
                {
                    printf("This request msg is invalid!Cann't response!\n");
                    eXosip_call_send_answer(p_eXosip_t,je->tid, 400, NULL);
                }
                else
                {
                    _snprintf(tmp, 4096,
                         "v=0\r\n"
                         "o=anonymous 0 0 IN IP4 0.0.0.0\r\n"
                         "t=1 10\r\n"
                         "a=username:rainfish\r\n"
                         "a=password:123\r\n");

                    //设置回复的SDP消息体,下一步计划分析消息体
                    //没有分析消息体，直接回复原来的消息，这一块做的不好。
                    osip_message_set_body(answer, tmp, strlen(tmp));
                    osip_message_set_content_type(answer, "application/sdp");

                    eXosip_call_send_answer(p_eXosip_t,je->tid, 200, answer);
                    printf("send 200 over!\n");
                }
                eXosip_unlock(p_eXosip_t);

                //显示出在sdp消息体中的attribute 的内容,里面计划存放我们的信息
                printf("the INFO is :\n");
                while(!osip_list_eol(&(remote_sdp->a_attributes), pos))
                {
                    sdp_attribute_t *at;

                    at = (sdp_attribute_t *)osip_list_get(&remote_sdp->a_attributes, pos);
                    printf("%s : %s\n", at->a_att_field, at->a_att_value);//这里解释了为什么在SDP消息体中属性a里面存放必须是两列

                    pos++;
                }
                break;
            case EXOSIP_CALL_ACK:
                printf("ACK recieved!\n");
                // printf ("the cid is %s, did is %s\n", je->did, je->cid); 
                break;
            case EXOSIP_CALL_CLOSED:
                printf("the remote hold the session!\n");
                // eXosip_call_build_ack(dialog_id, &ack);
                //eXosip_call_send_ack(dialog_id, ack); 
                i = eXosip_call_build_answer(p_eXosip_t,je->tid, 200, &answer);
                if(i != 0)
                {
                    printf("This request msg is invalid!Cann't response!\n");
                    eXosip_call_send_answer(p_eXosip_t, je->tid, 400, NULL);

                }
                else
                {
                    eXosip_call_send_answer(p_eXosip_t, je->tid, 200, answer);
                    printf("bye send 200 over!\n");
                }
                break;
            case EXOSIP_CALL_MESSAGE_NEW://至于该类型和EXOSIP_MESSAGE_NEW的区别，源代码这么解释的
                /*
                // request related events within calls (except INVITE)
                EXOSIP_CALL_MESSAGE_NEW,            < announce new incoming request.
                // response received for request outside calls
                EXOSIP_MESSAGE_NEW,            < announce new incoming request.
                我也不是很明白，理解是：EXOSIP_CALL_MESSAGE_NEW是一个呼叫中的新的消息到来，比如ring trying都算，所以在接受到后必须判断
                该消息类型，EXOSIP_MESSAGE_NEW而是表示不是呼叫内的消息到来。
                该解释有不妥地方，仅供参考。
                */
                printf(" EXOSIP_CALL_MESSAGE_NEW\n");
                if(MSG_IS_INFO(je->request)) //如果传输的是INFO方法
                {
                    eXosip_lock(p_eXosip_t);
                    i = eXosip_call_build_answer(p_eXosip_t, je->tid, 200, &answer);
                    if(i == 0)
                    {
                        eXosip_call_send_answer(p_eXosip_t, je->tid, 200, answer);
                    }
                    eXosip_unlock(p_eXosip_t);
                    {
                        osip_body_t *body;
                        osip_message_get_body(je->request, 0, &body);
                        printf("the body is %s\n", body->body);
                    }
                }
                break;
            default:
                printf("Could not parse the msg!\n");
        }
    }

    osip_free(p_eXosip_t);
}

