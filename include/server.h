#ifndef CIRCE_SERVER_H
#define CIRCE_SERVER_H

typedef enum {
    TYPE_IDENTIFY = 1,
    TYPE_STATUS = 2,
    TYPE_USERS = 3,
    TYPE_TEXT = 4,
    TYPE_PUBLIC_TEXT = 5,
    TYPE_NEW_ROOM = 6,
    TYPE_INVITE = 7,
    TYPE_JOIN_ROOM = 8,
    TYPE_ROOM_USERS = 9,
    TYPE_ROOM_TEXT = 10,
    TYPE_LEAVE_ROOM = 11,
    TYPE_DISCONNECT = 12,
    TYPE_RESPONSE = 22,
    TYPE_UNKNOWN = 24
} MessageType;

typedef struct {
    GHashTable *user_table;  // Hash table to manage users
} Server;

typedef struct {
    char *username;
    int status;
    int socket;
} UserInfo;


typedef void (*process_message_func)(Server *server, UserInfo *user, char *, const char *);

typedef struct {
    Server *server;
    UserInfo *user_info;
    process_message_func process_message;
} server_listener_args_t;


//listener
void *server_listener(void *arg);
GList *create_thread_pool(Server *server, UserInfo *user, GList *thread_list);
UserInfo *initialize_user();

//manage exit
static void handle_sigint(int _);


//handle requests
MessageType server_get_type(const char *message_type);
void process_message(Server *server, int, char *, const char *);
void handle_identify();
void handle_response();
void handle_new_user();
void handle_status();
void handle_users();
void handle_text();
void handle_public_text();
void handle_new_room();
void handle_invite();
void handle_join_room();
void handle_room_users();
void handle_room_text();
void handle_leave_room();
void handle_disconnect();


//server response 
int identify_response_success(int sock, char *user);
int identify_response_failed(int sock, char *user);


//server operations
int parse_user(int sock, char* buffer, char * id, size_t max_size);
int send_json_response(int sock, char *json_str, size_t size_json_str, const char *fields_and_values[][2], size_t num_fields);
void send_json_except_user(Server *server, const char *exclude_username);
void server_cleanup(Server *server);
void server_add_user(Server *server, const char *username, int status, int socket);
void server_remove_user(Server *server, const char *username);
char* Server_acceptClient();


//manage data
Server *server_init();
UserInfo* create_user(const char *username, int status, int socket);
void free_user(gpointer data);





#endif //CIRCE_SERVER_H