#include <stdio.h>
#include <mosquitto.h>
#include <string.h>

int main() {
    int rc;
    struct mosquitto *mosq;

    mosquitto_lib_init();

    mosq = mosquitto_new("publisher-test", true, NULL);

    rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 60);
    if (rc != 0) {
        printf("Client could not connect to broker! Error Code: %d\n", rc);
        mosquitto_destroy(mosq);
        return -1;
    }
    printf("We are now connected to the broker!\n");

    char message[100]; // Assuming max message length of 100 characters
    printf("Enter the message to send: ");
    fgets(message, sizeof(message), stdin);
    strtok(message, "\n"); // Remove trailing newline

    mosquitto_publish(mosq, NULL, "test/t1", strlen(message), message, 0, false);

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);

    mosquitto_lib_cleanup();
    return 0;
}
