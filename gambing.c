#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    char playerName[50];
    float maxBalance;
} Player;

// Function declarations
void addData(FILE *file, const char *playerName, int result1, int result2, int result3, float balance);
void saveMinMax(FILE *file, const char *playerName, float minBalance, float maxBalance);
void Gamehistory(FILE *file, const char *playerName);
void readFile(const char *filename, const char *playerName, float *minBalance, float *maxBalance, int *count);
double calculateExpectedValue();
void playGame(float *balance, FILE *file, const char *playerName);
void spinAnimation(int *result1, int *result2, int *result3);
float loadPlayerBalance(FILE *file, const char *playerName);
void displayLeaderboard(const char *filename);

int main() {
    float balance = 0.0;
    int choice;
    double ev = calculateExpectedValue();
    char playerName[50];
    const char *filename = "savefile.txt";

    printf("===================================\n");
    printf("    Welcome to the Slot Machine Game!\n");
    printf("     Try your luck and win big!\n");
    printf("===================================\n");

    printf("1. Start New Game\n");
    printf("2. Load Existing Game\n");
    printf("Choose an option: ");
    scanf("%d", &choice);

    FILE *file = fopen(filename, "a+"); // Open the file for appending and reading
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    if (choice == 1) {
        // Start a new game
        printf("Enter your name to start a new game: ");
        scanf("%s", playerName);
        balance = 50000; // Set initial balance for a new game
        printf("Expected loss per play (EV): %.3f\n", ev);

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
                    playGame(&balance, file, playerName);
                    break;
                case 2:
                    Gamehistory(file, playerName);
                    break;
                case 3: {
                    float minBalance = 50000.0, maxBalance = 50000.0;
                    int count = 0;
                    readFile(filename, playerName, &minBalance, &maxBalance, &count);
                    saveMinMax(file, playerName, minBalance, maxBalance);
                    printf("Number of plays: %d\n", count);
                    printf("Minimum balance: %.2f\n", minBalance);
                    printf("Maximum balance: %.2f\n", maxBalance);
                    break;
                }
                case 4:
                    displayLeaderboard(filename);
                    break;
                case 5:
                    printf("Thank you for playing, %s! You ended with a balance of %.2f.\n", playerName, balance);
                    fclose(file);
                    return 0;
                default:
                    printf("Invalid choice! Please select again.\n");
            }
        }

        printf("You've run out of money. This is how gambling can lead to loss.\n");

    } else if (choice == 2) {
        // Load an existing game
        printf("Enter your name to load your game: ");
        scanf("%s", playerName);

        balance = loadPlayerBalance(file, playerName);
        if (balance == -1) {
            printf("No save data found for player: %s\n", playerName);
            fclose(file);
            return 1;
        }

        printf("Loaded game for %s with balance: %.2f\n", playerName, balance);
        printf("Expected loss per play (EV): %.3f\n", ev);

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
                    playGame(&balance, file, playerName);
                    break;
                case 2:
                    Gamehistory(file, playerName);
                    break;
                case 3: {
                    float minBalance = balance, maxBalance = balance;
                    int count = 0;
                    readFile(filename, playerName, &minBalance, &maxBalance, &count);
                    saveMinMax(file, playerName, minBalance, maxBalance);
                    printf("Number of plays: %d\n", count);
                    printf("Minimum balance: %.2f\n", minBalance);
                    printf("Maximum balance: %.2f\n", maxBalance);
                    break;
                }
                case 4:
                    displayLeaderboard(filename);
                    break;
                case 5:
                    printf("Thank you for playing, %s! You ended with a balance of %.2f.\n", playerName, balance);
                    fclose(file);
                    return 0;
                default:
                    printf("Invalid choice! Please select again.\n");
            }
        }

        printf("You've run out of money. This is how gambling can lead to loss.\n");
    } else {
        printf("Invalid choice! Exiting program.\n");
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}

