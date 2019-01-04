#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#define BUFFER_SIZE (1 << 16)
#define PORT 10000

const char primary[] = "10.43.199.176";
const char secondary[] = "10.43.200.11";


int sock, ret, flags;
int i, reuse = 1;
int addr_count = 0;
char buffer[BUFFER_SIZE];
socklen_t from_len;

struct sockaddr_in addr;
struct sockaddr_in* laddr[10];
struct sockaddr_in* paddrs[10];
struct sctp_sndrcvinfo sinfo;
struct sctp_event_subscribe event;
struct sctp_prim prim_addr;
struct sctp_paddrparams heartbeat;
struct sigaction sig_handler;
struct sctp_rtoinfo rtoinfo;

void handle_signal(int signum);
static void
print_notification(void* buf);
int main(void)
{
    if ((sock = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0)
        perror("socket");

    memset(&addr, 0, sizeof(struct sockaddr_in));
    memset(&event, 1, sizeof(struct sctp_event_subscribe));
    memset(&heartbeat, 0, sizeof(struct sctp_paddrparams));
    memset(&rtoinfo, 0, sizeof(struct sctp_rtoinfo));

    addr.sin_family = AF_INET;
    inet_aton(primary, &(addr.sin_addr));
    //addr.sin_addr.s_addr = htonl(inet_addr(primary));
    addr.sin_port = htons(PORT);

    from_len = (socklen_t)sizeof(struct sockaddr_in);

    sig_handler.sa_handler = handle_signal;
    sig_handler.sa_flags = 0;

    heartbeat.spp_flags = SPP_HB_ENABLE;
    heartbeat.spp_hbinterval = 50;
    heartbeat.spp_pathmaxrxt = 1;

    rtoinfo.srto_max = 2000;

    /*Set Heartbeats*/
    if (setsockopt(sock, SOL_SCTP, SCTP_PEER_ADDR_PARAMS, &heartbeat, sizeof(heartbeat)) != 0)
        perror("setsockopt");

    /*Set rto_max*/
    if (setsockopt(sock, SOL_SCTP, SCTP_RTOINFO, &rtoinfo, sizeof(rtoinfo)) != 0)
        perror("setsockopt");

    /*Set Signal Handler*/
    if (sigaction(SIGINT, &sig_handler, NULL) == -1)
        perror("sigaction");

    /*Set Events */
    if (setsockopt(sock, IPPROTO_SCTP, SCTP_EVENTS, &event, sizeof(struct sctp_event_subscribe)) < 0)
        perror("setsockopt");

    /*Set the Reuse of Address*/
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
        perror("setsockopt");

    /*Bind the Addresses*/
    if (bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0)
        perror("bind");

    struct sockaddr_in bind_addrs;
    memset(&bind_addrs, 0, sizeof(struct sockaddr_in));
    bind_addrs.sin_family = AF_INET;
    bind_addrs.sin_port = htons(PORT);
    // bind_addrs.sin_addr.s_addr =  htonl(inet_addr(secondary));
    inet_aton(secondary, &(bind_addrs.sin_addr));
    sctp_bindx(sock, (struct sockaddr*)&bind_addrs, 1, SCTP_BINDX_ADD_ADDR);

    if (listen(sock, 2) < 0)
        perror("listen");

    /*Get Heartbeat Value*/
    i = (sizeof heartbeat);
    getsockopt(sock, SOL_SCTP, SCTP_PEER_ADDR_PARAMS, &heartbeat, (socklen_t*)&i);
    printf("Heartbeat interval %d\n", heartbeat.spp_hbinterval);

    /*Print Locally Binded Addresses*/
    addr_count = sctp_getladdrs(sock, 0, (struct sockaddr**)laddr);
    printf("Addresses binded: %d\n", addr_count);
    for (i = 0; i < addr_count; i++)
        printf("Address %d: %s:%d\n", i + 1, inet_ntoa((*laddr)[i].sin_addr), (*laddr)[i].sin_port);
    sctp_freeladdrs((struct sockaddr*)*laddr);
    while (1) {
        flags = 0;

        ret = sctp_recvmsg(sock, buffer, BUFFER_SIZE, NULL, 0, NULL, &flags);

        if (flags & MSG_NOTIFICATION) {
            printf("Notification received from %s:%u\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            print_notification(buffer);
        }
        //printf("%d bytes received from %s:%u\n", ret, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }

    if (close(sock) < 0)
        perror("close");
}

void handle_signal(int signum)
{
    switch (signum) {
    case SIGINT:
        if (close(sock) != 0)
            perror("close");
        exit(0);
        break;
    default:
        exit(0);
        break;
    }
}

static void
print_notification(void* buf)
{
    struct sctp_assoc_change* sac;
    struct sctp_paddr_change* spc;
    struct sctp_adaptation_event* sad;
    union sctp_notification* snp;
    char addrbuf[INET6_ADDRSTRLEN];
    const char* ap;
    struct sockaddr_in* sin;
    struct sockaddr_in6* sin6;

    snp = buf;

    switch (snp->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE:
        sac = &snp->sn_assoc_change;
        printf("^^^ Association change: ");
        switch (sac->sac_state) {
        case SCTP_COMM_UP:
            printf("Communication up (streams (in/out)=(%u/%u)).\n",
                sac->sac_inbound_streams, sac->sac_outbound_streams);
            break;
        case SCTP_COMM_LOST:
            printf("Communication lost (error=%d).\n", sac->sac_error);
            break;
        case SCTP_RESTART:
            printf("Communication restarted (streams (in/out)=(%u/%u).\n",
                sac->sac_inbound_streams, sac->sac_outbound_streams);
            break;
        case SCTP_SHUTDOWN_COMP:
            printf("Communication completed.\n");
            break;
        case SCTP_CANT_STR_ASSOC:
            printf("Communication couldn't be started.\n");
            break;
        default:
            printf("Unknown state: %d.\n", sac->sac_state);
            break;
        }
        break;
    case SCTP_PEER_ADDR_CHANGE:
        spc = &snp->sn_paddr_change;
        if (spc->spc_aaddr.ss_family == AF_INET) {
            sin = (struct sockaddr_in*)&spc->spc_aaddr;
            ap = inet_ntop(AF_INET, &sin->sin_addr,
                addrbuf, INET6_ADDRSTRLEN);
        }
        else {

            sin6 = (struct sockaddr_in6*)&spc->spc_aaddr;
            ap = inet_ntop(AF_INET6, &sin6->sin6_addr,
                addrbuf, INET6_ADDRSTRLEN);
        }
        printf("^^^ Peer Address change: %s ", ap);
        switch (spc->spc_state) {
        case SCTP_ADDR_AVAILABLE:
            printf("is available.\n");
            break;
        case SCTP_ADDR_UNREACHABLE:
            printf("is not available (error=%d).\n", spc->spc_error);
            break;
        case SCTP_ADDR_REMOVED:
            printf("was removed.\n");
            break;
        case SCTP_ADDR_ADDED:
            printf("was added.\n");
            break;
        case SCTP_ADDR_MADE_PRIM:
            printf("is primary.\n");
            break;
        default:
            printf("unknown state (%d).\n", spc->spc_state);
            break;
        }
        break;
    case SCTP_SHUTDOWN_EVENT:
        printf("^^^ Shutdown received.\n");
        break;
    case SCTP_ADAPTATION_INDICATION:
        sad = &snp->sn_adaptation_event;
        printf("^^^ Adaptation indication 0x%08x received.\n",
            sad->sai_adaptation_ind);
        break;
    default:
        printf("^^^ Unknown event of type: %u.\n",
            snp->sn_header.sn_type);
        break;
    };
}
