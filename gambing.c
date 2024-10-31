#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_PLAYERS 100

typedef struct {
    char name[50];
    int plays;
    double balance;
    double minBalance;
    double maxBalance;
} Player;

void addData(FILE *file, int result1, int result2, int result3, double balance);
void Gamehistory(FILE *file);
void readFile(const char *filename, double *minBalance, double *maxBalance, int *count);
double calculateExpectedValue();
void playGame(float *balance, FILE *file, Player *player);
void spinAnimation(int *result1, int *result2, int *result3);
int readBalanceFromBottom(const char *filename);
void updateLeaderboard(Player *players, int *playerCount, Player currentPlayer);
void displayLeaderboard(Player *players, int playerCount);
void loadLeaderboard(Player *players, int *playerCount); // New function to load existing players
void listFiles(); // Function to list all save files

int main() {
    float balance = 0.0;
    int choice;
    double ev = calculateExpectedValue();
    char playerName[50];
    char filename[100];
    Player players[MAX_PLAYERS] = {0};
    int playerCount = 0;

    loadLeaderboard(players, &playerCount);

    printf("===================================\n");
    printf("    Welcome to the Slot Machine Game!\n");
    printf("     Try your luck and win big!\n");
    printf("===================================\n");

    printf("1. Start New Game\n");
    printf("2. Load Existing Game\n");
    printf("Choose an option: ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Enter your name: ");
        scanf("%s", playerName);

        sprintf(filename, "save_%s.txt", playerName);
        FILE *file = fopen(filename, "w+");

        if (file == NULL) {
            printf("Error opening file!\n");
            return 1;
        }

        balance = 50000;
        srand(time(0));
        printf("Expected loss per play (EV): %.3f\n", ev);

        Player currentPlayer;
        strcpy(currentPlayer.name, playerName);
        currentPlayer.plays = 0;
        currentPlayer.minBalance = balance;
        currentPlayer.maxBalance = balance;
        currentPlayer.balance = balance;

        while (balance > 0) {
            printf("\n--- Menu ---\n");
            printf("1. Play Slot Machine\n");
            printf("2. Game history\n");
            printf("3. Summary of results\n");
            printf("4. Leaderboard\n");
            printf("5. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    playGame(&balance, file, &currentPlayer);
                    break;
                case 2:
                    Gamehistory(file);
                    break;
                case 3:
                    readFile(filename, &currentPlayer.minBalance, &currentPlayer.maxBalance, &currentPlayer.plays);
                    printf("Number of plays: %d\n", currentPlayer.plays);
                    printf("Minimum balance: %.2f\n", currentPlayer.minBalance);
                    printf("Maximum balance: %.2f\n", currentPlayer.maxBalance);
                    break;
                case 4:
                    updateLeaderboard(players, &playerCount, currentPlayer);
                    displayLeaderboard(players, playerCount);
                    break;
                case 5:
                    printf("Thank you for playing! You ended with a balance of %.2f.\n", balance);
                    fclose(file);
                    return 0;
                default:
                    printf("Invalid choice! Please select again.\n");
            }
        }

        printf("You've run out of money. This is how gambling can lead to loss.\n");
        fclose(file);
    } 
        else if (choice == 2) {
            printf("Available save files:\n");
            listFiles();

            printf("Enter your name: ");
            scanf("%s", playerName);

            // ใช้ strcpy เพื่อสร้างชื่อไฟล์
            strcpy(filename, "save_");
            strcat(filename, playerName);
            strcat(filename, ".txt");

            printf("Loading existing game from file: %s\n", filename);

            FILE *file = fopen(filename, "r");
            if (file == NULL) {
                printf("No save file found for player: %s\n", playerName);
                return 1;
            }

            // โค้ดอื่นๆ ที่คุณต้องการทำต่อไป
        
        srand(time(0));
        printf("Loaded game for player: %s with balance: %.2f\n", playerName, balance);
        printf("Expected loss per play (EV): %.3f\n", ev);

        Player currentPlayer;
        strcpy(currentPlayer.name, playerName);
        currentPlayer.balance = balance;

        while (balance > 0) {
            printf("\n--- Menu ---\n");
            printf("1. Play Slot Machine\n");
            printf("2. Game history\n");
            printf("3. Summary of results\n");
            printf("4. Leaderboard\n");
            printf("5. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    playGame(&balance, file, &currentPlayer);
                    break;
                case 2:
                    Gamehistory(file);
                    break;
                case 3:
                    readFile(filename, &currentPlayer.minBalance, &currentPlayer.maxBalance, &currentPlayer.plays);
                    printf("Number of plays: %d\n", currentPlayer.plays);
                    printf("Minimum balance: %.2f\n", currentPlayer.minBalance);
                    printf("Maximum balance: %.2f\n", currentPlayer.maxBalance);
                    break;
                case 4:
                    updateLeaderboard(players, &playerCount, currentPlayer);
                    displayLeaderboard(players, playerCount);
                    break;
                case 5:
                    printf("Thank you for playing! You ended with a balance of %.2f.\n", balance);
                    fclose(file);
                    return 0;
                default:
                    printf("Invalid choice! Please select again.\n");
            }
        }

        printf("You've run out of money. This is how gambling can lead to loss.\n");
        fclose(file);
    } else {
        printf("Invalid choice! Exiting program.\n");
        return 1;
    }

    return 0;
}

