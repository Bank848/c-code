#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// คำนวณ EV ของเกมเพื่อแสดงให้ผู้เล่นเห็นว่าโอกาสเสียเงินเป็นเท่าไร
double calculateExpectedValue() {
    double win3Match = 0.001 * 5;    // ความน่าจะเป็น 0.1% ของการได้ 5 บาท
    double win2Match = 0.01 * 2;     // ความน่าจะเป็น 1% ของการได้ 2 บาท
    double loseBet = 0.989 * -1;     // ความน่าจะเป็น 98.9% ของการเสียเงินเดิมพัน

    return win3Match + win2Match + loseBet; // ค่า EV ต่อการหมุนหนึ่งครั้ง
}

void playGame(int *balance) {
    int bet;
    printf("Enter your bet amount: ");
    scanf("%d", &bet);

    if(bet > *balance) {
        printf("You don't have enough money to make this bet.\n");
        return;
    }

    *balance -= bet; // หักจำนวนเงินที่เดิมพันจากยอดคงเหลือของผู้เล่น

    int result1 = rand() % 10;
    int result2 = rand() % 10;
    int result3 = rand() % 10;

    printf("Slot results: [%d] [%d] [%d]\n", result1, result2, result3);

    // ตรวจสอบว่าผู้เล่นชนะหรือไม่
    if(result1 == result2 && result2 == result3) {
        int winAmount = bet * 5;
        printf("Congratulations! You won %d.\n", winAmount);
        *balance += winAmount;
    } else {
        printf("Sorry, you lost this round.\n");
    }

    printf("Your current balance: %d\n", *balance);
}

int main() {
    int balance = 1000; // เงินเริ่มต้นของผู้เล่น
    int choice;
    
    srand(time(0)); // ตั้งค่า random seed

    double ev = calculateExpectedValue(); // คำนวณ EV ของเกมสล็อต

    printf("Welcome to the Slot Machine Game! \n");
    printf("The goal is to show how gambling can cause you to lose your money over time.\n");
    printf("Expected loss per play (EV): %.3f\n", ev);

    while(balance > 0) {
        printf("\n--- Menu ---\n");
        printf("1. Play Slot Machine\n");
        printf("2. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                playGame(&balance);
                break;
            case 2:
                printf("Thank you for playing! You ended with a balance of %d.\n", balance);
                return 0;
            default:
                printf("Invalid choice! Please select again.\n");
        }
    }

    printf("You've run out of money. This is how gambling can lead to loss.\n");
    return 0;
}