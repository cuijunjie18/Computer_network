#include "common.hpp"
#include "utils.hpp"
#include "proxy_server.hpp"

int main(int argc,char *argv[])
{
    ProxyServer server(8001);
    server.connect_with_client();
    server.interact_with_client();
    return 0;
}