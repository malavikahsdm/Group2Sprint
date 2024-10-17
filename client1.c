#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 256

void handleError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void registerUser(int sockfd) {
    char username[50], password[100];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "REGISTER %s %s", username, password);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void loginUser(int sockfd) {
    char username[50], password[100];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "LOGIN %s %s", username, password);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void activateCallForwarding(int sockfd) {
    char username[50], forwardingType[20], destination[20];
    printf("Enter your username: ");
    scanf("%s", username);
    printf("Enter call forwarding type (e.g., 'Unconditional', 'busy', 'Unanswered'): ");
    scanf("%s", forwardingType);
    printf("Enter destination number: ");
    scanf("%s", destination);

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "ACTIVATE %s %s %s", username, forwardingType, destination);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void deactivateCallForwarding(int sockfd) {
    char username[50];
    printf("Enter your username: ");
    scanf("%s", username);

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "DEACTIVATE %s", username);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void makeCall(int sockfd) {
    char caller[50], callee[50];
    printf("Enter your username: ");
    scanf("%s", caller);
    printf("Enter callee username: ");
    scanf("%s", callee);

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "CALL %s %s", caller, callee);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}
void displayCallLog(sockfd){
  printf("call log\n");
  return;
}
void unRegister(sockfd){
	printf("unregister\n");
	return;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handleError("Socket creation failed");
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        handleError("Connection to server failed");
    }

    int choice;
    while (1) {
        printf("\n1. Register\n2. Login\n3. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        if (choice == 1) {
            registerUser(sockfd);
            // After registration, ask for login or logout
            char action[10];
            printf("Do you want to LOGIN or EXIT  or UNREGISTER? ");
            scanf("%s", action);

            if (strcmp(action, "LOGIN") == 0) {
                loginUser(sockfd);
                int subChoice;
                while (1) {
                    printf("\n1. Activate Call Forwarding\n2. Deactivate Call Forwarding\n3. Make a call\n4. Display call logs \n5. Logout\n6. Unregister\n");
                    printf("Choose an option: ");
                    scanf("%d", &subChoice);

                      
                        if(subChoice==1){
                            activateCallForwarding(sockfd);
                            break;}
                        else if(subChoice=2){
                            deactivateCallForwarding(sockfd);
                            break;}
						else if(subChoice==3){
							makeCall(sockfd);
							break;}
						else if(subChoice==4){
							displayCallLog(sockfd);
							break;}
						else if(subChoice==5){
							printf("Logging out..\n");
							break;
						}
						else if(subChoice==6){
							unRegister(sockfd);
							break;
							}
                        else{
                            printf("Invalid choice. Please try again.\n");
                    }
                   
                   }
            } else if(strcmp(action,"UNREGISTER")==0){
				unRegister(sockfd);
				
			}
			
			else  {
                printf("Exiting...\n");
                break;
            }
        } else if (choice == 2) {
            loginUser(sockfd);
            int subChoice;
            
                while (1) {
                    printf("\n1. Activate Call Forwarding\n2. Deactivate Call Forwarding\n3. Make a call\n4. Display call logs \n5. Logout\n6. Unregister\n");
                    printf("Choose an option: ");
                    scanf("%d", &subChoice);

                   
                        if(subChoice==1){
                            activateCallForwarding(sockfd);
                            break;}
                        else if(subChoice==2){
                            deactivateCallForwarding(sockfd);
                            break;}
				     	else if(subChoice==3){
							makeCall(sockfd);
							break;}
						else if(subChoice==4){
							displayCallLog(sockfd);
							break;}
						else if(subChoice==5){
							printf("Logging out..");
							break;
						}
						else if(subChoice==6){
							unRegister(sockfd);
							break;}
                        else{
                            printf("Invalid choice. Please try again.\n");
                    }
				
                }
             
			
           
 
        } else if (choice == 3) {
			close(sockfd);
            exit(EXIT_SUCCESS);
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }

    close(sockfd);
    return 0;
}

