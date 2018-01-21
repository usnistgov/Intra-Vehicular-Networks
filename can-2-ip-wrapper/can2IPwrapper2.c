#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "terminal.h"
#include "lib.h"
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>
#define MAXSOCK 32    /* max. number of CAN interfaces given on the cmdline */
#define MAXIFNAMES 30 /* size of receive name index to omit ioctls */
#define MAXCOL 6      /* number of different colors for colorized output */
#define ANYDEV "any"  /* name of interface to receive from any CAN interface */
#define ANL "\r\n"    /* newline in ASC mode */
#define SILENT_INI 42 /* detect user setting on commandline */
#define SILENT_OFF 0  /* no silent mode */
#define SILENT_ANI 1  /* silent mode with animation */
#define SILENT_ON  2  /* silent mode (completely silent) */
#define BOLD    ATTBOLD
#define RED     ATTBOLD FGRED
#define GREEN   ATTBOLD FGGREEN
#define YELLOW  ATTBOLD FGYELLOW
#define BLUE    ATTBOLD FGBLUE
#define MAGENTA ATTBOLD FGMAGENTA
#define CYAN    ATTBOLD FGCYAN
#define MAXANI 4
int vflag = 0;
const char col_on[MAXCOL][19] = { BLUE, RED, GREEN, BOLD, MAGENTA, CYAN };
const char col_off[] = ATTRESET;
static char *cmdlinename[MAXSOCK];
char host[100];
char json[1024];
static char devname[MAXIFNAMES][IFNAMSIZ + 1];
static int dindex[MAXIFNAMES];
static int max_devname_len;
const int canfd_on = 1;
const char anichar[MAXANI] = { '|', '/', '-', '\\' };
const char extra_m_info[4][4] = { "- -", "B -", "- E", "B E" };
extern int optind, opterr, optopt;
static volatile int running = 1;
fd_set rdfs;
int s[MAXSOCK];

unsigned char timestamp = 0;
unsigned char dropmonitor = 0;
int maxdlen;
unsigned char log = 0;
unsigned char logfrmt = 0;
int rcvbuf_size = 0;
int opt, ret;
int currmax, numfilter;
int join_filter;
char *ptr, *nptr;
struct sockaddr_can addr;
char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32 ))];
struct iovec iov;
struct msghdr msg;
struct can_filter *rfilter;
can_err_mask_t err_mask;
struct canfd_frame frame;
int nbytes, i;
struct ifreq ifr;
struct timeval tv;
struct timeval timeout_config = { 0, 0 }, *timeout_current = NULL;





























int sockfd, portno/*, n*/;

void IP_handler(char* omega, char* host) {


	struct sockaddr_in serv_addr;
	struct hostent *server;
	portno = 3333;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server = gethostbyname(host);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr,
			server->h_length);
	serv_addr.sin_port = htons(portno);
	connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	send(sockfd, omega, strlen(omega), 0);
	bzero(omega, 256);
	close(sockfd);

}



void sigterm(int signo) {
	running = 0;
}
void signal_callback_handler(int signum) {
	printf("Caught signal SIGPIPE %d\n", signum);
}
void retrieve_params(int argc, char **argv)


{


	while ((opt = getopt(argc, argv, "v:Lh:?")) != -1) {
		switch (opt) {
		case 'v':
			vflag = 1;
			break;
		case 'h':
			printf("%s : ", host);
			memset(host, '\0', sizeof(host));
			strcpy(host, optarg);
			break;
		case 'L':
			logfrmt = 1;
			break;
		default:
			exit(1);
			break;
		}
	}


}
int Can_interface_to_CAN_socket_binder(int ifidx, int socket) {
	int i;
	struct ifreq ifr;
	for (i = 0; i < MAXIFNAMES; i++) {
		if (dindex[i] == ifidx)
			return i;
	}
	for (i = 0; i < MAXIFNAMES; i++) {
		if (dindex[i]) {
			ifr.ifr_ifindex = dindex[i];
			if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
				dindex[i] = 0;
		}
	}
	for (i = 0; i < MAXIFNAMES; i++)
		if (!dindex[i]) /* free entry */
			break;
	if (i == MAXIFNAMES) {
		fprintf(stderr, "Interface index cache only supports %d interfaces.\n",
		MAXIFNAMES);
		exit(1);
	}

	dindex[i] = ifidx;
	ifr.ifr_ifindex = ifidx;
	if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
		perror("SIOCGIFNAME");
	if (max_devname_len < strlen(ifr.ifr_name))
		max_devname_len = strlen(ifr.ifr_name);
	strcpy(devname[i], ifr.ifr_name);
#ifdef DEBUG
	printf("new index %d (%s)\n", i, devname[i]);
#endif
	return i;
}






