#include "raylib/include/raylib.h"
void jump();
int main() {
    // Инициализация окна
    InitWindow(800, 600, "My Game");

    // Основные переменные
    int playerX = 400, playerY = 500;  // Координаты игрока
    int playerSpeed = 5;  // Скорость перемещения игрока
    const int g = 10;
    int v0 = 5;

    SetTargetFPS(60); // Ограничение кадров до 60

    // Основной игровой цикл
    while (!WindowShouldClose()) {
        // Логика игры
        if (IsKeyDown(KEY_D)) playerX += playerSpeed;
        if (IsKeyDown(KEY_A)) playerX -= playerSpeed;
        if (IsKeyDown(KEY_W)) 
        // Отрисовка
        BeginDrawing();
        ClearBackground(RED);

        DrawRectangle(playerX, playerY, 50, 50, BLUE);  // Рисуем игрока

        EndDrawing();
    }

    CloseWindow();  // Закрытие окна после завершения игры
    return 0;
}

void jump() {

}