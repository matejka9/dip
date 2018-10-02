#include <stdlib.h>

#include "corner.h"

void create_test_lines_data(segments_data *segments)
{
  segments->count = 5;

  segments->segments[0].start.x = -2142.2473;
  segments->segments[0].start.y = 2379.2067;
  segments->segments[0].end.x = -2703.6131;
  segments->segments[0].end.y = 1755.7468;
  segments->segments[0].votes = 47;
  segments->segments[0].length = 838.9479;

  segments->segments[1].start.x = 1862.3707;
  segments->segments[1].start.y = -1798.4705;
  segments->segments[1].end.x = 2211.9580;
  segments->segments[1].end.y = -1436.4623;
  segments->segments[1].votes = 31;
  segments->segments[1].length = 503.2506;

  segments->segments[2].start.x = 2235.1006;
  segments->segments[2].start.y = 903.0393;
  segments->segments[2].end.x = 2109.3441;
  segments->segments[2].end.y = 1028.795;
  segments->segments[2].votes = 8;
  segments->segments[2].length = 177.8466;

  segments->segments[3].start.x = 1845.7393;
  segments->segments[3].start.y = 1292.4006;
  segments->segments[3].end.x = 1733.9858;
  segments->segments[3].end.y = 1404.1541;
  segments->segments[3].votes = 12;
  segments->segments[3].length = 158.0433;

  segments->segments[4].start.x = 2048.7828;
  segments->segments[4].start.y = 1089.3571;
  segments->segments[4].end.x = 1989.5010;
  segments->segments[4].end.y = 1148.6389;
  segments->segments[4].votes = 8;
  segments->segments[4].length = 83.8371;
}

int main(int argc,char **argv)
{
  corners_data results;
  segments_data segments;
  create_test_lines_data(&segments);

  corner_find_from_segments(&segments, &results);
  corner_print_data(&results);

  exit(0);
}
