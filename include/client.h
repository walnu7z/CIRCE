#ifndef CLIENT_H
#define CLIENT_H
#include <stdbool.h>
#include "connection.h"
#include "common.h"

typedef struct {
    bool logged_in;    
    int socket;
    char username[9];
    Status status;
} Client;

typedef void (*client_process_message_func)(Client *client, char *buffer, const char *msg_type);

typedef struct {
    Client *client;
    int socket;
    client_process_message_func client_process_message;
} client_listener_args_t;

typedef enum {
    TYPE_NEW_USER = 13,
    TYPE_NEW_STATUS = 14,
    TYPE_USER_LIST = 15,
    TYPE_TEXT_FROM = 16,
    TYPE_PUBLIC_TEXT_FROM = 17,
    TYPE_JOINED_ROOM = 18,
    TYPE_ROOM_USER_LIST = 19,
    TYPE_ROOM_TEXT_FROM = 20,
    TYPE_LEFT_ROOM = 21,
    TYPE_DISCONNECTED = 22,
    TYPE_CLIENT_RESPONSE = 23,
    TYPE_NOT_KNOWN = 24
} ClientMessageType;

enum Command {
    CMD_HELP,
    CMD_ECHO,
    CMD_LOGIN,
    CMD_LOGOUT,
    CMD_STATUS,
    CMD_UNKNOWN
};


//manage interrupt signal
void handle_sigint(int sig);

//manage data client
Client *client_init(int sock);
Client *client(int sock);

//client listener 
void *client_listener(void *arg);

//functions in the client
int identify_client(int sock, const char* user);
int de_identify_client(Client *client);
void ask_for_username(char *username, int max_len);
void login_client(Client *client, bool logged, Status status, const char *user);
void update_client(Client *client, bool logged, Status status);
int send_public_text(Client *client, char *msg);
void handle_status_client(Client *client, const char *status, int params);
int update_status(Client *client, const char *status);
Status get_status(const char *statusStr);










// functions that handle operations with the server
ClientMessageType client_get_type(const char *message_type);
void client_process_message(Client *client, char *, const char *);
void handle_client_new_user(Client *client,  char *buffer);
void handle_new_status(Client *client,  char *buffer);
void handle_user_list(Client *client,  char *buffer);
void handle_text_from(Client *client,  char *buffer);
void handle_public_text_from(Client *client,  char *buffer);
void handle_joined_room(Client *client,  char *buffer);
void handle_room_user_list(Client *client,  char *buffer);
void handle_room_text_from(Client *client,  char *buffer);
void handle_left_room(Client *client,  char *buffer);
void handle_disconnected(Client *client);


//Response handlers
void handle_client_response(Client *client,  char *json_str);
void handle_identify_response(Client *client,  char *json_str);
void handle_leave_room_response(Client *client,  char *json_str);
void handle_room_text_response(Client *client,  char *json_str);
void handle_room_users_response(Client *client,  char *json_str);
void handle_join_room_response(Client *client,  char *json_str);
void handle_invite_response(Client *client,  char *json_str);
void handle_text_response(Client *client,  char *json_str);
void handle_unknown_operation_response(Client *client,  char *json_str);



// command handlers
// Get input from the user
bool get_input(char *buffer);
// function to select command
enum Command get_command_type(const char* command);
// Parse the command and arguments from the input buffer
void execute_command(Client *client, const char *command, const char *args, int n_params);
// Handle the '\help' command
bool parse_command(const char *buffer, char *command, char *args);
// function to count commands
int count_tokens(const char *str, const char *delim);
// Check if the command is '\leave'
bool is_leave_command(const char *command);
// Execute the parsed command and pass the arguments
void handle_help(const char *args);
// Handle the '\echo' command
void handle_echo(const char *args);
// Handle login command
void handle_login(Client *client, const char *command, int n_params);
// logout client command
void handle_logout(Client *client);
// Handle unknown commands
void handle_unknown(const char *command);



int client_create_listener(Client *client, client_process_message_func process_message, client_listener_args_t *args, void *(*listener)(void *));


#endif