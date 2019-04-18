#include<io.h>
#include<stdio.h>
#include<winsock2.h>
#include <sys/stat.h>
#pragma comment(lib,"ws2_32.lib")

#define SV_SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100
#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))

void startGame (SOCKET sfd) {
    char message[1000] , server_reply[2000];
    char str[1000], card[2000];

    printf("Waiting for game start...\n");

    //Wait for game started message
    memset(server_reply, 0, 255);
    if(recv(sfd , server_reply , 2000 , 0) < 0) {
        puts("recv failed");
    }
    printf("%s\n", server_reply);

    // Wait for cards
    memset(server_reply, 0, 255);
    if(recv(sfd , server_reply , 2000 , 0) < 0) {
        puts("recv failed");
    }
    printf("%s\n", server_reply);

    // Wait for moves
    memset(str, 0, 10);
    if(recv(sfd , str , 2000 , 0) < 0) {
        puts("recv failed");
    }
    if (*str == '1') {
        printf("You move first \n");
    } else {
        printf("Your opponent move first \n");
    }

    int cnt = 0;
    while(1){
        cnt++;

        if (*str == '1') {
            printf("Your move: ");
            scanf("%s" , message);

            if(send(sfd , message , strlen(message) , 0) < 0) {
                puts("Send failed");
            }
        } else {
            printf("Opponent's move\n");
        }

        // Get move's cards
        memset(server_reply, 0, 255);
        if(recv(sfd , server_reply , 2000 , 0) < 0) {
            puts("recv failed");
        } else {
            printf("%s\n", server_reply);
        }

        // Get player's current cards
        memset(card, 0, 255);
        if(recv(sfd , card , 2000 , 0) < 0) {
            puts("recv failed");
        } else {
            printf("%s\n", card);
        }

        // Get the winner of the move
        if (cnt % 2 == 0) {
            memset(server_reply, 0, 255);
            if(recv(sfd , server_reply , 2000 , 0) < 0) {
                puts("recv failed");
            } else {
                if (*server_reply == 'w') {
                    printf("You win the move\n");
                } else {
                    printf("You lose the move\n");
                }
            }
        }

        // Get the winner of the game
        if (cnt == 6) {
            if(recv(sfd , server_reply , 2000 , 0) < 0) {
                puts("recv failed");
            } else {
                printf("Game over\n");
                if (*server_reply == 'w') {
                    printf("You win\n");
                    return;
                } else {
                    printf("You lose\n");
                    return;
                }
            }
        }

        // Get moves order
        memset(str, 0, 255);
        if(recv(sfd , str , 2000 , 0) < 0) {
            puts("recv failed");
        }
    }
}

int main(int argc, char *argv[])
{
    SOCKET sfd, cfd;
    WSADATA wsa;
    ssize_t numRead;
    struct sockaddr_in addr;
    char buf[BUF_SIZE];
    char message[1000] , server_reply[2000];

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    if((sfd = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
         printf("Could not create socket : %d" , WSAGetLastError());
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));

    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);

    // if(inet_pton(AF_INET, argv[1], &addr.sin_addr)<=0)
    // {
    //    printf("\n inet_pton error occured\n");
    //    return 1;
    // }

    if (connect(sfd, (struct sockaddr *)&addr , sizeof(addr)) < 0)
    {
        puts("connect error");
        return 1;
    }

    // Choose to start a new game or join
    while(1)
    {
        printf("1. Start new game \n");
        printf("2. Join the game \n");
        printf("Your choice: ");
        scanf("%s" , message);
        //Send choose
        if(send(sfd , message , strlen(message) , 0) < 0) {
            puts("Send failed");
            return 1;
        }

        if (*message == '1') {
            if(recv(sfd , server_reply , 2000 , 0) < 0) {
                puts("recv failed");
                break;
            } else {
		recv(sfd, server_reply, 2000, 0);	
                printf("Your game number: %s\n", server_reply);

                startGame(sfd);
            }
        } else {
            if(recv(sfd , server_reply , 2000 , 0) < 0) {
                puts("recv failed");
                break;
            } else {
                printf("Enter the game number: ");
                scanf("%s" , message);

                if(send(sfd, message , strlen(message) , 0) < 0) {
                    puts("Send failed");
                    return 1;
                } else {
                    startGame(sfd);
                }
            }
        }
    }

    exit(EXIT_SUCCESS);
}
