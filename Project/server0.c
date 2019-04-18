#include<io.h>
#include<stdio.h>
#include<winsock2.h>
#include <sys/stat.h>
#pragma comment(lib,"ws2_32.lib")

#define SV_SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100
#define BACKLOG 5

struct Players {
    SOCKET socket;
    int gameNum;
};
struct Players players[1000];

struct Card {
    int value;
    char name[2];
    char* suit;
};
struct Map {
    char* suit;
    char* value;
};

HANDLE playerMutex;

int gameCount = 0;
int playersCount = 0;

SOCKET* getPlayers(int gameNum) {
    SOCKET* player_socket;
    player_socket = malloc(10);
    DWORD dwWaitResult;

    while (1) {
        int playersNum = 0;
        int i;
        dwWaitResult = WaitForSingleObject(playerMutex,INFINITE);
        for (i = 0; i < playersCount; i++) {
            if (players[i].gameNum == gameNum) {
                player_socket[playersNum++] = players[i].socket;
            }
        }
        ReleaseMutex(playerMutex);

        if (playersNum == 2) break;

	//for(int i = 0; i <= 300000000; i++) {
	//}
        Sleep(3000);
    }

    return player_socket;
}

struct Card* loadCards() {
    static struct Card cards[36];

    char *suits[] = {"chervi", "bubi", "piki"};
    int cnt = 0;
    int i,j;
    for (i = 0; i < 3; i++) {
        for (j = 6; j < 10; j++) {
            cards[cnt].value = j;
            cards[cnt].name[0] = j + '0';
            cards[cnt].suit = suits[i];
            cnt++;
        }
        cards[cnt].value = 10;
        cards[cnt].name[0] = '1';
        cards[cnt].name[1] = '0';
        cards[cnt].suit = suits[i];
        cnt++;
        cards[cnt].value = 11;
        cards[cnt].name[0] = 'B';
        cards[cnt].suit = suits[i];
        cnt++;
        cards[cnt].value = 12;
        cards[cnt].name[0] = 'D';
        cards[cnt].suit = suits[i];
        cnt++;
        cards[cnt].value = 13;
        cards[cnt].name[0] = 'K';
        cards[cnt].suit = suits[i];
        cnt++;
        cards[cnt].value = 14;
        cards[cnt].name[0] = 'T';
        cards[cnt].suit = suits[i];
        cnt++;
    }

    return cards;
}

char* getSuit(char* suit) {
    static struct Map suits[4];
    suits[0].suit = "chervi";
    suits[0].value = "\u2664";
    suits[1].suit = "bubi";
    suits[1].value = "\u2662";
    suits[2].suit = "piki";
    suits[2].value = "\u2660";

    int i;
    for (i = 0; i < 3; i++) {
        if (suits[i].suit == suit) {
            return suits[i].value;
        }
    }

    return "";
}

struct Card* mixCards(struct Card* cards) {
    srand(time(NULL));

    int i;
    for (i = 0; i < 10; i++) {
        int l = rand() % 27;
        int r = rand() % 27;

        struct Card card = *(cards + l);
        *(cards + l) = *(cards + r);
        *(cards + r) = card;
    }

    return cards;
}

char* cardToString(struct Card card) {
    char* str;
    str = malloc(1000);

    strcpy(str, "");
    strcat(str, card.name);
    strcat(str, getSuit(card.suit));

    return str;
}

struct Card* updatePlayersCards(struct Card *cards, int pos) {
    struct Card *updatedCards;
    updatedCards = malloc(20);

    int i;
    for (i = 0; i < pos; i++) {
        updatedCards[i] = cards[i];
    }

    for (i = pos; i < 2; i++) {
        updatedCards[i] = cards[i+1];
    }

    return updatedCards;
}

int whoWins(struct Card player1_card, struct Card player2_card, int nowGoes, char* trump) {
    if (player1_card.value > player2_card.value && player1_card.suit == player2_card.suit){
        return 1;
    } else
    if (player1_card.value < player2_card.value && player1_card.suit == player2_card.suit){
        return 2;
    } else
    if (player1_card.suit != player2_card.suit && player1_card.suit == trump) {
        return 1;
    } else
    if (player1_card.suit != player2_card.suit && player2_card.suit == trump) {
        return 2;
    } else
    if (player1_card.suit != player2_card.suit && player2_card.suit != trump && nowGoes == 1) {
        return 1;
    } else
    if (player1_card.suit != player2_card.suit && player1_card.suit != trump && nowGoes == 2) {
        return 2;
    }

    return 0;
}

