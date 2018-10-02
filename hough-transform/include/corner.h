#ifndef _SEGMENT_H_
#define _SEGMENT_H_

#include "segment.h"

#define CORNERS_MAX_DATA_COUNT 1

typedef struct segments_struct {
  segment_data segments[CORNERS_MAX_DATA_COUNT];
  int count;
} segments_data;

void corner_find_from_segments(segments_data *segments);
void corner_find_from_lines(lines_data *lines);

#endif
