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

    result = GBT28181_client_go_online();

    system("pause");

    result = GBT28181_query_device_info("34020000001320000141");

    system("pause");

    result = GBT28181_free_client();
    return 0;
}

