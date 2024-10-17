#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 256
#define MAX_USERS 100

typedef struct {
    char user_id[20];
    char username[50];
    char password[100];
    int is_registered;
} UserRegistration;

typedef struct {
    char username[50];
    int is_forwarding_active;
    char forwarding_type[20];
    char destination_number[20];
    int is_busy;  // Indicates if the user is busy
} UserForwarding;

UserRegistration users[MAX_USERS];
UserForwarding userForwardings[MAX_USERS];
int userCount = 0;
int forwardingCount = 0;
//UserRegistration users1[MAX_USERS];
//int user1Count = 0;

pthread_mutex_t user_mutex = PTHREAD_MUTEX_INITIALIZER;


/*void loadUsers1FromFile() {
    FILE *file = fopen("users.txt", "r");
    if (!file) return;

    while (user1Count<MAX_USERS && fscanf(file, "%[^,],%[^,],%[^,],%d\n",
                  users1[user1Count].user_id,
                  users1[user1Count].username,
                  users1[user1Count].password,
                  &users1[user1Count].is_registered) ==4 ) {
        user1Count++;
    }
    fclose(file);
}
*/
void loadUsersFromFile() {
    FILE *file = fopen("users.txt", "r");
    if (!file) return;

    while (userCount<MAX_USERS && fscanf(file, "%[^,],%[^,],%[^,],%d\n",
                  users[userCount].user_id,
                  users[userCount].username,
                  users[userCount].password,
                  &users[userCount].is_registered) ==4 ) {
        userCount++;
    }
    fclose(file);
}

void loadForwardingsFromFile() {
    FILE *file = fopen("forwardings.txt", "r");
    if (!file) return;

    while (forwardingCount<MAX_USERS && fscanf(file, "%[^,],%d,%[^,],%s,%d\n",
                  userForwardings[forwardingCount].username,
                  &userForwardings[forwardingCount].is_forwarding_active,
                  userForwardings[forwardingCount].forwarding_type,
                  userForwardings[forwardingCount].destination_number,
                  &userForwardings[forwardingCount].is_busy) == 5) {
        forwardingCount++;
    }
    fclose(file);
}

void saveUsersToFile() {
    FILE *file = fopen("users.txt", "w");
    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s,%s,%s,%d\n",
                users[i].user_id,
                users[i].username,
                users[i].password,
                users[i].is_registered);
    }
    fclose(file);
}

void saveForwardingsToFile() {
    FILE *file = fopen("forwardings.txt", "a");
    for (int i = 0; i < forwardingCount; i++) {
        fprintf(file, "%s,%d,%s,%s,%d\n",
                userForwardings[i].username,
                userForwardings[i].is_forwarding_active,
                userForwardings[i].forwarding_type,
                userForwardings[i].destination_number,
                userForwardings[i].is_busy);
    }
    fclose(file);
}

void logCall(const char *caller) {
    FILE *file = fopen("call_log.txt", "a");
    if (!file) return;

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strcspn(timestamp, "\n")] = 0; // Remove newline character from timestamp

    fprintf(file, "Caller: %s, Timestamp: %s\n", caller, timestamp);
    fclose(file);
}

void registerUser(const char *username, const char *password, int client_socket) {
    pthread_mutex_lock(&user_mutex);
    if (userCount >= MAX_USERS) {
        send(client_socket, "User limit reached.\n", BUFFER_SIZE, 0);
        pthread_mutex_unlock(&user_mutex);
        return;
    }

    sprintf(users[userCount].user_id, "U%d", userCount + 1);
    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    users[userCount].is_registered = 1;
	userCount++;
   // initializing forwarding information
   strcpy(userForwardings[forwardingCount].username, username);
    userForwardings[forwardingCount].is_forwarding_active = 0;
    strcpy(userForwardings[forwardingCount].forwarding_type, "");
    strcpy(userForwardings[forwardingCount].destination_number, "");
    userForwardings[forwardingCount].is_busy = 0;
    forwardingCount++;

    saveUsersToFile();
   	send(client_socket, "User registered successfully. please login. \n", BUFFER_SIZE, 0);
	
    saveForwardingsToFile();
    pthread_mutex_unlock(&user_mutex);
}

void activateCallForwarding(const char *username, const char *type, const char *destination, int client_socket) {
    pthread_mutex_lock(&user_mutex);

    for (int i = 0; i < forwardingCount; i++) {
        if (strcmp(userForwardings[i].username, username) == 0) {
            userForwardings[i].is_forwarding_active = 1;
            strcpy(userForwardings[i].forwarding_type, type);
            strcpy(userForwardings[i].destination_number, destination);
            saveForwardingsToFile();
            send(client_socket, "Call forwarding activated.\n", BUFFER_SIZE, 0);
            pthread_mutex_unlock(&user_mutex);
            return;
        }
    }
    send(client_socket, "User not found or not registered.\n", BUFFER_SIZE, 0);
    pthread_mutex_unlock(&user_mutex);
}

