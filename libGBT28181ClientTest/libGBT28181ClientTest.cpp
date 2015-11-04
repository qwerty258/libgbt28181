#include <libGBT28181Client.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int result;
    GBT28181_client_initial();

    result = GBT28181_set_client_name("34020000001320000001");

    result = GBT28181_set_client_ID("34020000001320000001");

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

    result = GBT28181_client_go_online();

    scanf("%d", &result);

    result = GBT28181_free_client();
    return 0;
}

