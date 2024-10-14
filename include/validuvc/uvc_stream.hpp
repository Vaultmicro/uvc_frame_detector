// packet_queue.hpp

#ifndef PACKET_QUEUE_HPP
#define PACKET_QUEUE_HPP

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_QUEUE_SIZE 100
extern uint8_t* packet_queue[MAX_QUEUE_SIZE];
extern size_t packet_sizes[MAX_QUEUE_SIZE];
extern int queue_front;
extern int queue_rear;
extern pthread_mutex_t queue_mutex;
extern pthread_cond_t queue_cv;
extern int stop_processing;

void process_packets();

#endif // PACKET_QUEUE_HPP
