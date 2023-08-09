#include <stdio.h>
#include <pthread.h>
#include <mosquitto.h>
#include <string.h>
#include "../include/subscriber.h"

extern CircularQueue message_queue;
extern pthread_mutex_t queue_mutex;

void on_connect(struct mosquitto *mosq, void *userdata, int rc) {
    if (rc == 0) {
        printf("Successfully subscribed\n");
    } else {
        fprintf(stderr, "Failed to connect: %s\n", mosquitto_connack_string(rc));
    }
}


void print_queue(CircularQueue *queue) {
    printf("Queue contents:\n");
    for (int i = 0; i <= queue->count; i++) {
        int index = (queue->front + i) % QUEUE_SIZE;
        printf("Topic: %s, Payload: %s\n", queue->messages[index].topic, queue->messages[index].payload);
    }
}



void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg) {
    printf("Received message on topic %s: %s\n", msg->topic, (char *)msg->payload);
    pthread_mutex_lock(&queue_mutex); // Lock the mutex before accessing the shared queue

    if (message_queue.count < QUEUE_SIZE) {
        message_queue.rear = (message_queue.rear + 1) % QUEUE_SIZE;
        strcpy(message_queue.messages[message_queue.rear].topic, msg->topic);
        strcpy(message_queue.messages[message_queue.rear].payload, (char *)msg->payload);
        message_queue.count++;
    }

    pthread_mutex_unlock(&queue_mutex); 
	
	print_queue(&message_queue);
}


void *subscriber_thread(void *arg) {
    struct SubscriberThreadData *data = (struct SubscriberThreadData *)arg;

    mosquitto_lib_init();

    char unique_id[20];
    snprintf(unique_id, sizeof(unique_id), "subscribe-%lu", (unsigned long)pthread_self());

    data->mosq = mosquitto_new(unique_id, true, NULL);

    if (!data->mosq) {
        fprintf(stderr, "Error: Out of memory.\n");
        return NULL;
    }
	
	print_queue(&message_queue);

    mosquitto_connect_callback_set(data->mosq, on_connect);
    mosquitto_message_callback_set(data->mosq, on_message);

    if (mosquitto_connect(data->mosq, data->broker_ip, BROKER_PORT, 10) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Unable to connect to the broker.\n");
        return NULL;
    }

    mosquitto_subscribe(data->mosq, NULL, data->topic, 0);

    int rc;
    do {
        rc = mosquitto_loop(data->mosq, -1, 1);
    } while (rc == MOSQ_ERR_SUCCESS);

    mosquitto_destroy(data->mosq);
    mosquitto_lib_cleanup();

    return NULL;
	
	
}
