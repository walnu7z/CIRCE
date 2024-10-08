#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ctype.h>
#include "connection.h"


// Function to initialize server configuration with defaults
struct server_config* initialize_config() {
    //struct server_config* config;
    struct server_config* config = (struct server_config*)malloc(sizeof(struct server_config));
    config->port = DEFAULT_PORT;
    config->reuse_address = 1;
    config->listen_queue_size = DEFAULT_QUEUE_SIZE;
    strcpy(config->ip_address, DEFAULT_IP);
    return config;
}

// Getter and Setter functions to update configuration parameters
void set_port(struct server_config *config, int port) {
    config->port = port;
}

int get_port(struct server_config *config) {
    return config->port;
}

void set_ip_address(struct server_config *config, const char *ip_address) {
    strncpy(config->ip_address, ip_address, sizeof(config->ip_address)-1);
    config->ip_address[sizeof(config->ip_address)-1] = '\0';
}

const char* get_ip_address(struct server_config *config) {
    return config->ip_address;
}

void set_reuse_address(struct server_config *config, int reuse) {
    config->reuse_address = reuse;
}

int get_reuse_address(struct server_config *config) {
    return config->reuse_address;
}

void set_listen_queue_size(struct server_config *config, int size) {
    config->listen_queue_size = size;
}

int get_listen_queue_size(struct server_config *config) {
    return config->listen_queue_size;
}

// Function to create a socket
int create_socket() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

// Function to set socket options based on the configuration
void set_socket_options(int server_fd, struct server_config *config) {
    if (config->reuse_address) {
        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            perror("setsockopt failed");
            exit(EXIT_FAILURE);
        }
    }
}

// Function to bind the socket using the configuration
void bind_socket(int server_fd, struct sockaddr_in *address, struct server_config *config) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = inet_addr(config->ip_address);  // Use IP from config
    address->sin_port = htons(config->port);  // Use port from config

    if (bind(server_fd, (struct sockaddr*)address, sizeof(*address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

// Function to start listening with configurable queue size
void start_listening(int server_fd, struct server_config *config) {
    if (listen(server_fd, config->listen_queue_size) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

// Function to accept a new connection
int accept_connection(int server_fd, struct sockaddr_in *address) {
    socklen_t addrlen = sizeof(*address);
    int new_socket = accept(server_fd, (struct sockaddr*)address, &addrlen);
    if (new_socket < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    return new_socket;
}

int connect_to_server(int server_fd, struct sockaddr_in *server_addr) {
    if (connect(server_fd, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("Connection failed");
        return 0;
    }
    return 1;
}


// Function to set up the server address structure
int setup_server_address(struct server_config *config, struct sockaddr_in *address) {
    address->sin_family = AF_INET;
    address->sin_port = htons(config->port);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, config->ip_address, &address->sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return 0;
    }
    return 1;
}


int parse_arguments(int argc, const char *argv[], char *ip_address, int *port) {
    if (argc > 5 || argc == 3) {
        fprintf(stderr, "Usage: %s --ip <ip_address> --port <port_number>\n", argv[0]);
        return -1;
    }

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "--ip") == 0) {
            if (!is_valid_ip(argv[i+1])) {
                fprintf(stderr, "Invalid IP address: %s\n", argv[i+1]);
                return -1;
            }
            strcpy(ip_address, argv[i+1]);
        } else if (strcmp(argv[i], "--port") == 0) {
            if (!is_valid_port(argv[i+1])) {
                fprintf(stderr, "Invalid port number: %s\n", argv[i+1]);
                return -1;
            }
            *port = atoi(argv[i+1]);
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return -1;
        }
    }

    return 0;
}


// Function to verify if a string is a valid IP address
int is_valid_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;
}

// Function to verify if a string is a valid port number (1-65535)
int is_valid_port(const char *port_str) {
    int port = atoi(port_str);
    if (port <= 0 || port > 65535) {
        return 0;
    }
    // Use size_t for the loop variable to match strlen's return type
    for (size_t i = 0; i < strlen(port_str); i++) {
        if (!isdigit((unsigned char)port_str[i])) {
            return 0;
        }
    }
    return 1;
}