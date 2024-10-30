#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // สำหรับฟังก์ชัน sleep
#include <dirent.h> // สำหรับการทำงานของโฟลเดอร์

void addData(FILE *file, int result1, int result2, int result3, int balance);
void Gamehistory(FILE *file);
void readFile(const char *filename, int *minBalance, int *maxBalance, int *count); 
double calculateExpectedValue();
void playGame(int *balance, FILE *file);
void spinAnimation(int *result1, int *result2, int *result3);
void listFiles();
int readBalanceFromBottom(const char *filename);

int main() {
    int balance = 0; // กำหนดค่าเริ่มต้นของ balance เป็น 0
    int choice;
    double ev = calculateExpectedValue();
    char filename[50];
    char filepath[100] = "./"; // กำหนดตำแหน่งให้เป็นโฟลเดอร์ปัจจุบัน

    // แสดงข้อความแนะนำเกม
    printf("===================================\n");
    printf("    Welcome to the Slot Machine Game!\n");
    printf("     Try your luck and win big!\n");
    printf("===================================\n");

    // เมนูหลักสำหรับเริ่มเกมใหม่หรือโหลดเซฟที่มีอยู่แล้ว
    printf("1. Start New Game\n");
    printf("2. Load Existing Game\n");
    printf("Choose an option: ");
    scanf("%d", &choice);

    if (choice == 1) {
        // เริ่มเกมใหม่
        printf("Enter a filename to save your game data (without .txt): ");
        scanf("%s", filename);

        // ตรวจสอบว่าชื่อไฟล์มี .txt อยู่แล้วหรือไม่
        if (strstr(filename, ".txt") == NULL) {
            strcat(filename, ".txt"); // ถ้าไม่มี ให้เพิ่ม .txt
        }

        strcat(filepath, filename); // รวม path และชื่อไฟล์

        FILE *file = fopen(filepath, "a+"); // เปิดไฟล์สำหรับเขียนข้อมูลต่อท้าย

        if (file == NULL) {
            printf("Error opening file!\n");
            return 1;
        }

        // กำหนดค่าเริ่มต้นของ balance
        balance = 50000; // กำหนดเงินเริ่มต้นของเซฟเกมใหม่(เล่นรอบแรก)
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
                    readFile(filepath, &minBalance, &maxBalance, &count); // ส่ง pointer เพื่ออัพเดตค่า
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
        // โหลดเกมที่มีอยู่แล้ว
        printf("Available save files:\n");
        listFiles(); // แสดงรายชื่อไฟล์ .txt ทั้งหมดในโฟลเดอร์

        printf("Enter the filename you want to load (without .txt): ");
        scanf("%s", filename);

        // รีเซ็ตตำแหน่งสำหรับโหลดเซฟไฟล์
        char filepath[100] = "./"; // กำหนดให้ตำแหน่งเป็นโฟลเดอร์ปัจจุบันอีกครั้ง

        // ตรวจสอบว่าชื่อไฟล์มี .txt อยู่แล้วหรือไม่
        if (strstr(filename, ".txt") == NULL) {
            strcat(filename, ".txt"); // ถ้าไม่มี ให้เพิ่ม .txt
        }

        strcat(filepath, filename); // รวม path และชื่อไฟล์

        FILE *file = fopen(filepath, "r+"); // เปิดไฟล์สำหรับอ่านและเขียน
        if (file == NULL) {
            printf("Error opening file! File may not exist.\n");
            return 1;
        }

        // อ่านยอดคงเหลือปัจจุบันจากไฟล์ โดยเริ่มจากท้ายไฟล์
        balance = readBalanceFromBottom(filepath);
        if (balance == -1) {
            printf("No balance found in the file!\n");
            fclose(file);
            return 1; // ออกถ้าไม่พบยอดคงเหลือ
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
                    readFile(filepath, &minBalance, &maxBalance, &count); // ส่ง pointer เพื่ออัพเดตค่า
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

void Gamehistory(FILE *file) { //search ฟังชั่น
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

    FILE *file = fopen(filename, "r"); // เปิดไฟล์เพื่ออ่านค่า
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
        return -1; //error return-1
    }

    // ย้ายตำแหน่งไปที่ท้ายไฟล์
    fseek(file, 0, SEEK_END);
    long position = ftell(file);
    
    // อ่านไฟล์ย้อนกลับจากล่างขึ้นบนเพื่อค้นหารายการยอดคงเหลือสุดท้าย
    char line[256];
    while (position > 0) {
        fseek(file, --position, SEEK_SET);
        if (fgetc(file) == '\n') {
            if (position == 0) { 
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
            return balance; // คืนค่ายอดเงินคงเหลือ
        }
    }

    fclose(file);
    return -1; // ไม่เจอยอดเงิน คืน-1
}

double calculateExpectedValue() {
    double win3Match = 0.001 * 10; //โอกาสชนะได้เงินคูณ10คือ 0.1%
    double loseBet = 0.999 * -1; //โอกาศโดนเจ้ามือรับประทาน99.9% เสีย 99.9สตางค์ต่อ1หมุน

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
    addData(file, result1, result2, result3, *balance); // เซฟข้อมูลลงไฟล์

    // Check for winnings
    if (result1 == result2 && result2 == result3) { // ถ้าชนะยอดเงินพนันx10
        printf("Jackpot! You won: %d\n", bet * 10);
        *balance += bet * 10;
    } else {
        printf("You lost your bet of: %d\n", bet);
    }

    printf("Your remaining balance: %d\n", *balance); // เหลือเงินทั้งหมดเท่าไหร่
    
    
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