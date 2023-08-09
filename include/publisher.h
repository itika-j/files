#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <mosquitto.h>

#define BROKER_PORT 1883

void initialize_mosquitto(struct mosquitto **mosq, const char *broker_ip);
void publish_message(struct mosquitto *mosq, const char *message, const char *topic);

#endif // PUBLISHER_H
