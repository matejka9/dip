#ifndef LASER_TIM571COMMON_H
#define LASER_TIM571COMMON_H

#include "tim571protocol.h"

#include <arpa/inet.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_SEND_SIZE 100
#define MAX_BUFFER_SIZE 102400

typedef struct {
  size_t one;
  size_t repeat;
} tim571_request;

typedef struct {
  char buffer[MAX_BUFFER_SIZE];
  int current;
  int last;
} tim571_data;

typedef struct {
  int socket;
  struct sockaddr_in addres;
  
  tim571_request info;

  tim571_data to_send;
  tim571_data to_receive;
} tim571_peer;

void reset_peer_data(tim571_peer *peer)
{
  peer->to_send.current     = 0;
  peer->to_send.last        = 0;
  peer->to_receive.current  = 0;
  peer->to_receive.last     = 0;
  peer->info.one            = 0;
  peer->info.repeat         = 0;
}

int create_peer(tim571_peer *peer)
{ 
  reset_peer_data(peer);
  
  return 0;
}

void remove_space_from_begin(tim571_data *data)
{
  if (data->current > 0) {
    memmove(data->buffer, data->buffer + data->current, data->last - data->current);
    data->last     -= data->current;
    data->current   = 0;
  }
}

int append_bytes_to_data(tim571_data *data, char *buffer, int length)
{
  if (data->last + length < MAX_BUFFER_SIZE) {
    strncpy(&data->buffer[data->last], buffer, length);
    data->last += length;
    return length;
  }
  return 0;
}

char *peer_get_addres_str(tim571_peer *peer)
{
  static char ret[INET_ADDRSTRLEN + 10];
  char peer_ipv4_str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &peer->addres.sin_addr, peer_ipv4_str, INET_ADDRSTRLEN);
  sprintf(ret, "%s:%d", peer_ipv4_str, peer->addres.sin_port);
  
  return ret;
}

int index_of_char_in_data(tim571_data *data, char c)
{
  for (int i = data->current; i < data->last; i++) {
    if (data->buffer[i] == c)
      return i;
  }
  return -1;
}

int print_message_from_buffer(char *buffer, int start, int end)
{
  int len = end - start;
  char *message = malloc((len + 1) * sizeof(char));

  if (message) {
    memcpy(message, &buffer[start], len * sizeof(char));
    message[len]  = '\0';

    printf("Message: Length=%lu Data=\"%s\"\n", strlen(message), message);

    free(message);

    return 0;
  }
  return -1;
}

int print_message(tim571_data *data)
{
  return print_message_from_buffer(&data->buffer[data->current], data->current, data->last);
}

int handle_received_message(tim571_peer *peer, int (*message_handler)(tim571_peer *, char *, int, int))
{
  int start = index_of_char_in_data(&peer->to_receive, START_CHAR);
  int end   = index_of_char_in_data(&peer->to_receive, END_CHAR);

  if (start > -1 && end > -1) {
    printf("Start index:%d End index:%d\n", start, end);
    if (message_handler(peer, &peer->to_receive.buffer[0], start + 1, end) != 0)
      return -1;
    peer->to_receive.current = end + 1;
  } else if (start > 0) {
    peer->to_receive.current = start;
  } else if (start == -1 && end == 1) {
    peer->to_receive.current = peer->to_receive.last;
  }
  
  return 0;
}

int receive_from_peer(tim571_peer *peer, int (*message_handler)(tim571_peer *,char *, int, int))
{
  remove_space_from_begin(&peer->to_receive);
  // printf("Ready for recv() from %s.\n", peer_get_addres_str(peer));
  
  size_t len_to_receive;
  ssize_t received_count;
  size_t received_total = 0;
  do {
    // Is completely received?
    if (peer->to_receive.last >= sizeof(peer->to_receive.buffer)) {
      if (handle_received_message(peer, message_handler) < 0)
        return -1;
    }
    
    // Count bytes to send.
    len_to_receive = sizeof(peer->to_receive.buffer) - peer->to_receive.last;
    if (len_to_receive > MAX_SEND_SIZE)
      len_to_receive = MAX_SEND_SIZE;
    
    printf("Let's try to recv() %zd bytes... ", len_to_receive);
    received_count = recv(peer->socket, (char *)&peer->to_receive.buffer + peer->to_receive.last, len_to_receive, MSG_DONTWAIT);
    if (received_count < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        printf("peer is not ready right now, try again later.\n");
      }
      else {
        perror("recv() from peer error");
        return -1;
      }
    } 
    else if (received_count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      break;
    }
    // If recv() returns 0, it means that peer gracefully shutdown. Shutdown peer.
    else if (received_count == 0) {
      printf("recv() 0 bytes. peer gracefully shutdown.\n");
      return -1;
    }
    else if (received_count > 0) {
      peer->to_receive.last += received_count;
      received_total += received_count;
      printf("recv() %zd bytes\n", received_count);
    }
  } while (received_count > 0);

  if (handle_received_message(peer, message_handler) < 0)
    return -1;
  
  printf("Total recv()'ed %zu bytes.\n", received_total);
  return 0;
}

int send_to_peer(tim571_peer *peer)
{
  printf("Ready for send() to %s.\n", peer_get_addres_str(peer));
  
  size_t len_to_send;
  ssize_t send_count;
  size_t send_total = 0;
  do {
    if (peer->to_send.current >= peer->to_send.last) {
      printf("There is no pending to send() message...");
      break;
    }
    
    // Count bytes to send.
    len_to_send = peer->to_send.last - peer->to_send.current;
    if (len_to_send > MAX_SEND_SIZE)
      len_to_send = MAX_SEND_SIZE;
    
    printf("Let's try to send() %zd bytes... ", len_to_send);
    send_count = send(peer->socket, (char *)&peer->to_send.buffer + peer->to_send.current, len_to_send, 0);
    if (send_count < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        printf("peer is not ready right now, try again later.\n");
      }
      else {
        perror("send() from peer error");
        return -1;
      }
    }
    // we have write as many as possible
    else if (send_count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      break;
    }
    else if (send_count == 0) {
      printf("send()'ed 0 bytes. It seems that peer can't accept data right now. Try again later.\n");
      break;
    }
    else if (send_count > 0) {
      peer->to_send.current += send_count;
      send_total += send_count;
      printf("send()'ed %zd bytes.\n", send_count);
    }
  } while (send_count > 0);

  remove_space_from_begin(&peer->to_send);

  printf("Total send()'ed %zu bytes.\n", send_total);
  return 0;
}

int read_from_stdin(tim571_data *data)
{
  size_t max_len = MAX_BUFFER_SIZE - data->last;
  memset(&data->buffer[data->last], 0, max_len);
  
  ssize_t read_count = 0;
  ssize_t total_read = 0;
  
  do {
    read_count = read(STDIN_FILENO, (char *)&data->buffer + data->last, max_len);
    if (read_count < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("read()");
      return -1;
    }
    else if (read_count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      break;
    }
    else if (read_count > 0) {
      total_read += read_count;
      if (total_read > max_len) {
        printf("Message too large and will not be accepted.\n");
        fflush(STDIN_FILENO);
        return 0;
      }
    }
  } while (read_count > 0);
  
  // if (len > 0 && data->buffer[data->last + total_read - 1] == '\n')
  //   data->buffer[data->last + total_read - 1] = '\0';
  data->last += total_read;
  
  printf("Read from stdin %zu bytes. Let's prepare message to send.\n", total_read);

  return 0;
}

#endif //LASER_TIM571COMMON_H
