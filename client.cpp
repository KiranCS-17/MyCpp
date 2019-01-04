#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <errno.h>
#define PORT 10000
#define MY_PORT 10001
#define MINE_PORT 10002

#define MSG_SIZE 1000
#define NUMBER_OF_MESSAGES 1000

int sock;
struct sockaddr_in* paddrs[5];
struct sockaddr_in* laddrs[5];

const char client[] = "10.43.201.133";
const char secondary[] = "10.43.202.133";
char interest[] = "10.43.199.176";

void handle_signal(int signum);

//./client 10.43.199.176 10.43.200.11
int main(int argc, char** argv)
{
    int i;
    int counter = 0;
    int asconf = 1;
    int ret;
    int addr_count;
    char address[16];
    char buffer[MSG_SIZE];
    sctp_assoc_t id;
    struct sockaddr_in addr;
    struct sctp_status status;
    struct sctp_initmsg initmsg;
    struct sctp_event_subscribe events;
    struct sigaction sig_handler;
    struct sctp_paddrparams heartbeat;
    struct sctp_rtoinfo rtoinfo;

    memset(&buffer, 'j', MSG_SIZE);
    memset(&initmsg, 0, sizeof(struct sctp_initmsg));
    memset(&addr, 0, sizeof(struct sockaddr_in));
    memset(&events, 1, sizeof(struct sctp_event_subscribe));
    memset(&status, 0, sizeof(struct sctp_status));
    memset(&heartbeat, 0, sizeof(struct sctp_paddrparams));
    memset(&rtoinfo, 0, sizeof(struct sctp_rtoinfo));

    if (argc != 3 || (inet_addr(argv[1]) == -1)) {
        puts("Usage: client [IP ADDRESS in form xxx.xxx.xxx.xxx] [IP ADDRESS in form xxx.xxx.xxx.xxx] ");
        return 0;
    }

    strncpy(address, argv[1], 15);
    address[15] = 0;

    addr.sin_family = AF_INET;
    inet_aton(address, &(addr.sin_addr));
    addr.sin_port = htons(PORT);

    initmsg.sinit_num_ostreams = 2;
    initmsg.sinit_max_instreams = 2;
    initmsg.sinit_max_attempts = 1;

    heartbeat.spp_flags = SPP_HB_ENABLE;
    heartbeat.spp_hbinterval = 500;
    heartbeat.spp_pathmaxrxt = 1;

    rtoinfo.srto_max = 2000;

    sig_handler.sa_handler = handle_signal;
    sig_handler.sa_flags = 0;

    /*Handle SIGINT in handle_signal Function*/
    if (sigaction(SIGINT, &sig_handler, NULL) == -1)
        perror("sigaction");

    /*Create the Socket*/
    if ((ret = (sock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP))) < 0)
        perror("socket");
    struct sockaddr_in addr_cli;
    memset(&addr_cli, 0, sizeof(struct sockaddr_in));
    addr_cli.sin_family = AF_INET;
    addr_cli.sin_port = htons(MY_PORT);
    inet_aton(client, &(addr_cli.sin_addr));

    if (bind(sock, (struct sockaddr*)&addr_cli, sizeof(struct sockaddr)) < 0)
        perror("socket");

    struct sockaddr_in bind_addrs;
    memset(&bind_addrs, 0, sizeof(struct sockaddr_in));
    bind_addrs.sin_family = AF_INET;
    bind_addrs.sin_port = htons(MY_PORT);
    inet_aton(secondary, &(bind_addrs.sin_addr));

    if (sctp_bindx(sock, (struct sockaddr*)&bind_addrs, 1, SCTP_BINDX_ADD_ADDR) == -1) {
        perror("sctp bindx");
        exit(2);
    }
    /*Configure Heartbeats*/
    if ((ret = setsockopt(sock, SOL_SCTP, SCTP_PEER_ADDR_PARAMS, &heartbeat, sizeof(heartbeat))) != 0)
        perror("setsockopt 11");

    /*Set rto_max*/
    if ((ret = setsockopt(sock, SOL_SCTP, SCTP_RTOINFO, &rtoinfo, sizeof(rtoinfo))) != 0)
        perror("setsockopt 22");

    /*Set SCTP Init Message*/
    if ((ret = setsockopt(sock, SOL_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg))) != 0)
        perror("setsockopt 33");

    /*Enable SCTP Events*/
    if ((ret = setsockopt(sock, SOL_SCTP, SCTP_EVENTS, (void*)&events, sizeof(events))) != 0)
        perror("setsockopt 44");

    /*Get And Print Heartbeat Interval*/
    i = (sizeof heartbeat);
    getsockopt(sock, SOL_SCTP, SCTP_PEER_ADDR_PARAMS, &heartbeat, (socklen_t*)&i);

    printf("Heartbeat interval %d\n", heartbeat.spp_hbinterval);

    /*Connect to Host*/

    /*
    if(((ret = connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr)))) < 0)
    {
        perror("connect");
        close(sock);
        exit(0);
    }
    */

    struct sockaddr_in addr_s;
    char address2[16];
    strncpy(address2, argv[2], 15);
    address2[15] = 0;

    #define NUM 2

    addr_s.sin_family = AF_INET;
    inet_aton(address2, &(addr_s.sin_addr));
    addr_s.sin_port = htons(PORT);

    struct sockaddr* address_array = NULL;
    address_array = (struct sockaddr*)new sockaddr_in[NUM];
    memset((void*)address_array, 0, sizeof(sockaddr_in) * NUM);

    for (unsigned i = 0; i < NUM; i++) {
        if (0 == i) {
            memcpy(((char*)address_array + (sizeof(sockaddr_in) * i)), (sockaddr*)&addr_s, sizeof(sockaddr_in));
        }
        else {
            memcpy(((char*)address_array + (sizeof(sockaddr_in) * i)), (sockaddr*)&addr, sizeof(sockaddr_in));
        }
    }
    if (sctp_connectx(sock, (struct sockaddr*)address_array, NUM, &id) != 0) {
        perror("sctp_connectx");
        return 1;
    }

    printf("Connected! Assoc ID %d\n", (int)id);

    /*Get Peer Addresses*/
    addr_count = sctp_getpaddrs(sock, 0, (struct sockaddr**)paddrs);
    printf("\nPeer addresses: %d\n", addr_count);

    /*Print Out Addresses*/
    for (i = 0; i < addr_count; i++)
        printf("Address %d: %s:%d\n", i + 1, inet_ntoa((*paddrs)[i].sin_addr), (*paddrs)[i].sin_port);

    sctp_freepaddrs((struct sockaddr*)*paddrs);

    /*Print Locally Binded Addresses*/
    addr_count = sctp_getladdrs(sock, 0, (struct sockaddr**)laddrs);
    printf("Addresses binded: %d\n", addr_count);
    for (i = 0; i < addr_count; i++)
        printf("Address %d: %s:%d\n", i + 1, inet_ntoa((*laddrs)[i].sin_addr), (*laddrs)[i].sin_port);
    sctp_freeladdrs((struct sockaddr*)*laddrs);

    /*Monitor*/
    struct sctp_paddrinfo pinfo;
    struct sockaddr_in sin_interest;
    memset(&sin_interest, 0, sizeof(sin_interest));
    sin_interest.sin_family = AF_INET;
    sin_interest.sin_port = htons(PORT);
    inet_aton(interest, &(sin_interest.sin_addr));
    memcpy(&pinfo.spinfo_address, &sin_interest, sizeof(sin_interest));
    socklen_t len = sizeof(pinfo);
    struct sctp_sndrcvinfo sinfo;

    /*Start to Send Data*/
    for (i = 0; i < NUMBER_OF_MESSAGES; i++) {
        counter++;
        //printf("Sending data chunk #%d...", counter);
        bzero(&sinfo, sizeof(sinfo));
        //sinfo.sinfo_flags |= SCTP_SENDALL;
        sctp_send(sock, buffer, MSG_SIZE, &sinfo, 0);
        /* if ((ret = send(sock, buffer, MSG_SIZE, 0)) == -1)
           perror("write");

        printf("Sent %d bytes to peer\n",ret);
		*/
        int error = getsockopt(sock, SOL_SCTP, SCTP_GET_PEER_ADDR_INFO, &pinfo, &len);

        if ((-1 == error)) {
            printf("SCTP_GET_PEER_ADDR_INFO failed %d", error);
        }
        printf("%d \n Status :==", pinfo.spinfo_state);

        sleep(10);
    }

    if (close(sock) != 0)
        perror("close");
}

void handle_signal(int signum)
{
    switch (signum) {
    case SIGINT:
        if (close(sock) != 0)
            perror("close");
        //exit(0);
        break;
    default: //exit(0);
        break;
    }
}
