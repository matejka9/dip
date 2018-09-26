#include "tim571client.h"
#include "tim571common.h"
#include "tim571server.h"
#include "tim571protocol.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

tim571_peer server;

void shutdown_properly(int code);

void handle_signal_action(int sig_number)
{
  if (sig_number == SIGINT) {
    printf("SIGINT was catched!\n");
    shutdown_properly(EXIT_SUCCESS);
  }
  else if (sig_number == SIGPIPE) {
    printf("SIGPIPE was catched!\n");
    shutdown_properly(EXIT_SUCCESS);
  }
}

int setup_signals()
{
  struct sigaction sa;
  sa.sa_handler = handle_signal_action;
  if (sigaction(SIGINT, &sa, 0) != 0) {
    perror("sigaction()");
    return -1;
  }
  if (sigaction(SIGPIPE, &sa, 0) != 0) {
    perror("sigaction()");
    return -1;
  }
  
  return 0;
}

int connect_server(tim571_peer *server)
{
  // create socket
  server->socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server->socket < 0) {
    perror("socket()");
    return -1;
  }
  
  // set up addres
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(TIM571_SERVER_IPV4_ADDR);
  server_addr.sin_port = htons(TIM571_SERVER_LISTEN_PORT);
  
  server->addres = server_addr;
  
  if (connect(server->socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) != 0) {
    perror("connect()");
    return -1;
  }
  
  printf("Connected to %s:%d.\n", TIM571_SERVER_IPV4_ADDR, TIM571_SERVER_LISTEN_PORT);
  
  return 0;
}

int build_fd_sets(tim571_peer *server, fd_set *read_fds, fd_set *write_fds, fd_set *except_fds)
{
  FD_ZERO(read_fds);
  FD_SET(STDIN_FILENO, read_fds);
  FD_SET(server->socket, read_fds);
  
  FD_ZERO(write_fds);
  // there is smth to send, set up write_fd for server socket
  if (server->to_send.current < server->to_send.last)
    FD_SET(server->socket, write_fds);
  
  FD_ZERO(except_fds);
  FD_SET(STDIN_FILENO, except_fds);
  FD_SET(server->socket, except_fds);
  
  return 0;
}

int handle_read_from_stdin(tim571_peer *server)
{
  if (read_from_stdin(&server->to_send) != 0)
    return -1;

  printf("New message to send was enqueued right now.\n");
  
  return 0;
}

void shutdown_properly(int code)
{
  close(server.socket);
  printf("Shutdown client properly.\n");
  exit(code);
}

int process_received_message(tim571_peer *peer, char *buffer, int start, int end)
{
  return print_message_from_buffer(buffer, start, end);
}

int main(int argc,char **argv)
{
  if (setup_signals() != 0)
    exit(EXIT_FAILURE);
  
  printf("Client start.\n");
  
  create_peer(&server);
  if (connect_server(&server) != 0)
    shutdown_properly(EXIT_FAILURE);
  
  /* Set nonblock for stdin. */
  int flag = fcntl(STDIN_FILENO, F_GETFL, 0);
  flag |= O_NONBLOCK;
  fcntl(STDIN_FILENO, F_SETFL, flag);
  
  fd_set read_fds;
  fd_set write_fds;
  fd_set except_fds;

  printf("Waiting for server message or stdin input. Please, type text to send:\n");

  int maxfd = server.socket;

  while (1) {
    // Select() updates fd_set's, so we need to build fd_set's before each select()call.
    build_fd_sets(&server, &read_fds, &write_fds, &except_fds);

    int activity = select(maxfd + 1, &read_fds, &write_fds, &except_fds, NULL);

    switch (activity) {
      case -1:
        perror("select()");
        shutdown_properly(EXIT_FAILURE);
      case 0:
        printf("select() returns timeout.\n");
        shutdown_properly(EXIT_FAILURE);
      default:
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (handle_read_from_stdin(&server) != 0)
                shutdown_properly(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &except_fds)) {
            printf("except_fds for stdin.\n");
            shutdown_properly(EXIT_FAILURE);
        }

        if (FD_ISSET(server.socket, &read_fds)) {
            if (receive_from_peer(&server, &process_received_message) != 0)
                shutdown_properly(EXIT_FAILURE);
        }

        if (FD_ISSET(server.socket, &write_fds)) {
            if (send_to_peer(&server) != 0)
                shutdown_properly(EXIT_FAILURE);
        }

        if (FD_ISSET(server.socket, &except_fds)) {
            printf("except_fds for server.\n");
            shutdown_properly(EXIT_FAILURE);
        }
    }
  }
}