DWORD WINAPI playGame (void *initialGameNum) {
    int gameNum = *(int*)initialGameNum;
    SOCKET *player_socket = getPlayers(gameNum);
    char message[2000], client_message[2000];
    struct Card *player1_cards, *player2_cards;
    struct Card player1_move, player2_move;
    struct Card *cards;
    char* trump;
    int nowGoes = 1, movesCount = 0;
    int player1_score = 0, player2_score = 0;

    player1_cards = malloc(1000);
    player2_cards = malloc(1000);

    printf("Game %d has started \n", gameNum);
    int i;
    for (i = 0; i < 2; i++ ) {
        write(*(player_socket + i), "Game started", 20);
    }

    cards = loadCards();
    cards = mixCards(cards);

    for (i = 0; i < 3; i++) {
        player1_cards[i] = *(cards + i);
        player2_cards[i] = *(cards + (3 + i));
    }

    // Cast players' cards to String to return
    char* player1_cards_string = malloc(1000);
    strcpy(player1_cards_string, "");
    char* player2_cards_string = malloc(1000);
    strcpy(player2_cards_string, "");
    for (i = 0; i < 3; i++) {
        strcat(player1_cards_string, cardToString(player1_cards[i]));
        strcat(player1_cards_string, " ");
        strcat(player2_cards_string, cardToString(player2_cards[i]));
        strcat(player2_cards_string, " ");
    }

    //Add trump
    trump = (*(cards + 26)).suit;
    strcat(player1_cards_string, "      K:");
    strcat(player1_cards_string, cardToString(cards[26]));
    strcat(player2_cards_string, "      K:");
    strcat(player2_cards_string, cardToString(cards[26]));

    // Return cards to players
    send(*(player_socket + 0), player1_cards_string, strlen(player1_cards_string), 0);
    Sleep(1000);
    send(*(player_socket + 1), player2_cards_string, strlen(player2_cards_string), 0);

    // Return move order
    send(*(player_socket + 0), "1", 2, 0);
    Sleep(1000);
    send(*(player_socket + 1), "2", 2, 0);

    char* str = malloc(1000);
    strcpy(str, "");
    int cnt = 0;
    while(1) {
        memset(client_message, 0 ,255);
        // nowGoes show whose move is now
        if (nowGoes == 1) {
            if(recv(*(player_socket + 0), client_message , 2000 , 0) < 0) {
                puts("recv failed");
            } else {
                // printf("1 player move\n");
                int x = atoi(client_message) - 1;
                if (cnt == 1) {
                    strcat(str, "x");
                }
                strcat(str, cardToString(player1_cards[x]));
                player1_move = player1_cards[x];

                player1_cards = updatePlayersCards(player1_cards, x);
                player1_cards_string = malloc(1000);
                strcpy(player1_cards_string, "");
                for (i = 0; i < 2 - movesCount; i++) {
                    strcat(player1_cards_string, cardToString(player1_cards[i]));
                    strcat(player1_cards_string, " ");
                }
                strcat(player1_cards_string, "      K:");
                strcat(player1_cards_string, cardToString(cards[26]));

                Sleep(1000);
                send(player_socket[0], player1_cards_string, strlen(player1_cards_string), 0);
                send(player_socket[1], player2_cards_string, strlen(player2_cards_string), 0);

                Sleep(1000);
                send(player_socket[0], str, strlen(str), 0);
                send(player_socket[1], str, strlen(str), 0);

                nowGoes = 2;
                cnt++;
            }
        } else {
            if(recv(*(player_socket + 1), client_message , 2000 , 0) < 0) {
                puts("recv failed");
            } else {
                // printf("2 player move\n");
                int x = atoi(client_message) - 1;
                if (cnt == 1) {
                    strcat(str, "x");
                }
                strcat(str, cardToString(player2_cards[x]));
                player2_move = player2_cards[x];

                printf("%s\n", str);

                player2_cards = updatePlayersCards(player2_cards, x);
                player2_cards_string = malloc(1000);
                strcpy(player2_cards_string, "");
                for (i = 0; i < 2 - movesCount; i++) {
                    strcat(player2_cards_string, cardToString(player2_cards[i]));
                    strcat(player2_cards_string, " ");
                }
                strcat(player2_cards_string, "      K:");
                strcat(player2_cards_string, cardToString(cards[26]));

                Sleep(1000);
                send(player_socket[0], player1_cards_string, strlen(player1_cards_string), 0);
                send(player_socket[1], player2_cards_string, strlen(player2_cards_string), 0);

                Sleep(1000);
                send(player_socket[0], str, strlen(str), 0);
                send(player_socket[1], str, strlen(str), 0);

                nowGoes = 1;
                cnt++;
            }
        }

        // Choose winner of the move
        if (cnt == 2) {
            if (whoWins(player1_move, player2_move, nowGoes, trump) == 1) {
                Sleep(1000);
                send (player_socket[0], "w", 4, 0);
                send (player_socket[1], "l", 4, 0);

                player1_score++;
                nowGoes = 1;
            } else {
                Sleep(1000);
                send (player_socket[1], "w", 4, 0);
                send (player_socket[0], "l", 4, 0);

                player2_score++;
                nowGoes = 2;
            }
            cnt = 0;
            movesCount++;

            memset(str, 0, 20);
            strcpy(str, "");
        }

        // Choose winner of the game
        if (movesCount == 3) {
            if (player1_score > player2_score) {
                Sleep(1000);
                send (player_socket[0], "w", 4, 0);
                send (player_socket[1], "l", 4, 0);
            } else {
                Sleep(1000);
                send (player_socket[1], "w", 4, 0);
                send (player_socket[0], "l", 4, 0);
            }

           return 0;
        }

        if (nowGoes == 2) {
            Sleep(1000);
            send (player_socket[1], "1", 4, 0);
            send (player_socket[0], "2", 4, 0);
        } else {
            Sleep(1000);
            send (player_socket[0], "1", 4, 0);
            send (player_socket[1], "2", 4, 0);
        }
    }

    return 0;
}