void deactivateCallForwarding(const char *username, int client_socket) {
    pthread_mutex_lock(&user_mutex);
    for (int i = 0; i < forwardingCount; i++) {
        if (strcmp(userForwardings[i].username, username) == 0) {
            userForwardings[i].is_forwarding_active = 0;
            strcpy(userForwardings[i].forwarding_type, "");
            strcpy(userForwardings[i].destination_number, "");
            saveForwardingsToFile();
            send(client_socket, "Call forwarding deactivated.\n", BUFFER_SIZE, 0);
            pthread_mutex_unlock(&user_mutex);
            return;
        }
    }
    send(client_socket, "User not found or not registered.\n", BUFFER_SIZE, 0);
    pthread_mutex_unlock(&user_mutex);
}

void authenticateUser(const char *username, const char *password, int client_socket) {
    pthread_mutex_lock(&user_mutex);
	loadUsersFromFile();
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            send(client_socket, "Authentication successful.\n", BUFFER_SIZE, 0);
            pthread_mutex_unlock(&user_mutex);
            return;
        }
    }
    send(client_socket, "Authentication failed.\n", BUFFER_SIZE, 0);
    pthread_mutex_unlock(&user_mutex);
}

void handleCall(const char *caller, const char *callee, int client_socket) {
    pthread_mutex_lock(&user_mutex);
    for (int i = 0; i < forwardingCount; i++) {
        if (strcmp(userForwardings[i].username, callee) == 0) {
        /*    if (userForwardings[i].is_busy) {
                send(client_socket, "Callee and destination number are busy.\n", BUFFER_SIZE, 0);
                pthread_mutex_unlock(&user_mutex);
                return;
            }
			*/
            //userForwardings[i].is_busy = 0;  // Set user as busy for the call
            logCall(caller);  // Log the call

            if (userForwardings[i].is_forwarding_active && userForwardings[i].is_busy==1) {
                char response[BUFFER_SIZE];
                sprintf(response, "Call from %s is forwarded to %s.\n", caller, userForwardings[i].destination_number);
                send(client_socket, response, BUFFER_SIZE, 0);
                userForwardings[i].is_busy = 1;  // Reset busy status after call handling
                pthread_mutex_unlock(&user_mutex);
                return;
            } else {
                send(client_socket, "Call connected normally.\n", BUFFER_SIZE, 0);
                userForwardings[i].is_busy = 1;  // Reset busy status after call handling
                pthread_mutex_unlock(&user_mutex);
                return;
            }
        }
    }
    send(client_socket, "Callee not found.\n", BUFFER_SIZE, 0);
    pthread_mutex_unlock(&user_mutex);
}

void *clientHandler(void *socket_desc) {
    int client_socket = *(int *)socket_desc;
    char buffer[BUFFER_SIZE];
    int valread;
 
    while ((valread = read(client_socket, buffer, BUFFER_SIZE)) > 0) {
        buffer[valread] = '\0';
        char command[20], username[50], password[100], forwardingType[20], destination[20], callee[50];
 
        // Command parsing with more flexibility
        if (sscanf(buffer, "%19s", command) == 1) {
            if (strcmp(command, "REGISTER") == 0 && sscanf(buffer, "%*s %49s %99s", username, password) == 2) {
                registerUser(username, password, client_socket);
            } else if (strcmp(command, "LOGIN") == 0 && sscanf(buffer, "%*s %49s %99s", username, password) == 2) {
                authenticateUser(username, password, client_socket);
            } else if (strcmp(command, "ACTIVATE") == 0 && sscanf(buffer, "%*s %49s %19s %19s", username, forwardingType, destination) == 3) {
                activateCallForwarding(username, forwardingType, destination, client_socket);
            } else if (strcmp(command, "DEACTIVATE") == 0 && sscanf(buffer, "%*s %49s", username) == 1) {
                deactivateCallForwarding(username, client_socket);
            } else if (strcmp(command, "CALL") == 0 && sscanf(buffer, "%*s %49s %49s", username, callee) == 2) {
                handleCall(username, callee, client_socket);
            } else {
                send(client_socket, "Invalid command or parameters.\n", BUFFER_SIZE, 0);
            }
        }
    }
 
    close(client_socket);
    free(socket_desc);
    return NULL;
}
int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t thread_id;
   
  //  printf("Server listening on port %d\n", PORT);
   // loadUsersFromFile();
   // loadForwardingsFromFile();
    
   // printf("Server listening on port1 %d\n", PORT);
    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

   // printf("Server listening on port2 %d\n", PORT);
    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
   // printf("Server listening on port3%d\n", PORT);
    // Start listening for connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);
    loadUsersFromFile();
	loadForwardingsFromFile();
//	printf("fggr");
    while (1) {
        // Accept a new connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Connection accepted\n");

        // Create a new thread for the client
        int *new_sock = malloc(1);
        *new_sock = client_socket;
        if (pthread_create(&thread_id, NULL, clientHandler, (void *)new_sock) < 0) {
            perror("Could not create thread");
            free(new_sock);
            continue;
        }

        // Detach the thread
        pthread_detach(thread_id);
    }

    // Cleanup
    close(server_socket);
    return 0;
}