void listFiles() {
    struct dirent *entry;
    DIR *dp = opendir("./");

    if (dp == NULL) {
        printf("Error opening directory.\n");
        return;
    }

    while ((entry = readdir(dp))) {
        if (strstr(entry->d_name, ".txt") != NULL) {
            printf("- %s\n", entry->d_name);
        }
    }
    closedir(dp);
}

void addData(FILE *file, int result1, int result2, int result3, double balance) {
    fseek(file, 0, SEEK_END);
    if (fprintf(file, "Result: [%d] [%d] [%d], Balance: %.2lf\n", result1, result2, result3, balance) < 0) {
        perror("Error writing to file");
    }
    fflush(file);
}

void Gamehistory(FILE *file) {
    char line[256];
    rewind(file);
    printf("Game history:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
}

void readFile(const char *filename, double *minBalance, double *maxBalance, int *count) {
    double balance; // Change float to double
    char line[256];

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file: %s\n", filename);
        return;
    }

    printf("Data from %s:\n", filename);

    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);

        if (strstr(line, "Result:") != NULL) {
            // Change %f to %lf in sscanf
            if (sscanf(line, "Result: [%*d] [%*d] [%*d], Balance: %lf", &balance) == 1) {
                if (balance < *minBalance) {
                    *minBalance = balance;
                }
                if (balance > *maxBalance) {
                    *maxBalance = balance;
                }
                (*count)++;
            } else {
                printf("Error parsing balance from line: %s", line);
            }
        }
    }

    fclose(file);
}


int readBalanceFromBottom(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file: %s\n", filename);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long pos = ftell(file);

    char line[256];
    while (pos > 0) {
        fseek(file, --pos, SEEK_SET);
        if (fgetc(file) == '\n') {
            fgets(line, sizeof(line), file);
            if (strstr(line, "Balance:") != NULL) {
                float balance;
                sscanf(line, "Result: [%*d] [%*d] [%*d], Balance: %f", &balance);
                fclose(file);
                return balance;
            }
        }
    }
    if (strstr(line, "Balance:") != NULL) {
    float balance;
    if (sscanf(line, "Result: [%*d] [%*d] [%*d], Balance: %f", &balance) == 1) {
        fclose(file);
        return balance;
    } else {
        printf("Error parsing balance from line: %s\n", line);
        }
    }

    fclose(file);
    return -1;
}

double calculateExpectedValue() {
    double win3Match = 0.001 * 10; //โอกาสชนะได้เงินคูณ10คือ 0.1%
    double loseBet = 0.999 * -1; //โอกาศโดนเจ้ามือรับประทาน99.9% เสีย 99.9สตางค์ต่อ1หมุน
    return win3Match + loseBet;
}