DWORD WINAPI playerAction (void *socket_desc) {
    ssize_t numRead;
    char message[2000], client_message[2000];
    SOCKET cfd = *(SOCKET*)socket_desc;
    HANDLE gameThread;
    DWORD gameThreadID;
    DWORD dwWaitResult;

    // Get the client's choose of the playing mode
    if ((numRead = recv(cfd , client_message , 2000 , 0)) < 0) {
        puts("recv failed");
    }
    if (numRead == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else
    if (numRead == -1) {
        perror("recv failed");
    } else {
        // Check player mode choose
        if (*client_message == '1') {
            printf("Player choose to start new game\n" );
            // Add player to the game and start new thread for the game
            dwWaitResult = WaitForSingleObject(playerMutex, INFINITE);
    	    if (dwWaitResult == WAIT_OBJECT_0) {
                gameCount++;
                players[playersCount].socket = cfd;
                players[playersCount].gameNum = gameCount;
                playersCount++;
                int *num = malloc(sizeof(*num));
                *num = gameCount;
    		ReleaseMutex(playerMutex);
    		        	
                char arg = gameCount + '0';
                char *gNum = malloc(sizeof(char));
                *gNum = arg;
                send(cfd , gNum, strlen(gNum), 0);

                if ((gameThread = CreateThread(NULL, 0, playGame, num, 0, &gameThreadID)) == NULL) {
                    fprintf(stderr, "Error creating newCustomerThread thread\n");
                }
    	     }
        } else {
            printf("Player choose to join the game\n" );
                  	
	    send(cfd , "ok", 2, 0);

            if(recv(cfd , client_message , 2000 , 0) < 0) {
                puts("recv failed");
            } else {
                // Add player to the game
                while(1) {
                    dwWaitResult = WaitForSingleObject(playerMutex, INFINITE);
                    if (dwWaitResult == WAIT_OBJECT_0) {
                        players[playersCount].socket = cfd;
                        players[playersCount].gameNum = atoi(client_message);
                        playersCount++;
                        ReleaseMutex(playerMutex);

                        break;
                    }
                }
            }
        }
    }

    WaitForSingleObject(gameThread, gameThreadID);
    CloseHandle(gameThread);
    	
    //printf("Hello");
    return 0;
}

int main(int argc, char *argv[])
{
    HANDLE playerThread[1000];
    DWORD playerThreadID[1000];
    WSADATA wsa;
    struct sockaddr_in addr;
    SOCKET sfd, cfd, c;
    char buf[BUF_SIZE];

    playerMutex = CreateMutex(NULL, FALSE, NULL);

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == INVALID_SOCKET) {
        fprintf(stderr, "Socket error: %s\n", strerror(errno));
        exit(0);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "Bind error: %s\n", SV_SOCK_PATH);
        exit(0);
    }

    if (listen(sfd, BACKLOG) == -1) {
        fprintf(stderr, "Listen error: %s\n", SV_SOCK_PATH);
        exit(0);
    }

    // Listen to all the connections of the clients
    int cnt = 0;
    c = sizeof(struct sockaddr_in);
    while((cfd = accept(sfd, (struct sockaddr *)&addr, &c)))
    {
        printf("Player added\n");

        if (cfd == -1) {
            fprintf(stderr, "Accept error: %s\n", SV_SOCK_PATH);
            exit(0);
        } else {
            SOCKET *new_sock;
            new_sock = malloc(1);
            *new_sock = cfd;

            // Start a new thread for player
            if ((playerThread[cnt] = CreateThread(NULL, 0, playerAction, (void*)new_sock, 0, &playerThreadID[cnt])) == NULL) {
                fprintf(stderr, "Error creating newCustomerThread thread\n");
                return 1;
            } else { cnt++; }
        }
    }

    WaitForMultipleObjects(cnt, playerThread, TRUE, INFINITE);
    for(int i = 0; i < cnt; i++)
    {
        CloseHandle(playerThread[i]);
    }
}
