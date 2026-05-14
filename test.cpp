#include <iostream>
#include <conio.h>
#include <ctime>
#include <random>
#include <Windows.h>

int main() {

    int mas[100][100];

    std::default_random_engine randomEngine(time(NULL));
    std::uniform_int_distribution<int> randomNum(0, 2);

    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 100; j++) {
            mas[i][j] = 0;
        }
    }

    for (int i = 30; i < 35;) {
        for (int j = 0; j < 100;) {
            int num = randomNum(randomEngine);
            if (num == 0 || num == 1) {
                mas[i][j] = num;
                j++;
            }
        }
        i++;
    }

    for (int i = 35; i < 45; i++) {
        for (int j = 0; j < 100; j++) {
            mas[i][j] = 1;
        }
    }

    for (int i = 45; i < 100;) {
        for (int j = 0; j < 100;) {
            int num = randomNum(randomEngine);
            if (num == 1 || num == 2) {
                mas[i][j] = num;
                j++;
            }
        }
        i++;
    }

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            std::cout << mas[i][j];
        }
        std::cout << "\n";
    }

    _getch();
    return 0;
}