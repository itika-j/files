#include <stdio.h>
#include <pthread.h>
#include <mosquitto.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 100
#define BROKER_HOST "127.0.0.1"
#define BROKER_PORT 1883
#define SUBSCRIBER_TOPIC "test_sub"
#define PUBLISHER_TOPIC "test_pub"

struct ThreadData {
    struct mosquitto *mosq;
    const char *topic;
};

void on_connect(struct mosquitto *mosq, void *userdata, int rc) {
    if (rc == 0) {
        printf("Connected to MQTT broker\n");
    } else {
        fprintf(stderr, "Failed to connect: %s\n", mosquitto_connack_string(rc));
    }
}

void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg) {
    printf("Received message on topic %s: %s\n", msg->topic, (char *)msg->payload);
}

void *subscriber_thread(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;

    mosquitto_lib_init();

    data->mosq = mosquitto_new("subscribe-test", true, NULL);

    if (!data->mosq) {
        fprintf(stderr, "Error: Out of memory.\n");
        return NULL;
    }

    mosquitto_connect_callback_set(data->mosq, on_connect);
    mosquitto_message_callback_set(data->mosq, on_message);

    if (mosquitto_connect(data->mosq, BROKER_HOST, BROKER_PORT, 10) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Unable to connect to the broker.\n");
        return NULL;
    }

    mosquitto_subscribe(data->mosq, NULL, data->topic, 0);
    mosquitto_loop_forever(data->mosq, -1, 1);

    mosquitto_destroy(data->mosq);
    mosquitto_lib_cleanup();

    return NULL;
}

void publish_message(const char *message) {
    struct mosquitto *pub_mosq = NULL;

    mosquitto_lib_init();

    pub_mosq = mosquitto_new(NULL, true, NULL);

    if (!pub_mosq) {
        fprintf(stderr, "Error: Out of memory.\n");
        return;
    }

    if (mosquitto_connect(pub_mosq, BROKER_HOST, BROKER_PORT, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Unable to connect to the broker.\n");
        return;
    }

    mosquitto_publish(pub_mosq, NULL, PUBLISHER_TOPIC, strlen(message), message, 0, false);
	printf("---->Sent to topic %s\n", PUBLISHER_TOPIC);

    mosquitto_disconnect(pub_mosq);
    mosquitto_destroy(pub_mosq);
    mosquitto_lib_cleanup();
}

int main() {
    pthread_t sub_thread;
    struct ThreadData sub_thread_data;

    // Create subscriber thread
    sub_thread_data.topic = SUBSCRIBER_TOPIC;
    if (pthread_create(&sub_thread, NULL, subscriber_thread, &sub_thread_data) != 0) {
        fprintf(stderr, "Error creating subscriber thread.\n");
        return 1;
    }

    printf("Enter a message to publish (or 'quit' to exit)\n");

    // Publisher loop
    while (1) {
        char message[MAX_MESSAGE_LENGTH];
        fgets(message, sizeof(message), stdin);
        strtok(message, "\n"); // Remove trailing newline

        if (strcmp(message, "quit") == 0) {
            break;
        }

        publish_message(message);
    }

    // Wait for the subscriber thread to finish
	pthread_cancel(sub_thread);
    pthread_join(sub_thread, NULL);

    return 0;
}
