#include "tim571server.h"
#include "tim571common.h"
#include "tim571protocol.h"
#include "tim571resource.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define NO_SOCKET -1
#define MAX_PEERS 3

int listen_sock;
tim571_peer connection_list[MAX_PEERS];

char resource_buffer[MAX_BUFFER_SIZE];


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

int start_listen_socket(int *listen_sock)
{
  // Obtain a file descriptor for our "listening" socket.
  *listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (*listen_sock < 0) {
    perror("socket");
    return -1;
  }
 
  int reuse = 1;
  if (setsockopt(*listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) {
    perror("setsockopt");
    return -1;
  }
  
  struct sockaddr_in my_addr;
  memset(&my_addr, 0, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = inet_addr(TIM571_SERVER_IPV4_ADDR);
  my_addr.sin_port = htons(TIM571_SERVER_LISTEN_PORT);
 
  if (bind(*listen_sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) != 0) {
    perror("bind");
    return -1;
  }
 
  // start accept peer connections
  if (listen(*listen_sock, TIM571_SERVER_MAX_PEERS) != 0) {
    perror("listen");
    return -1;
  }
  printf("Accepting connections on port %d.\n", (int)TIM571_SERVER_LISTEN_PORT);
 
  return 0;
}

void shutdown_properly(int code)
{
  int i;

  deinit_tim571resource();
  
  close(listen_sock);
  
  for (i = 0; i < TIM571_SERVER_MAX_PEERS; ++i)
    if (connection_list[i].socket != NO_SOCKET)
      close(connection_list[i].socket);
    
  printf("Shutdown server properly.\n");
  exit(code);
}

int build_fd_sets(fd_set *read_fds, fd_set *write_fds, fd_set *except_fds)
{
  int i;
  
  FD_ZERO(read_fds);
  FD_SET(listen_sock, read_fds);
  for (i = 0; i < MAX_PEERS; ++i)
    if (connection_list[i].socket != NO_SOCKET)
      FD_SET(connection_list[i].socket, read_fds);

  FD_ZERO(write_fds);
  for (i = 0; i < MAX_PEERS; ++i)
    if (connection_list[i].socket != NO_SOCKET && connection_list[i].to_send.current < connection_list[i].to_send.last)
      FD_SET(connection_list[i].socket, write_fds);
  
  FD_ZERO(except_fds);
  FD_SET(STDIN_FILENO, except_fds);
  FD_SET(listen_sock, except_fds);
  for (i = 0; i < MAX_PEERS; ++i)
    if (connection_list[i].socket != NO_SOCKET)
      FD_SET(connection_list[i].socket, except_fds);
 
  return 0;
}

int handle_new_connection()
{
  struct sockaddr_in peer_addr;
  memset(&peer_addr, 0, sizeof(peer_addr));
  socklen_t peer_len = sizeof(peer_addr);
  int new_peer_sock = accept(listen_sock, (struct sockaddr *)&peer_addr, &peer_len);
  if (new_peer_sock < 0) {
    perror("accept()");
    return -1;
  }
  
  char peer_ipv4_str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &peer_addr.sin_addr, peer_ipv4_str, INET_ADDRSTRLEN);
  
  printf("Incoming connection from %s:%d.\n", peer_ipv4_str, peer_addr.sin_port);
  
  int i;
  for (i = 0; i < MAX_PEERS; ++i) {
    if (connection_list[i].socket == NO_SOCKET) {
      connection_list[i].socket                 = new_peer_sock;
      connection_list[i].addres                 = peer_addr;
      connection_list[i].to_send.current        = 0;
      connection_list[i].to_send.last           = 0;
      connection_list[i].to_receive.current     = 0;
      connection_list[i].info.one               = 0;
      connection_list[i].info.repeat            = 0;
      return 0;
    }
  }
  
  printf("There is too much connections. Close new connection %s:%d.\n", peer_ipv4_str, peer_addr.sin_port);
  close(new_peer_sock);
  return -1;
}

void close_peer_connection(tim571_peer *peer)
{
  printf("Close peer socket for %s.\n", peer_get_addres_str(peer));
  
  close(peer->socket);
  peer->socket = NO_SOCKET;
  reset_peer_data(peer);
}

int process_received_message(tim571_peer *peer, char *buffer, int start, int end)
{
  int len = end - start;
  char *message = malloc((len + 1) * sizeof(char));

  if (message) {
    memcpy(message, &buffer[start], len * sizeof(char));
    message[len]  = '\0';

    if (strcmp(message, REQUEST_ONE) == 0) {
      printf("Send one time data to %s\n", peer_get_addres_str(peer));
      peer->info.one = 1;
    } else if (strcmp(message, REQUEST_REPEAT_START) == 0) {
      printf("Start sending data to %s\n", peer_get_addres_str(peer));
      peer->info.repeat = 1;
    } else if (strcmp(message, REQUEST_REPEAT_END) == 0) {
      printf("Stop sending data to %s\n", peer_get_addres_str(peer));
      peer->info.repeat = 0;
    } else {
      printf("Unknown message from %s: %s\n", peer_get_addres_str(peer), message);
    }

    free(message);

    return 0;
  }
  return -1;
}

long getMicrotime()
{
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

void prepare_new_message_to_peers()
{
  int length = get_tim571resource(&resource_buffer[0], MAX_BUFFER_SIZE);

  int i;
  for (i = 0; i < MAX_PEERS; ++i) {
    if (connection_list[i].socket != NO_SOCKET) {
      // if (connection_list[i].info.one || connection_list[i].info.repeat) {
        if (append_bytes_to_data(&connection_list[i].to_send, &resource_buffer[0], length) > 0) {
          connection_list[i].info.one = 0;
        }
      // }
    }
  }
}

void set_next_timeout(time_t *last_sent_data, struct timeval *tv)
{
  long rawtime = getMicrotime();

  if (*last_sent_data + TIM571_SERVER_SEND_FREQUENCE <= rawtime) {
    *last_sent_data = rawtime;
    prepare_new_message_to_peers();
  }

  time_t difference = rawtime - *last_sent_data;
  time_t missing = TIM571_SERVER_SEND_FREQUENCE - difference;
  time_t missing_microsecs = missing % TIM571_SERVER_SEC_NUMBER_OF_MILISECS;
  time_t mising_seconds = (missing - missing_microsecs) / TIM571_SERVER_SEC_NUMBER_OF_MILISECS;

  struct timeval new_tv = {mising_seconds, missing_microsecs};
  *tv = new_tv;
}

int main(int argc, char **argv)
{
  if (setup_signals() != 0)
    exit(EXIT_FAILURE);
  
  if (start_listen_socket(&listen_sock) != 0)
    exit(EXIT_FAILURE);

  int i;
  for (i = 0; i < MAX_PEERS; ++i) {
    connection_list[i].socket = NO_SOCKET;
    create_peer(&connection_list[i]);
  }

  if (init_tim571resource()!= 0)
    shutdown_properly(EXIT_FAILURE);

  time_t last_sent_data = 0;
  struct timeval tv = {0, 0};

  fd_set read_fds;
  fd_set write_fds;
  fd_set except_fds;
  
  int high_sock = listen_sock;

  printf("Waiting for incoming connections.\n");

  while (1) {
    set_next_timeout(&last_sent_data, &tv);

    build_fd_sets(&read_fds, &write_fds, &except_fds);
    
    high_sock = listen_sock;
    for (i = 0; i < MAX_PEERS; ++i) {
      if (connection_list[i].socket > high_sock)
        high_sock = connection_list[i].socket;
    }

    int activity = select(high_sock + 1, &read_fds, &write_fds, &except_fds, &tv);

    switch (activity) {
      case -1:
        perror("select()");
        shutdown_properly(EXIT_FAILURE);
      case 0:
        printf("select() timeout.\n");
        break;
      default:
        if (FD_ISSET(listen_sock, &read_fds)) {
          handle_new_connection();
        }

        if (FD_ISSET(listen_sock, &except_fds)) {
          printf("Exception listen socket fd.\n");
          shutdown_properly(EXIT_FAILURE);
        }

        for (i = 0; i < MAX_PEERS; ++i) {
          if (connection_list[i].socket != NO_SOCKET && FD_ISSET(connection_list[i].socket, &read_fds)) {
            if (receive_from_peer(&connection_list[i], &process_received_message) != 0) {
              close_peer_connection(&connection_list[i]);
              continue;
            }
          }
  
          if (connection_list[i].socket != NO_SOCKET && FD_ISSET(connection_list[i].socket, &write_fds)) {
            if (send_to_peer(&connection_list[i]) != 0) {
              close_peer_connection(&connection_list[i]);
              continue;
            }
          }

          if (connection_list[i].socket != NO_SOCKET && FD_ISSET(connection_list[i].socket, &except_fds)) {
            printf("Exception peer fd.\n");
            close_peer_connection(&connection_list[i]);
            continue;
          }
        }
    }

    printf("And we are still waiting for peers.\n");
  }
}

// TODO pokracuj send message