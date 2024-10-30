#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // For sleep function
#include <dirent.h> // For directory operations

void addData(FILE *file, int result1, int result2, int result3, int balance);
void Gamehistory(FILE *file);
void readFile(const char *filename, int *minBalance, int *maxBalance, int *count); // Updated function prototype
double calculateExpectedValue();
void playGame(int *balance, FILE *file);
void spinAnimation(int *result1, int *result2, int *result3);
void listFiles();
int readBalanceFromBottom(const char *filename);

int main() {
    int balance = 0; // Initialize balance to 0
    int choice;
    double ev = calculateExpectedValue();
    char filename[50];
    char filepath[100] = "./"; // Set the path to the current directory

    // Display introductory message
    printf("===================================\n");
    printf("    Welcome to the Slot Machine Game!\n");
    printf("     Try your luck and win big!\n");
    printf("===================================\n");

    // Main menu for starting a new game or loading an existing game
    printf("1. Start New Game\n");
    printf("2. Load Existing Game\n");
    printf("Choose an option: ");
    scanf("%d", &choice);

    if (choice == 1) {
        // Start a new game
        printf("Enter a filename to save your game data (without .txt): ");
        scanf("%s", filename);

        // Check if the filename already ends with .txt
        if (strstr(filename, ".txt") == NULL) {
            strcat(filename, ".txt"); // Append .txt if not present
        }

        strcat(filepath, filename); // Combine path and filename

        FILE *file = fopen(filepath, "a+"); // Open file for appending data

        if (file == NULL) {
            printf("Error opening file!\n");
            return 1;
        }

        // Initialize balance
        balance = 1000; // Set initial balance for a new game
        srand(time(0));
        printf("Expected loss per play (EV): %.3f\n", ev);

        while (balance > 0) {
            printf("\n--- Menu ---\n");
            printf("1. Play Slot Machine\n");
            printf("2. Game history\n");
            printf("3. Summary of results\n");
            printf("4. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    playGame(&balance, file);
                    break;
                case 2:
                    Gamehistory(file);
                    break;
                case 3: {
                    int minBalance = 1000, maxBalance = 0, count = 0;
                    readFile(filepath, &minBalance, &maxBalance, &count); // Pass in pointers to update
                    printf("Number of plays: %d\n", count);
                    printf("Minimum balance: %d\n", minBalance);
                    printf("Maximum balance: %d\n", maxBalance);
                    break;
                }
                case 4:
                    printf("Thank you for playing! You ended with a balance of %d.\n", balance);
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
        // Load an existing game
        printf("Available save files:\n");
        listFiles(); // List all .txt files in the directory

        printf("Enter the filename you want to load (without .txt): ");
        scanf("%s", filename);

        // Reset the filepath for loading the file
        char filepath[100] = "./"; // Set the path to the current directory again

        // Check if the filename already ends with .txt
        if (strstr(filename, ".txt") == NULL) {
            strcat(filename, ".txt"); // Append .txt if not present
        }

        strcat(filepath, filename); // Combine path and filename

        FILE *file = fopen(filepath, "r+"); // Open file for reading and updating
        if (file == NULL) {
            printf("Error opening file! File may not exist.\n");
            return 1;
        }

        // Read the current balance from the file starting from the bottom
        balance = readBalanceFromBottom(filepath);
        if (balance == -1) {
            printf("No balance found in the file!\n");
            fclose(file);
            return 1; // Exit if no balance found
        }

        srand(time(0));
        printf("Loaded game with balance: %d\n", balance);
        printf("Expected loss per play (EV): %.3f\n", ev);


        while (balance > 0) {
            printf("\n--- Menu ---\n");
            printf("1. Play Slot Machine\n");
            printf("2. Game history\n");
            printf("3. Summary of results\n");
            printf("4. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    playGame(&balance, file);
                    break;
                case 2:
                    Gamehistory(file);
                    break;
                case 3: {
                    int minBalance = 1000, maxBalance = 0, count = 0;
                    readFile(filepath, &minBalance, &maxBalance, &count); // Pass in pointers to update
                    printf("Number of plays: %d\n", count);
                    printf("Minimum balance: %d\n", minBalance);
                    printf("Maximum balance: %d\n", maxBalance);
                    break;
                }
                case 4:
                    printf("Thank you for playing! You ended with a balance of %d.\n", balance);
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

void addData(FILE *file, int result1, int result2, int result3, int balance) {
    fseek(file, 0, SEEK_END); // ย้ายตำแหน่งไปที่ท้ายไฟล์ก่อนบันทึก
    fprintf(file, "Result: [%d] [%d] [%d], Balance: %d\n", result1, result2, result3, balance);
}

void Gamehistory(FILE *file) { //search
    char line[256];
    rewind(file);
    printf("Game history:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
}

void readFile(const char *filename, int *minBalance, int *maxBalance, int *count) {
    int balance;
    char line[256];

    FILE *file = fopen(filename, "r"); // Open file for reading
    if (file == NULL) {
        printf("Could not open file: %s\n", filename);
        return;
    }

    printf("Data from %s:\n", filename);
    while (fgets(line, sizeof(line), file)) {
        // Print the line for debugging
        printf("%s", line);

        // Only count lines with "Result:"
        if (strstr(line, "Result:") != NULL) {
            sscanf(line, "Result: [%*d] [%*d] [%*d], Balance: %d", &balance);
            if (balance < *minBalance) {
                *minBalance = balance;
            }
            if (balance > *maxBalance) {
                *maxBalance = balance;
            }
            (*count)++; // Increment only for valid game plays
        }
    }
    fclose(file);
}

int readBalanceFromBottom(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file: %s\n", filename);
        return -1; // Indicate an error
    }

    // Move to the end of the file
    fseek(file, 0, SEEK_END);
    long position = ftell(file);
    
    // Read the file in reverse to find the last balance entry
    char line[256];
    while (position > 0) {
        fseek(file, --position, SEEK_SET);
        if (fgetc(file) == '\n') {
            if (position == 0) { // If we are at the start of the file
                fseek(file, 0, SEEK_SET);
            } else {
                continue;
            }
        }
        fgets(line, sizeof(line), file);
        if (strstr(line, "Balance:") != NULL) {
            int balance;
            sscanf(line, "Balance: %d", &balance);
            fclose(file);
            return balance; // Return the last found balance
        }
    }

    fclose(file);
    return -1; // Balance not found
}

double calculateExpectedValue() {
    double win3Match = 0.001 * 5;
    double loseBet = 0.999 * -1;

    return win3Match + loseBet;
}

void playGame(int *balance, FILE *file) {
    int bet;
    printf("Enter your bet amount: ");
    scanf("%d", &bet);

    if (bet > *balance) {
        printf("You don't have enough money to make this bet. Your current balance is: %d\n", *balance);
        return;
    }

    *balance -= bet;

    int result1, result2, result3;
    spinAnimation(&result1, &result2, &result3);

    printf("Final Slot results: [%d] [%d] [%d]\n", result1, result2, result3);
    addData(file, result1, result2, result3, *balance); // Log the result to the file

    // Check for winnings
    if (result1 == result2 && result2 == result3) { // Win condition: all three results must be the same
        printf("Jackpot! You won: %d\n", bet * 10);
        *balance += bet * 10;
    } else {
        printf("You lost your bet of: %d\n", bet);
    }

    printf("Your remaining balance: %d\n", *balance); // Show remaining balance
    
    // Save the updated balance after the game play
    fseek(file, 0, SEEK_END); // Move to the end of the file before saving
}

void spinAnimation(int *result1, int *result2, int *result3) {
    printf("Spinning the slot machine...\n");

    for (int i = 0; i < 10; i++) { // Spin for 10 iterations
        *result1 = rand() % 10;
        *result2 = rand() % 10;
        *result3 = rand() % 10;

        printf("\rSlot results: [%d] [%d] [%d]   ", *result1, *result2, *result3);
        fflush(stdout);
        usleep(100000); // Delay for 0.1 seconds
    }
    printf("\n");
}