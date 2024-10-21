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
	while(1){
    printf("Enter phone_no.: +91");
    scanf("%s", phone_no);
    if(strlen(phone_no)!=10){
		printf("Enter valid phone no.\n");
	}
	else{
		break;
	}
	}
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
    printf("Enter phone_no: +91");
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
    char username[50],forwardingType[20],  phone_no[11], destination[11];
	int choice=0;
    printf("Enter your username: ");
    scanf("%s", username);
    printf(" call forwarding type\n1. Unconditional\n2. Busy\n3. Unanswered\n ");
	printf("Enter your choice: ");
    scanf("%d", &choice);
    
	if(choice==1){
	strcpy(forwardingType, "Unconditional");
	forwardingType[strlen(forwardingType)]='\0';

	}

	else if(choice==2){
	strcpy(forwardingType, "Busy");
	forwardingType[strlen(forwardingType)]='\0';
	
	}


	else if(choice==3){
	
	strcpy(forwardingType, "Unanswered");
	forwardingType[strlen(forwardingType)]='\0';
	}


	else{
	printf("Invalid choice. \n");
	}


    printf("Enter your phone_no: +91");
    scanf("%s",phone_no);
	 while(1){
        printf("Enter destination number: +91");
 		scanf("%s", destination);
      if(strlen(destination)!=10){
         printf("Enter valid destination number\n");
      }
      else{
         break;
      }
     }
    
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
    printf("Enter your phone_no: +91");
    scanf("%s", phone_no);
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "DEACTIVATE %s %s", username, phone_no);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}

void makeCall(int sockfd) {
    char YourPhoneNo[50], callee[20], phone_no[11];
	while(1){
     printf("Enter YourPhoneNo: +91");
     scanf("%s", YourPhoneNo);
	 if(strlen(YourPhoneNo)!=10){
	 	printf("Entered valid number\n");
	 }
	 else{
	 	break;
	 }
	}
    printf("Enter callee username: ");
    scanf("%s", callee);
    printf("Enter callee phone_no: +91");
    scanf("%s",phone_no);
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "CALL %s %s %s", YourPhoneNo, callee, phone_no);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);
}


void displayCallLog(int sockfd){

       char caller[11];
        printf("Enter Your Phone No: +91");
        scanf("%s", caller);
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "CALLLOG %s ", caller);
        send(sockfd, buffer, strlen(buffer), 0);
        recv(sockfd, buffer, BUFFER_SIZE, 0);
		printf("%s\n", buffer);
        
}


void unregisterUser(int sockfd){
   char phone_no[11], password[100];
 
    printf("\nEnter phone_no: +91");
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
	printf("Enter phone no: +91");
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
            // After registration, ask for login or exit or unregister.
            int action=0;
            printf("Do you want to:\n1. Login\n2. Unregister\n3. Exit\n");
            scanf("%d", &action);

            if (action==1) {
                loginUser(sockfd);
                int subChoice;
                while (1) {
                    printf("\n1. Activate Call Forwarding\n2. Deactivate Call Forwarding\n3. Make a call\n4. Display call logs \n5. change password\n6. Logout\n7. Unregistered\n");
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
            } else if(action==2){
				unregisterUser(sockfd);
				break;
				
			}
			
			else if(action==3){
                printf("Exiting...\n");
                break;
            }
			else{
			  printf("Invalid choice entered\n");
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
