#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <mosquitto.h>

#define BROKER_PORT 1883
#define QUEUE_SIZE 15
#define MAX_SUB_THREADS 10
#define MAX_MESSAGE_LENGTH 100

typedef struct Message {
    char topic[50];
    char payload[MAX_MESSAGE_LENGTH];
} Message;

struct SubscriberThreadData {
    struct mosquitto *mosq;
    const char *topic;
    const char *broker_ip;
};

typedef struct CircularQueue {
    Message messages[QUEUE_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
} CircularQueue;


extern CircularQueue message_queue;
extern pthread_mutex_t queue_mutex;

void *subscriber_thread(void *arg);

#endif // SUBSCRIBER_H