void playGame(float *balance, FILE *file, Player *player) {
    float bet;

    printf("Your Balance is %.2f\n", *balance);
    while (1) { // Loop to check for valid bet amount
        printf("Enter your bet amount: ");
        if (scanf("%f", &bet) != 1) {
            printf("Invalid input! Please enter a valid number.\n");
            while (getchar() != '\n'); // Clear invalid input
            continue; // Ask again
        }

        if (bet < 0) {
            printf("Bet cannot be negative! Please enter a valid bet.\n");
        } else if (bet >= *balance * 1 / 10) { // Minimum bet is 10% of balance
            break;
        } else {
            printf("Sorry! You have to place a bet of at least %.2f\n", *balance * 1 / 10);
        }
    }

    *balance -= bet;

    int result1, result2, result3;
    spinAnimation(&result1, &result2, &result3);

    // Example winning condition
    if (result1 == result2 && result2 == result3) {
        *balance += bet * 10; // Winning condition
        printf("Congratulations! You won! New balance: %.2f\n", *balance);
    } else {
        printf("Sorry, you lost. New balance: %.2f\n", *balance);
    }

    // Update player data
    player->plays++;
    if (*balance < player->minBalance) {
        player->minBalance = *balance;
    }
    if (*balance > player->maxBalance) {
        player->maxBalance = *balance;
    }

    // Save game data
    addData(file, result1, result2, result3, *balance);
}

void spinAnimation(int *result1, int *result2, int *result3) {
    printf("Spinning the slot machine...\n");

    for (int i = 0; i < 10; i++) { // หมุน10รอบทำอนิเมชั่น
        *result1 = rand() % 10;
        *result2 = rand() % 10;
        *result3 = rand() % 10;

        printf("\rSlot results: [%d] [%d] [%d]   ", *result1, *result2, *result3);
        fflush(stdout); // ใช้เพื่อบังคับให้ข้อมูลในบัฟเฟอร์ของ stdout แสดงผลทันที 
                        ////โดยปกติ ข้อความที่แสดงผลบนหน้าจอจะถูกเก็บในบัฟเฟอร์ชั่วคราวก่อนที่จะถูกแสดงผล
        usleep(100000); // ดีเลย์0.1
    }
    printf("\n");
}

void loadLeaderboard(Player *players, int *playerCount) {
    struct dirent *entry;
    DIR *dp = opendir("./");

    if (dp == NULL) {
        printf("Error opening directory.\n");
        return;
    }

    while ((entry = readdir(dp))) {
        if (strstr(entry->d_name, ".txt") != NULL) {
            // Load player data from save file
            Player player;
            strcpy(player.name, entry->d_name + 5); // Assuming files named "save_PlayerName.txt"
            player.plays = 0; // You might want to load actual play count from the file
            player.balance = readBalanceFromBottom(entry->d_name); // Read the balance from the file
            player.minBalance = player.balance; // Initialize min and max balance
            player.maxBalance = player.balance;

            // Add to players array
            players[*playerCount] = player;
            (*playerCount)++;
        }
    }
    closedir(dp);
}

void updateLeaderboard(Player *players, int *playerCount, Player currentPlayer) {
    // Check if the player already exists
    for (int i = 0; i < *playerCount; i++) {
        if (strcmp(players[i].name, currentPlayer.name) == 0) {
            // Player exists, update their data
            players[i].plays += currentPlayer.plays;
            players[i].balance = currentPlayer.balance;
            if (currentPlayer.minBalance < players[i].minBalance || players[i].minBalance == 0) {
                players[i].minBalance = currentPlayer.minBalance; // Set min if it's the first time
            }
            if (currentPlayer.maxBalance > players[i].maxBalance) {
                players[i].maxBalance = currentPlayer.maxBalance;
            }
            return; // Exit the function after updating
        }
    }

    // If the player does not exist, add them to the leaderboard
    players[*playerCount] = currentPlayer;
    players[*playerCount].minBalance = currentPlayer.balance; // Initialize minBalance
    players[*playerCount].maxBalance = currentPlayer.balance; // Initialize maxBalance
    (*playerCount)++;
}

int comparePlayers(const void *a, const void *b) {
    return strcmp(((Player *)a)->name, ((Player *)b)->name);
}

void displayLeaderboard(Player *players, int playerCount) {
    // Sort players by name
    qsort(players, playerCount, sizeof(Player), comparePlayers);

    printf("\n--- Leaderboard ---\n");
    for (int i = 0; i < playerCount; i++) {
        printf("Player %s: Plays: %d, Balance: %.2f, Min: %.2f, Max: %.2f\n",
               players[i].name, players[i].plays, players[i].balance, players[i].minBalance, players[i].maxBalance);
    }
}
