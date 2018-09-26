#ifndef LASER_TIM571RESOURCE_H
#define LASER_TIM571RESOURCE_H

int init_tim571resource();
int get_tim571resource(char *buffer, int buffer_size);
void deinit_tim571resource();

#endif //LASER_TIM571RESOURCE_H
