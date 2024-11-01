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
double calculateBlackjackEV(int wins, int losses);
void playGame(float *balance, FILE *file, const char *playerName);
void playBlackjack(float *balance, FILE *file, const char *playerName);
void spinAnimation(int *result1, int *result2, int *result3);
float loadPlayerBalance(FILE *file, const char *playerName);
void displayLeaderboard(const char *filename);
#define MAX_CARDS 52  // Example value; adjust as needed
#define BLACKJACK 21  // The value for a Blackjack hand
#define DEALER_HIT 17  // Example threshold; adjust based on your game rules


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
        printf("Expected loss per play (EV) in Slot Machine: %.3f\n", ev);

        while (balance > 0) {
            printf("\n--- Menu ---\n");
            printf("1. Play Slot Machine\n");
            printf("2. Play Blackjack\n");
            printf("3. Game history\n");
            printf("4. Summary of results\n");
            printf("5. Leaderboard\n");
            printf("6. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    playGame(&balance, file, playerName);
                    break;
                case 2:
                    playBlackjack(&balance, file, playerName);
                    break;
                case 3:
                    Gamehistory(file, playerName);
                    break;
                case 4: {
                    float minBalance = 50000.0, maxBalance = 50000.0;
                    int count = 0;
                    readFile(filename, playerName, &minBalance, &maxBalance, &count);
                    saveMinMax(file, playerName, minBalance, maxBalance);
                    printf("Number of plays: %d\n", count);
                    printf("Minimum balance: %.2f\n", minBalance);
                    printf("Maximum balance: %.2f\n", maxBalance);
                    break;
                }
                case 5:
                    displayLeaderboard(filename);
                    break;
                case 6:
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
        printf("Expected loss per play (EV) in Slot Machine: %.3f\n", ev);

        while (balance > 0) {
            printf("\n--- Menu ---\n");
            printf("1. Play Slot Machine\n");
            printf("2. Play Blackjack\n");
            printf("3. Game history\n");
            printf("4. Summary of results\n");
            printf("5. Leaderboard\n");
            printf("6. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    playGame(&balance, file, playerName);
                    break;
                case 2:
                    playBlackjack(&balance, file, playerName);
                    break;
                case 3:
                    Gamehistory(file, playerName);
                    break;
                case 4: {
                    float minBalance = balance, maxBalance = balance;
                    int count = 0;
                    readFile(filename, playerName, &minBalance, &maxBalance, &count);
                    saveMinMax(file, playerName, minBalance, maxBalance);
                    printf("Number of plays: %d\n", count);
                    printf("Minimum balance: %.2f\n", minBalance);
                    printf("Maximum balance: %.2f\n", maxBalance);
                    break;
                }
                case 5:
                    displayLeaderboard(filename);
                    break;
                case 6:
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

void shuffleDeck(int *deck) {
    for (int i = 0; i < MAX_CARDS; i++) {
        deck[i] = i % 13 + 1; // Assign values 1-13 for cards
    }
    for (int i = 0; i < MAX_CARDS; i++) {
        int r = rand() % MAX_CARDS;
        int temp = deck[i];
        deck[i] = deck[r];
        deck[r] = temp;
    }
}

void dealCard(int *deck, int *index, int *hand, int handSize) {
    hand[handSize] = deck[(*index)++];
}

int calculateHandValue(int *hand, int handSize) {
    int value = 0;
    int aces = 0;
    for (int i = 0; i < handSize; i++) {
        if (hand[i] > 10) {
            value += 10; // Face cards are worth 10
        } else if (hand[i] == 1) {
            aces++;
            value += 11; // Aces are worth 11 initially
        } else {
            value += hand[i];
        }
    }
    // Adjust for aces
    while (value > BLACKJACK && aces) {
        value -= 10;
        aces--;
    }
    return value;
}

void displayHands(int *playerHand, int playerHandSize, int *dealerHand, int dealerHandSize, int dealerShowHand) {
    printf("Dealer's Hand: ");
    for (int i = 0; i < dealerHandSize; i++) {
        if (i == 0 && !dealerShowHand) {
            printf("?? "); // Hide the first card of dealer
        } else {
            printf("%d ", dealerHand[i]);
        }
    }
    printf("\nPlayer's Hand: ");
    for (int i = 0; i < playerHandSize; i++) {
        printf("%d ", playerHand[i]);
    }
    printf("\n");
}

void playBlackjack(float *balance, FILE *file, const char *playerName) {
    int playerHand[10], dealerHand[10];
    int playerHandSize = 0, dealerHandSize = 0;
    int deck[MAX_CARDS];
    int index = 0;
    int wins = 0;  // To count player wins
    int losses = 0;  // To count player losses
    int dealerTough = 0;  // 0 = easy, 1 = tough
    float bet;
    float minBet = *balance * 0.1; // Minimum bet is 10% of balance
    char input[100];
    
    while (*balance > 0) {
        int playerHand[10], dealerHand[10];
        int playerHandSize = 0, dealerHandSize = 0;
        int deck[MAX_CARDS];
        int index = 0;
        float bet;
        float minBet = *balance * 0.1; // Minimum bet is 10% of balance

        while (1) {
            printf("Your balance: %.2f\n", *balance);
            printf("Enter your bet (minimum %.2f): ", minBet);
            scanf("%s", input);
            bet = atof(input);
            
            if (bet < minBet) {
                printf("Bet is too low. Minimum is %.2f\n", minBet);
            } 
            else if (bet > *balance) {
                printf("Insufficient balance.\n");
            } 
            else {
                *balance -= bet; // Deduct bet amount initially
                break; // Exit loop once a valid bet is entered
            }
        }
        
        shuffleDeck(deck);
        playerHandSize = dealerHandSize = 0; // Reset hands
        dealCard(deck, &index, playerHand, playerHandSize++);
        dealCard(deck, &index, dealerHand, dealerHandSize++);
        dealCard(deck, &index, playerHand, playerHandSize++);
        dealCard(deck, &index, dealerHand, dealerHandSize++);

        // Player's turn
        while (1) {
            printf("Your hand: ");
            for (int i = 0; i < playerHandSize; i++) {
                printf("%d ", playerHand[i]);
            }
            printf("(Value: %d)\n", calculateHandValue(playerHand, playerHandSize));
            printf("Dealer's visible card: %d\n", dealerHand[0]);
            printf("Balance: %.2f\n", *balance);
            
            char action;
            printf("Do you want to (H)it or (S)tand? ");
            scanf(" %c", &action);
            
            if (action == 'H' || action == 'h') {
                dealCard(deck, &index, playerHand, playerHandSize++);
                if (calculateHandValue(playerHand, playerHandSize) > BLACKJACK) {
                    printf("You busted! Your hand value exceeded 21.\n");
                    losses++;
                    break; // Player loses
                }
            } else {
                break; // Player stands
            }
        }

        // Dealer's turn
        if (calculateHandValue(playerHand, playerHandSize) <= BLACKJACK) {
            while (calculateHandValue(dealerHand, dealerHandSize) < (dealerTough ? DEALER_HIT + 3 : DEALER_HIT)) {
                dealCard(deck, &index, dealerHand, dealerHandSize++);
            }

            // Show dealer's hand
            printf("Dealer's hand: ");
            for (int i = 0; i < dealerHandSize; i++) {
                printf("%d ", dealerHand[i]);
            }
            printf("(Value: %d)\n", calculateHandValue(dealerHand, dealerHandSize));

            int playerValue = calculateHandValue(playerHand, playerHandSize);
            int dealerValue = calculateHandValue(dealerHand, dealerHandSize);

            if (dealerValue > BLACKJACK || playerValue > dealerValue) {
                printf("You win!\n");
                wins++;
                *balance += bet * 2; // Winning doubles the bet amount
            } else if (playerValue < dealerValue) {
                printf("You lose.\n");
                losses++;
            } else {
                printf("It's a tie!\n");
                *balance += bet; // Refund bet on tie
            }
        }

        // Check win/loss conditions and adjust dealer difficulty
        if (wins > 3) {
            dealerTough = 1; // Make dealer tougher
            printf("The dealer is getting tougher!\n");
        }

        if (losses == 6) {
            wins = 0; // Reset wins
            losses = 0; // Reset losses
            dealerTough = 0; // Reset dealer strategy
            printf("The dealer has gone easy again.\n");
        }

        printf("Your current balance: %.2f\n", *balance);
        addData(file, playerName, playerHand[0], playerHand[1], dealerHand[0], *balance); // Modify as needed

        // Ask if the player wants to continue
        printf("Do you want to bet again or go back to the menu? (bet/back): ");
        scanf("%s", input);
        if (strcmp(input, "back") == 0) {
            return;
        }
    }

    printf("You've run out of money. This is how gambling can lead to loss.\n");
}