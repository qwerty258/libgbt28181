#include <libGBT28181Client.h>
#include <stdio.h>
#include <stdlib.h>

void query_deviceInfo_callback(char* device_ID, char* device_type, char* manufacturer, char* model, char* firmware, unsigned long long max_camera, unsigned long long max_alarm)
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

void query_device_status_callback(char* deviceID, unsigned long long sum_num, MANSCDP_device* p_MANSCDP_device_array)
{
    printf("device_ID: %s\n", deviceID);
    printf("count    : %u\n", sum_num);
    for(unsigned long long i = 0; i < sum_num; i++)
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

    result = GBT28181_client_go_online();

    system("pause");

    printf("\n\ncall query device info\n\n");

    result = GBT28181_query_device_info("34020000001320000141");

    system("pause");

    printf("\n\ncall query device status\n\n");

    result = GBT28181_query_device_status("34020000001320000141");

    system("pause");

    printf("\n\ncall query catalog\n\n");

    result = GBT28181_query_catalog("34020000001320000141");

    system("pause");

    result = GBT28181_free_client();
    return 0;
}

