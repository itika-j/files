#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <mosquitto.h>
#include "../include/subscriber.h"
#include "../include/publisher.h"

#define MAX_MESSAGE_LENGTH 100
#define MAX_SUB_THREADS 10
#define BROKER_IP "127.0.0.1"

char broker_ip[20];
char input[MAX_MESSAGE_LENGTH];

struct mosquitto *pub_mosq = NULL;

pthread_t sub_threads[MAX_SUB_THREADS];
struct SubscriberThreadData sub_thread_data[MAX_SUB_THREADS];

CircularQueue message_queue; // Define the queue globally
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER; // Initialize the mutex


int main() {

    int num_sub_threads = 0;
    pthread_mutex_init(&queue_mutex, NULL);
    message_queue.front = 0;
    message_queue.rear = -1;
    message_queue.count = 0;

	printf("Enter command (connect/subscribe/publish/quit)\n");
	
    while (1) {
        // Process received messages from the circular queue
        pthread_mutex_lock(&queue_mutex);

        printf("AB%d\n", message_queue.count);
        while (message_queue.count > 0) {
            printf("AB%d\n", message_queue.count);
            Message message = message_queue.messages[message_queue.front];
            printf("Received message on topic %s: %s\n", message.topic, message.payload);
            message_queue.front = (message_queue.front + 1) % QUEUE_SIZE;
            message_queue.count--;
        }
        pthread_mutex_unlock(&queue_mutex);

        printf(">>>");
        fgets(input, sizeof(input), stdin);
        strtok(input, "\n");

        if (strcmp(input, "c") == 0) {
            printf("Enter MQTT broker IP address: ");
            //fgets(broker_ip, sizeof(broker_ip), stdin);
            //strtok(broker_ip, "\n");
			strcpy(broker_ip, BROKER_IP);

            initialize_mosquitto(&pub_mosq, broker_ip);
        } else if (strcmp(input, "s") == 0) {
            if (num_sub_threads < MAX_SUB_THREADS) {
                char subscriber_topic[50];
                printf("Enter subscriber topic: ");
                fgets(subscriber_topic, sizeof(subscriber_topic), stdin);
                strtok(subscriber_topic, "\n");

                sub_thread_data[num_sub_threads].topic = strdup(subscriber_topic);
                sub_thread_data[num_sub_threads].broker_ip = broker_ip;

                if (pthread_create(&sub_threads[num_sub_threads], NULL, subscriber_thread, &sub_thread_data[num_sub_threads]) != 0) {
                    fprintf(stderr, "Error creating subscriber thread.\n");
                    return 1;
                }
                printf("%s",subscriber_topic);
                continue;
                num_sub_threads++;
            } else {
                fprintf(stderr, "Maximum number of subscriber threads reached.\n");
            }
        } else if (strcmp(input, "p") == 0) {
            if (pub_mosq == NULL) {
                printf("Please connect to MQTT broker first.\n");
            } else {
                char publisher_topic[50];
                printf("Enter a message to publish (or 'quit' to exit): ");
                fgets(input, sizeof(input), stdin);
                strtok(input, "\n");

                if (strcmp(input, "quit") == 0) {
                    break;
                }

                printf("Enter publisher topic: ");
                fgets(publisher_topic, sizeof(publisher_topic), stdin);
                strtok(publisher_topic, "\n");

                publish_message(pub_mosq, input, publisher_topic);
            }
        } else if (strcmp(input, "quit") == 0) {
            break;
        } else {
            printf("Invalid command.\n");
        }
		
        printf("next it");
    }

    if (pub_mosq) {
        cleanup_mosquitto(pub_mosq);
    }

    for (int i = 0; i < num_sub_threads; i++) {
        pthread_cancel(sub_threads[i]);
        pthread_join(sub_threads[i], NULL);
        cleanup_mosquitto(sub_thread_data[i].mosq);
        free(sub_thread_data[i].topic);
    }

    return 0;
}
