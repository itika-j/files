#include <stdio.h>
#include <mosquitto.h>
#include <string.h>
#include "../include/publisher.h"

void initialize_mosquitto(struct mosquitto **mosq, const char *broker_ip) {
    mosquitto_lib_init();
    *mosq = mosquitto_new(NULL, true, NULL);

    if (!*mosq) {
        fprintf(stderr, "Error: Out of memory.\n");
        return;
    }

    if (mosquitto_connect(*mosq, broker_ip, BROKER_PORT, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Unable to connect to the broker.\n");
        return;
    }
}

void publish_message(struct mosquitto *mosq, const char *message, const char *topic) {
    mosquitto_publish(mosq, NULL, topic, strlen(message), message, 0, false);
    printf("---->Sent to topic %s\n", topic);
}

void cleanup_mosquitto(struct mosquitto *mosq) {
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}