int main(int argc, char **argv) {

//////////////////////////// exception handlers

	signal(SIGTERM, sigterm);
	signal(SIGHUP, sigterm);
	signal(SIGINT, sigterm);
	signal(SIGPIPE, signal_callback_handler);

//////////////////////////// retrieve params
	retrieve_params(argc,argv);

//	printf ("interface khalid %s\n", host);

// available info : can interface,ip endpoint


////////////////////////////















	currmax = argc - optind; /* find real number of CAN devices */

	if (currmax > MAXSOCK) {
		fprintf(stderr, "More than %d CAN devices given on commandline!\n",
				MAXSOCK);
		return 1;
	}

/////////////////// reading parameters

	for (i = 0; i < currmax; i++) {

		ptr = argv[optind + i];
		nptr = strchr(ptr, ',');
#ifdef DEBUG
		printf("open %d '%s'.\n", i, ptr);
#endif
		s[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
		if (s[i] < 0) {
			perror("socket");
			return 1;
		}
		cmdlinename[i] = ptr; /* save pointer to cmdline name of this socket */
		if (nptr)
			nbytes = nptr - ptr; /* interface name is up the first ',' */
		else
			nbytes = strlen(ptr); /* no ',' found => no filter definitions */
		if (nbytes >= IFNAMSIZ) {
			fprintf(stderr, "name of CAN device '%s' is too long!\n", ptr);
			return 1;
		}
		if (nbytes > max_devname_len)
			max_devname_len = nbytes; /* for nice printing */
		addr.can_family = AF_CAN;
		memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
		strncpy(ifr.ifr_name, ptr, nbytes);
#ifdef DEBUG
		printf("using interface name '%s'.\n", ifr.ifr_name);
#endif
		if (strcmp(ANYDEV, ifr.ifr_name)) {
			if (ioctl(s[i], SIOCGIFINDEX, &ifr) < 0) {
				perror("SIOCGIFINDEX");
				exit(1);
			}
			addr.can_ifindex = ifr.ifr_ifindex;
		} else
			addr.can_ifindex = 0; /* any can interface */
		if (nptr) {
			numfilter = 0;
			ptr = nptr;
			while (ptr) {
				numfilter++;
				ptr++; /* hop behind the ',' */
				ptr = strchr(ptr, ','); /* exit condition */
			}
			rfilter = malloc(sizeof(struct can_filter) * numfilter);
			if (!rfilter) {
				fprintf(stderr, "Failed to create filter space!\n");
				return 1;
			}
			numfilter = 0;
			err_mask = 0;
			join_filter = 0;
			while (nptr) {
				ptr = nptr + 1; /* hop behind the ',' */
				nptr = strchr(ptr, ','); /* update exit condition */
				if (sscanf(ptr, "%x:%x", &rfilter[numfilter].can_id,
						&rfilter[numfilter].can_mask) == 2) {
					rfilter[numfilter].can_mask &= ~CAN_ERR_FLAG;
					numfilter++;
				} else if (sscanf(ptr, "%x~%x", &rfilter[numfilter].can_id,
						&rfilter[numfilter].can_mask) == 2) {
					rfilter[numfilter].can_id |= CAN_INV_FILTER;
					rfilter[numfilter].can_mask &= ~CAN_ERR_FLAG;
					numfilter++;
				} else if (*ptr == 'j' || *ptr == 'J') {
					join_filter = 1;
				} else if (sscanf(ptr, "#%x", &err_mask) != 1) {
					fprintf(stderr, "Error in filter option parsing: '%s'\n",
							ptr);
					return 1;
				}
			}
			if (err_mask)
				setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask,
						sizeof(err_mask));
			if (join_filter
					&& setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_JOIN_FILTERS,
							&join_filter, sizeof(join_filter)) < 0) {
				perror(
						"setsockopt CAN_RAW_JOIN_FILTERS not supported by your Linux Kernel");
				return 1;
			}
			if (numfilter)
				setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FILTER, rfilter,
						numfilter * sizeof(struct can_filter));
			free(rfilter);
		} /* if (nptr) */
		/* try to switch the socket into CAN FD mode */
		setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on,
				sizeof(canfd_on));
		if (rcvbuf_size) {
			int curr_rcvbuf_size;
			socklen_t curr_rcvbuf_size_len = sizeof(curr_rcvbuf_size);
			/* try SO_RCVBUFFORCE first, if we run with CAP_NET_ADMIN */
			if (setsockopt(s[i], SOL_SOCKET, SO_RCVBUFFORCE, &rcvbuf_size,
					sizeof(rcvbuf_size)) < 0) {
#ifdef DEBUG
				printf("SO_RCVBUFFORCE failed so try SO_RCVBUF ...\n");
#endif
				if (setsockopt(s[i], SOL_SOCKET, SO_RCVBUF, &rcvbuf_size,
						sizeof(rcvbuf_size)) < 0) {
					perror("setsockopt SO_RCVBUF");
					return 1;
				}
				if (getsockopt(s[i], SOL_SOCKET, SO_RCVBUF, &curr_rcvbuf_size,
						&curr_rcvbuf_size_len) < 0) {
					perror("getsockopt SO_RCVBUF");
					return 1;
				}
				if (!i && curr_rcvbuf_size < rcvbuf_size * 2)
					fprintf(stderr, "The socket receive buffer size was "
							"adjusted due to /proc/sys/net/core/rmem_max.\n");
			}
		}
		if (timestamp || log || logfrmt) {
			const int timestamp_on = 1;
			if (setsockopt(s[i], SOL_SOCKET, SO_TIMESTAMP, &timestamp_on,
					sizeof(timestamp_on)) < 0) {
				perror("setsockopt SO_TIMESTAMP");
				return 1;
			}
		}
		if (dropmonitor) {
			const int dropmonitor_on = 1;
			if (setsockopt(s[i], SOL_SOCKET, SO_RXQ_OVFL, &dropmonitor_on,
					sizeof(dropmonitor_on)) < 0) {
				perror(
						"setsockopt SO_RXQ_OVFL not supported by your Linux Kernel");
				return 1;
			}
		}
		if (bind(s[i], (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			perror("bind");
			return 1;
		}
	}
	iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;





	FD_ZERO(&rdfs);
	for (i = 0; i < currmax; i++)
		FD_SET(s[i], &rdfs);
//		if (timeout_current)
//			*timeout_current = timeout_config;
//		if ((ret = select(s[currmax - 1] + 1, &rdfs, NULL, NULL,
//				timeout_current)) <= 0) {
//			running = 0;
//			continue;
//		}
	int idx;
				iov.iov_len = sizeof(frame);
				msg.msg_namelen = sizeof(addr);
				msg.msg_controllen = sizeof(ctrlmsg);
				msg.msg_flags = 0;







	while (running) {

		for (i = 0; i < currmax; i++) { /* check all CAN RAW sockets */
			if (FD_ISSET(s[i], &rdfs)) {


/////////////////////////////////// receiving and buffering the CAN frame
				nbytes = recvmsg(s[i], &msg, 0);
				idx = Can_interface_to_CAN_socket_binder(addr.can_ifindex, s[i]);
								if ((size_t)nbytes == CAN_MTU)
									maxdlen = CAN_MAX_DLEN;
								else if ((size_t)nbytes == CANFD_MTU)
									maxdlen = CANFD_MAX_DLEN;
								else {
									fprintf(stderr, "read: incomplete CAN frame\n");
									return 1;
								}
				char buf[CL_CFSZ]; /* max length */
				sprint_canframe(buf, &frame, 0, maxdlen);
				snprintf(json, sizeof(json),
						"{ \"dev\": \"%s\", \"len\": %d, \"message\": \"%s\" }",
						devname[idx], max_devname_len, buf);
				printf("%s\n", json);
				usleep(400 * 1000);
/////////////////////////////////// passing the CAN message to the IP socket.
				IP_handler(json, host);
				//goto out_fflush;

				if (logfrmt) {
					char buf[CL_CFSZ];
					sprint_canframe(buf, &frame, 0, maxdlen);
//					printf("(%010ld.%06ld) %*s %s\n", tv.tv_sec, tv.tv_usec,
//							max_devname_len, devname[idx], buf);
					//goto out_fflush;
				             }
			}
			//out_fflush: fflush(stdout);
		}
	}
	for (i = 0; i < currmax; i++)
		close(s[i]);

	return 0;

}
