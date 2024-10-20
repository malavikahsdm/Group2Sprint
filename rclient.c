#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 256
int callLogCount=0;
void handleError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void registerUser(int sockfd) {
    char username[50], password[100], phone_no[11];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    printf("Enter phone_no.: ");
    scanf("%s", phone_no);

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "REGISTER %s %s %s", username, password, phone_no);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void loginUser(int sockfd) {
    char username[50], phone_no[11], password[100];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter phone_no: ");
    scanf("%s", phone_no);
    printf("Enter password: ");
    scanf("%s", password);

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "LOGIN %s %s %s", username, phone_no, password);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void activateCallForwarding(int sockfd) {
    char username[50], forwardingType[20], phone_no[11], destination[11];
    printf("Enter your username: ");
    scanf("%s", username);
    printf("Enter call forwarding type (e.g., 'Unconditional', 'busy', 'Unanswered'): ");
    scanf("%s", forwardingType);
    printf("Enter your phone_no: ");
    scanf("%s",phone_no);
    printf("Enter destination number: ");
    scanf("%s", destination);

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "ACTIVATE %s %s %s %s", username, forwardingType, phone_no, destination);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void deactivateCallForwarding(int sockfd) {
    char username[50], phone_no[11];
    printf("Enter your username: ");
    scanf("%s", username);
    printf("Enter your phone_no: ");
    scanf("%s", phone_no);
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "DEACTIVATE %s %s", username, phone_no);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void makeCall(int sockfd) {
    char YourPhoneNo[50], callee[20], phone_no[11];
    printf("Enter YourPhoneNo: ");
    scanf("%s", YourPhoneNo);
    printf("Enter callee username: ");
    scanf("%s", callee);
    printf("Enter callee phone_no: ");
    scanf("%s",phone_no);
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "CALL %s %s %s", YourPhoneNo, callee, phone_no);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}


void displayCallLog(int sockfd){

       char caller[11];
        printf("Enter Your Phone No: ");
        scanf("%s", caller);
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "CALLLOG %s ", caller);
        send(sockfd, buffer, strlen(buffer), 0);
        recv(sockfd, buffer, BUFFER_SIZE, 0);
		printf("%s\n", buffer);
        
}


void unregisterUser(int sockfd){
   char phone_no[11], password[100];
 
    printf("\nEnter phone_no: +91 ");
    scanf("%s", phone_no);
    printf("\nEnter password: ");
    scanf("%s", password);
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "UNREGISTER %s %s", phone_no, password);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
    //printf("\nUnregistered successfully\n");
}



void changePassword(int sockfd){
	char phone_no[11], password[100], new_password[100];
	printf("Enter phone no: ");
	scanf("%s", phone_no);
	printf("Enter old password: ");
	scanf("%s",password);
	printf("Enter new password: ");
	scanf("%s", new_password);
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "CHANGE_PASSWORD %s %s %s", phone_no, password, new_password);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);

}

int main() {
    int sockfd = -1;
    struct sockaddr_in server_addr;
    int choice=0;
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

    while (1) {
        printf("\n1. Register\n2. Login\n3. Make a call\n4. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        if (choice == 1) {
            registerUser(sockfd);
            // After registration, ask for login or logout
            char action[10];
            printf("Do you want to:\n1. LOGIN\n2. EXIT\n3. UNREGISTER");
            scanf("%s", action);

            if (strcmp(action, "LOGIN") == 0) {
                loginUser(sockfd);
                int subChoice;
                while (1) {
                    printf("\n1. Activate Call Forwarding\n2. Deactivate Call Forwarding\n3. Make a call\n4. Display call logs \n5. change password\n6. Logout\n7. Unregistered");
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
						    changePassword(sockfd);
							break;
						}

						else if(subChoice==6){
							printf("Logging out..\n");
							break;
						}
						else if(subChoice==7){
							unregisterUser(sockfd);
							break;
							}
                        else{
                            printf("Invalid choice. Please try again.\n");
                    }
                   
                   }
            } else if(strcmp(action,"UNREGISTER")==0){
				unregisterUser(sockfd);
				
			}
			
			else  {
                printf("Exiting...\n");
                break;
            }
        } else if (choice == 2) {
            loginUser(sockfd);
            int subChoice;
            
                while (1) {
                    printf("\n1. Activate Call Forwarding\n2. Deactivate Call Forwarding\n3. Make a call\n4. Display call logs \n5. change password\n6. Logout\n7. Unregister\n");
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
						    changePassword(sockfd);
							break;
						}
						else if(subChoice==5){
							printf("Logging out..");
							break;
						}
						else if(subChoice==6){
							unregisterUser(sockfd);
							break;}
                        else{
                            printf("Invalid choice. Please try again.\n");
                    }
				
                }
             
			
           
 
        }else if(choice == 3){
			makeCall(sockfd);
		}else if (choice == 4) {
			close(sockfd);
            exit(EXIT_SUCCESS);
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }

    close(sockfd);
    return 0;
}
