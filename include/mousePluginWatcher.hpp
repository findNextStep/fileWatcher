#pragma  once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <thread>


namespace wmj {
namespace fileWatcher {

class mousePluginWatcher {
    std::thread watch_thread;
    const int hotplug_sock;
public:
    bool plugin;
public:
    mousePluginWatcher() :
        hotplug_sock(init_hotplug_sock()),
        plugin(access("/dev/input/mouse0", F_OK) == 0),
        watch_thread([this]() {
        while(true) {
            char buf[2048 * 2] = {0};
            recv(hotplug_sock, &buf, sizeof(buf), 0);
            std::string info(buf);
            if(info.find("mouse") != info.npos) {
                if(info.find("add") != info.npos) {
                    this->plugin = true;
                } else {
                    this->plugin = false;
                }
                // std::cout << info << std::endl;
            }
        }
    }) {}
    static int init_hotplug_sock(void) {
        struct sockaddr_nl snl;
        const int buffersize = 16 * 1024 * 1024;
        memset(&snl, 0x00, sizeof(struct sockaddr_nl));
        snl.nl_family = AF_NETLINK;
        snl.nl_pid = getpid();
        snl.nl_groups = 1;
        const int hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
        if(hotplug_sock == -1) {
            printf("error getting socket: %s", strerror(errno));
            return -1;
        }
        /* set receive buffersize */
        setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
        const int retval = bind(hotplug_sock, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl));
        if(retval < 0) {
            printf("bind failed: %s", strerror(errno));
            close(hotplug_sock);
            return -1;
        }
        return hotplug_sock;

    }
};



}
}