void displayLeaderboard(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for leaderboard!\n");
        return;
    }

    Player players[100];
    int count = 0;
    char line[256];

    // Parse player entries to find max balances
    while (fgets(line, sizeof(line), file)) {
        char playerName[50];
        float balance;

        if (sscanf(line, "%[^:]: Result: [%*d] [%*d] [%*d], Balance: %f", playerName, &balance) == 2) {
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(players[i].playerName, playerName) == 0) {
                    if (balance > players[i].maxBalance) {
                        players[i].maxBalance = balance;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(players[count].playerName, playerName);
                players[count].maxBalance = balance;
                count++;
            }
        }
    }
    fclose(file);

    // Sort players by max balance in descending order
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (players[i].maxBalance < players[j].maxBalance) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    // Display leaderboard
    printf("\n--- Leaderboard ---\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s - Max Balance: %.2f\n", i + 1, players[i].playerName, players[i].maxBalance);
    }
}

void addData(FILE *file, const char *playerName, int result1, int result2, int result3, float balance) {
    fseek(file, 0, SEEK_END);
    fprintf(file, "%s: Result: [%d] [%d] [%d], Balance: %.2f\n", playerName, result1, result2, result3, balance);
    fflush(file);
}

void saveMinMax(FILE *file, const char *playerName, float minBalance, float maxBalance) {
    fseek(file, 0, SEEK_END);
    fprintf(file, "%s: Min Balance: %.2f, Max Balance: %.2f\n", playerName, minBalance, maxBalance);
    fflush(file);
}

void Gamehistory(FILE *file, const char *playerName) {
    char line[256];
    rewind(file);
    printf("Game history for %s:\n", playerName);
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, playerName) == line && !strstr(line, "Min Balance") && !strstr(line, "Max Balance")) { // Check if line starts with playerName and exclude min/max
            printf("%s", line);
        }
    }
}

void readFile(const char *filename, const char *playerName, float *minBalance, float *maxBalance, int *count) {
    FILE *file = fopen(filename, "r");
    char line[256];
    float balance;

    if (file == NULL) {
        printf("Could not open file: %s\n", filename);
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, playerName) == line) {
            if (sscanf(line, "%*[^:]: Result: [%*d] [%*d] [%*d], Balance: %f", &balance) == 1) {
                if (balance < *minBalance) *minBalance = balance;
                if (balance > *maxBalance) *maxBalance = balance;
                (*count)++;
            }
        }
    }

    fclose(file);
}

float loadPlayerBalance(FILE *file, const char *playerName) {
    char line[256];
    float balance = -1;
    rewind(file);

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, playerName) == line) {
            sscanf(line, "%*[^:]: Result: [%*d] [%*d] [%*d], Balance: %f", &balance);
        }
    }

    return balance;
}

void playGame(float *balance, FILE *file, const char *playerName) {
    float bet;
    float minBet = *balance * 0.1; // Set minimum bet to 10% of the balance

    do {
        printf("Enter your bet amount (minimum 10%% of balance: %.2f): ", minBet);
        scanf("%f", &bet);

        if (bet < minBet) {
            printf("The bet amount is too low. Please enter at least 10%% of your balance.\n");
        } else if (bet > *balance) {
            printf("You don't have enough money. Current balance: %.2f\n", *balance);
            return;
        }
    } while (bet < minBet);

    *balance -= bet;

    int result1, result2, result3;
    spinAnimation(&result1, &result2, &result3);

    printf("Final Slot results: [%d] [%d] [%d]\n", result1, result2, result3);

    // New winning conditions
    if (result1 == result2 && result2 == result3) {
        printf("Jackpot! You won: %.2f\n", bet * 15); // Increase jackpot payout to 15x
        *balance += bet * 15;
    } else if (result1 == result2 || result2 == result3 || result1 == result3) {
        printf("You won: %.2f for matching two symbols!\n", bet * 2); // Win for two matching symbols
        *balance += bet * 2;
    } else {
        printf("You lost your bet of: %.2f\n", bet);
    }

    printf("Your remaining balance: %.2f\n", *balance);
    addData(file, playerName, result1, result2, result3, *balance);
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

double calculateExpectedValue() {
    double win3Match = 0.001 * 15; // Adjusted probability for winning 10 times is now 0.1%
    double win2Match = 0.05 * 2;    // 5% chance of matching two symbols and winning double
    double loseBet = 0.949 * -1;    // Adjusted probability of losing

    return win3Match + win2Match + loseBet;
}