#include "ESP8266Interface.h"
#include "OPL1000Interface.h"
#include "mbed.h"

#define TCP_MODE 1  // 1:TCP ,0: UDP

#if (TCP_MODE)
TCPSocket socket;
#else
UDPSocket socket;
#endif

char rbuffer[64];
char sbuffer[] = "1234567890";

ATCmdParser *_parser;
OPL1000Interface wifi(PE_14, PE_15, 0);
// ESP8266Interface wifi(PH_8, PH_9, 1);
SocketAddress a("192.168.0.179", 8181);

int main() {
  int scount = -1, rcount = -1, ret = -1;

  printf("WiFi example\n");
  ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD,
                     NSAPI_SECURITY_WPA_WPA2);
  if (ret == 0) {
    printf("\nMAC: %s\r\n", wifi.get_mac_address());

    ret = socket.open(&wifi);
    if (ret != 0) {
      printf("socket.open fail...\n");
      return -1;
    }
#if (TCP_MODE)
    ret = socket.connect(a);
#else
    ret = socket.bind(a);
#endif
    if (ret == 0) {
#if (TCP_MODE)
      scount = socket.send(sbuffer, sizeof sbuffer);
#else
      scount = socket.sendto(a, sbuffer, sizeof sbuffer);
#endif
      printf("sent %d [%.*s]\n", scount, strstr(sbuffer, "\r\n") - sbuffer,
             sbuffer);
      // #if (TCP_MODE)
      //       rcount = socket.recv(rbuffer, sizeof rbuffer);
      // #else
      //       rcount = socket.recvfrom(&a, rbuffer, sizeof rbuffer);
      // #endif
      //       printf("recv %d [%.*s]\n", rcount, strstr(rbuffer, "\r\n") -
      //       rbuffer,
      //              rbuffer);
      socket.close();
    } else {
      printf("socket.connect fail, ret = %d\n", ret);
      return -2;
    }
    ret = wifi.disconnect();
    if (ret != 0) {
      printf("wifi.disconnect fail, ret = %d\n", ret);
      return -3;
    }
  } else {
    printf("Connect to AP failed...\r\n");
    return -4;
  }

  printf("Done\n");

  return 0;
}
