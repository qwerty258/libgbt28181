#include <libGBT28181Client.h>
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define _DEBUG_FOR_HANDLE 0

#ifdef _MSC_VER
#define snprintf _snprintf
#endif // _MSC_VER


void query_deviceInfo_callback(char* device_ID, char* device_type, char* manufacturer, char* model, char* firmware, uint64_t max_camera, uint64_t max_alarm)
{
    printf("device_ID   : %s\n", device_ID);
    printf("device_type : %s\n", device_type);
    printf("manufacturer: %s\n", manufacturer);
    printf("model       : %s\n", model);
    printf("firmware    : %s\n", firmware);
    printf("max_camera  : %u\n", max_camera);
    printf("max_alarm   : %u\n", max_alarm);
}

void query_device_status_callback(char* deviceID, MANSCDP_on_off_line online, MANSCDP_result_type status)
{
    printf("device_ID: %s\n", deviceID);
    switch(online)
    {
        case MANSCDP_ONLINE:
            printf("online   : ONLINE\n");
            break;
        case MANSCDP_OFFLINE:
            printf("online   : OFFLINE\n");
            break;
        case MANSCDP_UNKNOWN_ON_OFF_LINE:
            printf("online   : UNKNOWN\n");
            break;
    }
    switch(status)
    {
        case MANSCDP_OK:
            printf("status   : OK\n");
            break;
        case MANSCDP_ERROR:
            printf("status   : ERROR\n");
            break;
        case MANSCDP_UNKNOWN_RESULT_TYPE:
            printf("status   : UNKNOWN\n");
            break;
    }
}

void query_device_status_callback(char* deviceID, uint64_t sum_num, MANSCDP_device* p_MANSCDP_device_array)
{
    printf("device_ID: %s\n", deviceID);
    printf("count    : %u\n", sum_num);
    for(uint64_t i = 0; i < sum_num; i++)
    {
        printf("device number: %u\n", i + 1);
        printf("DeviceID    : %s\n", p_MANSCDP_device_array->DeviceID);
        printf("Name        : %s\n", p_MANSCDP_device_array->Name);
        printf("Manufacturer: %s\n", p_MANSCDP_device_array->Manufacturer);
        printf("Model       : %s\n", p_MANSCDP_device_array->Model);
        printf("Owner       : %s\n", p_MANSCDP_device_array->Owner);
        printf("CivilCode   : %s\n", p_MANSCDP_device_array->CivilCode);
        printf("Address     : %s\n", p_MANSCDP_device_array->Address);
        printf("Parental    : %s\n", p_MANSCDP_device_array->Parental);
        printf("SafetyWay   : %s\n", p_MANSCDP_device_array->SafetyWay);
        printf("RegisterWay : %s\n", p_MANSCDP_device_array->RegisterWay);
        printf("Secrecy     : %s\n", p_MANSCDP_device_array->Secrecy);
        printf("Status      : %s\n", p_MANSCDP_device_array->Status);
    }
}

typedef struct _receiving_thread_parameter
{
    uint16_t port;
    BOOL loop;
}receiving_thread_parameter;

DWORD WINAPI receiving_thread(LPVOID lpParam)
{
    receiving_thread_parameter* parameter = (receiving_thread_parameter*)lpParam;

    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);

    SOCKET receiving_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in sockaddr_local;
    memset(&sockaddr_local, 0x0, sizeof(sockaddr_in));
    sockaddr_local.sin_addr.S_un.S_addr = inet_addr("192.168.10.29");
    sockaddr_local.sin_family = AF_INET;
    sockaddr_local.sin_port = htons(parameter->port);

    bind(receiving_sock, (sockaddr*)&sockaddr_local, sizeof(sockaddr_in));

    DWORD timeout = 1000;

    setsockopt(receiving_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(DWORD));

    BYTE* buffer = (BYTE*)malloc(USHRT_MAX);

    int bytes_received;

    DWORD count = 0;
    char file_path[256];

    while(parameter->loop)
    {
        bytes_received = recv(receiving_sock, (char*)buffer, USHRT_MAX, 0);
        if(0 < bytes_received)
        {
            for(size_t i = 0; i < 8; i++)
            {
                printf("%02X", buffer[i]);
            }
            printf("\n");
            if(count < 10)
            {
                snprintf(file_path, 256, "D:\\gbt28181_stream_%u", count);
                FILE* pFile = fopen(file_path, "wb");
                fwrite(buffer, bytes_received, 1, pFile);
                fclose(pFile);
                count++;
            }
        }
    }

    closesocket(receiving_sock);

    free(buffer);

    WSACleanup();
    return 0;
}

int main(int argc, char* argv[])
{
    int result = GBT28181_client_initial();

    result = GBT28181_set_client_name("34020000001320000029");

    result = GBT28181_set_client_ID("34020000001320000029");

    result = GBT28181_set_client_password("123456");

    result = GBT28181_set_client_IP("192.168.10.29");

    result = GBT28181_set_client_port(5060);

    result = GBT28181_set_SIP_server_ID("34020000002000000001");

    result = GBT28181_set_SIP_server_domain("3402000000");

    result = GBT28181_set_SIP_server_IP("192.168.10.67");

    result = GBT28181_set_SIP_server_port(5060);

    result = GBT28181_set_expiration_interval(3600);

    result = GBT28181_set_heartbeat_interval(60);

    result = GBT28181_set_max_time_out_number(3);

    result = GBT28181_set_address_family(GBT28181_AF_INET);

    result = GBT28181_set_IP_protocol(GBT28181_IPPROTO_UDP);

    result = GBT28181_set_query_device_info_callback(query_deviceInfo_callback);

    result = GBT28181_set_query_device_status_callback(query_device_status_callback);

    result = GBT28181_set_query_catalog_callback(query_device_status_callback);

    result = GBT28181_set_max_number_of_live_video(64);

    result = GBT28181_client_go_online();

    system("pause");

#if _DEBUG_FOR_HANDLE
#else
    printf("\n\ncall query device info\n\n");

    result = GBT28181_query_device_info("34020000001320000141");

    system("pause");

    printf("\n\ncall query device status\n\n");

    result = GBT28181_query_device_status("34020000001320000141");

    system("pause");

    printf("\n\ncall query catalog\n\n");

    result = GBT28181_query_catalog("34020000001320000141");

    system("pause");
#endif

    uint32_t handle;
    receiving_thread_parameter thread_parameter;
    thread_parameter.port = 6000;
    thread_parameter.loop = true;

#if _DEBUG_FOR_HANDLE
    for(size_t i = 0; i < 100; i++)
    {
#endif
        result = GBT28181_get_idle_real_time_stream_handle(&handle);
        printf("result: %d,handle: %u\n", result, handle);
#if _DEBUG_FOR_HANDLE
    }
#endif

    system("pause");

    result = GBT28181_set_RTP_port(handle, thread_parameter.port);

    result = GBT28181_get_real_time_stream(handle, "34020000001320000141", "192.168.10.141", 5060);

    HANDLE hThread = CreateThread(NULL, 0, receiving_thread, &thread_parameter, 0, NULL);

    system("pause");

    result = GBT28181_close_real_time_stream(handle);

    result = GBT28181_free_client();

    thread_parameter.loop = false;

    WaitForSingleObject(hThread, INFINITE);

    return 0;
}

