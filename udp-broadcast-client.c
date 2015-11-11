/* udp-broadcast-client.c
 * udp datagram client
 * Get datagram stock market quotes from UDP broadcast:
 * see below the step by step explanation
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern int mkaddr(void *addr, int *addrlen, char *str_addr, char *protocol);

/*
 * This function reports the error and exits back to the shell:
 */
static void displayError(const char *on_what) {
  fputs(strerror(errno), stderr);
  fputs(": ", stderr);
  fputs(on_what, stderr);
  fputc('\n', stderr);
  exit(1);
}

int main(int argc, char **argv) {
  int z;
  int x;
  struct sockaddr_in adr_bc;   /* AF_INET */
  struct sockaddr_in adr_srvr; /* AF_INET */
  int len_bc;                  /* length */
  int s;                       /* Socket */
  char dgram[512];             /* Recv buffer */
  static int so_reuseaddr = TRUE;
  static char *bc_addr = "127.255.255.2:9097";

  /*
   * Use a server address from the command line, if one has been provided.
   * Otherwise, this program will default to using the arbitrary address
   * 127.0.0.:
   */
  if (argc > 1) /* Broadcast address: */
    bc_addr = argv[1];

  /*
   * Create a UDP socket to use:
   */
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s == -1) displayError("socket()");

  /*
   * Form the broadcast address:
   */
  len_bc = sizeof(adr_bc);

  z = mkaddr(&adr_bc, &len_bc, bc_addr, "udp");

  if (z < 0) displayError("Bad broadcast address");

  /*
   * Allow multiple listeners on the broadcast address:
   */
  z = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr,
                 sizeof(so_reuseaddr));

  if (z == -1) displayError("setsockopt(SO_REUSEADDR)");

  /*
   * Bind our socket to the broadcast address:
   */
  z = bind(s, (struct sockaddr *)&adr_bc, len_bc);

  if (z == -1) displayError("bind(2)");

  for (;;) {
    /*
     * Wait for a broadcast message:
     */
    x = sizeof(adr_srvr);
    z = recvfrom(s,                            /* Socket */
                 dgram,                        /* Receiving buffer */
                 sizeof(dgram),                /* Max rcv buf size */
                 0,                            /* Flags: no options */
                 (struct sockaddr *)&adr_srvr, /* Addr */
                 &x);                          /* Addr len, in & out */

    if (z == -1) displayError("recvfrom(2)"); /* else err */

    char from[INET_ADDRSTRLEN], to[INET_ADDRSTRLEN];
    printf(">> %s:%d -> %s:%d\n",
           inet_ntop(AF_INET, &adr_srvr.sin_addr, from, x),
           ntohs(adr_srvr.sin_port),
           inet_ntop(AF_INET, &adr_bc.sin_addr, to, len_bc),
           ntohs(adr_bc.sin_port));

    fwrite(dgram, z, 1, stdout);
    putchar('\n');

    fflush(stdout);
  }

  return 0;
}
