#include <stdio.h>  // console input/output, perror

#include <stdlib.h> // exit
#include <string.h> // string manipulation

#include <sys/socket.h> // socket APIs
#include <netinet/in.h> // sockaddr_in
#include <unistd.h>     // open, close

#include <signal.h> // signal handling

#include <pthread.h>

extern void initControl(void *context);
extern void getControlInstance(int instance, char *response, void *context);
extern void setControlInstance(int instance, char *value, void *context);
extern void getControlInstances(char *response, void *context);
extern void setTargetAddress(char *hostname, int port);

#define REQUEST_SIZE 1024  // request buffer size
#define RESPONSE_SIZE 50000  // response buffer size
#define BACKLOG 10 // number of pending connections queue will hold

static const char *OKPrebody   =  "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
static const char *BadRequest  =  "HTTP/1.1 400 Bad request\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
static const char *NoResource  =  "HTTP/1.1 404 No such resource\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
static const char *WrongMethod =  "HTTP/1.1 405 Method not allowed\r\nAccess-Control-Allow-Origin: *\r\n\r\n";

static void *serverRun(void *context)
{
  int serverPort = 3000;
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;                    
  serverAddress.sin_port = htons(serverPort);             
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

  int server = socket(AF_INET, SOCK_STREAM, 0);

  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

  if (bind(server, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
  {
    printf("Error: Fail to bind to port %d.\n",serverPort);
    return NULL;
  }

  if (listen(server, BACKLOG) < 0)
  {
    printf("Error: The server is not listening.\n");
    return NULL;
  }

  printf("\nServer is listening on port %d/\n\n",serverPort);
  fflush(stdout);

  while (1)
  {
    char *request = (char *)malloc(REQUEST_SIZE * sizeof(char));
    char *response = (char *)malloc(RESPONSE_SIZE * sizeof(char));
    char method[10], route[300];
    char *resource = NULL, *instance = NULL, *value = NULL;

    int client = accept(server, NULL, NULL);
    read(client, request, REQUEST_SIZE);

    sscanf(request, "%s %s", method, route);

    char *token = strtok(route, "/");
    if (token != NULL) {
         resource = token;
         token = strtok(NULL, "/");
         if (token != NULL) {
            instance = token;
            token = strtok(NULL, "/");
            if (token != NULL) {
               value = token;
            }
         }
    }

    free(request);

    if (resource == NULL) {
         send(client, NoResource, strlen(NoResource), 0);
    } else if (strcmp(resource,"control") == 0) {
         if (instance != NULL) {
           if (strcmp(method, "GET") == 0) {
             if (value != NULL) {
                setControlInstance(atoi(instance),value,context);
                send(client, OKPrebody, strlen(OKPrebody), 0);
             } else {
                getControlInstance(atoi(instance),response,context);
                send(client, OKPrebody, strlen(OKPrebody), 0);
                send(client, response, strlen(response), 0);
             }
           } else {
             send(client, WrongMethod, strlen(WrongMethod), 0);
           }
         } else {
           if (strcmp(method, "GET") == 0) {
             getControlInstances(response,context);
             send(client, OKPrebody, strlen(OKPrebody), 0);
             send(client, response, strlen(response), 0);
           } else {
             send(client, WrongMethod, strlen(WrongMethod), 0);
           }
         }
    } else {
         send(client, NoResource, strlen(NoResource), 0);
    }     
    close(client);
  }
  close(server);
  return NULL;
}

pthread_t serverThread;

int serverStart(void *context) {
  int k = pthread_create (&serverThread, NULL, serverRun, context);
  if (k != 0) {
    fprintf (stderr, "%d : %s\n", k, "pthread_create : Server thread");
    return (1);
  }
  return (0);
}

void serverStop(void) {
   pthread_join (serverThread, NULL);
}

int main(int argc, char **argv)
{
    void *context = NULL;

    setTargetAddress("localhost",9800);
    initControl(context);
    serverRun(context);
}
