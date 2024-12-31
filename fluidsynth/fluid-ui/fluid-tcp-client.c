#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include <sys/socket.h>
#include <unistd.h> 

static struct sockaddr_in servaddr;

void runCommand(char *command, char *response, int responseMax)
{
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 10000;
  bzero(response, responseMax);
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    printf("\nsocket creation failed...\n");
  } else if (connect(fd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) != 0) {
    printf("\nTCP connection with the fluidsynth failed...\n");
    close(fd);
  } else {
    printf("\ncommand: %s",command);
    int q = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
    write(fd, command, strlen(command));
    write(fd, "\n", 1);
    char *p = response;
    while(1) {
      int c = read(fd, p, responseMax-(p-response));
      if (c > 0) p = p + c;
      if (c < 0) {
        close(fd);
        printf("\nresponse: %s",response);
        return;
      }
    }
  }
}

void setTargetAddress(char *hostname, int port)
{
    struct hostent *server = gethostbyname(hostname);
    bcopy((char *)server->h_addr, (char *)&servaddr.sin_addr.s_addr, server->h_length);
    servaddr.sin_port = htons(port);
    servaddr.sin_family = AF_INET;
}

