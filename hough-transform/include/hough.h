#ifndef _HOUGH_H_
#define _HOUGH_H_

#include "tim571.h"

#define LINE_MAX_DATA_COUNT 10

typedef struct line_struct {
  int distance;
  int angle;
  int votes;
} line_data;

typedef struct lines_struct {
  line_data lines[LINE_MAX_DATA_COUNT];
  int line_count;
} lines_data;

typedef struct hough_config_struct {
  int distance_max; // 15000
  int distance_step; // 15

  int angle_step; // 5

  int votes_min; // 10

  int bad_distance; // 0
  int bad_rssi; // 0
} hough_config;

void hough_get_lines_data(hough_config *config, tim571_status_data *status_data, uint16_t *distance, uint8_t *rssi, lines_data *data);
void printf_lines_data(lines_data *data);

void test_hough_methods(hough_config *config, tim571_status_data *status_data, uint16_t *distance, uint8_t *rssi);

#endif
