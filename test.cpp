#include "FastNoiseLite.h"
#include <vector>
#include "raylib/include/raylib.h"

//батюшка дипсик спасает, на примере https://github.com/neskech/TerrariaGL

// Генерирует карту высот для мира шириной 'width' блоков
std::vector<int> generateHeightmap(int width, int seed) {
    // 1. Настраиваем генератор шума
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetSeed(seed);

    // Частота определяет "масштаб" холмов. 0.01 = очень плавные горы
    // Поэкспериментируйте со значениями 0.005, 0.02, 0.05
    noise.SetFrequency(0.01f);

    // 2. Создаем массив для хранения высоты (Y-координаты земли) для каждого X
    std::vector<int> heightmap(width);

    // 3. Базовая высота уровня земли (например, середина экрана 300)
    int baseGroundLevel = 300;

    for (int x = 0; x < width; ++x) {
        // Получаем значение шума для текущего X. Оно будет в диапазоне -1..1
        float noiseValue = noise.GetNoise((float)x, 0.0f);

        // Определяем "амплитуду" - насколько сильно холмы будут отклоняться от базового уровня
        int amplitude = 80; // Максимальное отклонение от baseGroundLevel

        // Вычисляем финальную высоту земли в этом столбце
        // noiseValue от -1 до 1 -> превращаем в отклонение от -amplitude до amplitude
        int groundY = baseGroundLevel + (int)(noiseValue * amplitude);

        heightmap[x] = groundY;
    }

    return heightmap;
}

// Определяем типы блоков
enum BlockType {
    AIR = 0,
    DIRT,
    GRASS,
    STONE
};

// Генерирует весь мир: для каждого блока определяет его тип
void generateWorld(std::vector<std::vector<int>>& world, int width, int height, int seed) {
    // Шаг 1: Генерируем карту высот для поверхности
    FastNoiseLite surfaceNoise;
    surfaceNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    surfaceNoise.SetSeed(seed);
    surfaceNoise.SetFrequency(0.01f);

    // Шаг 2: Настраиваем 2D шум для пещер
    FastNoiseLite caveNoise;
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    caveNoise.SetSeed(seed + 1); // Другой seed, чтобы пещеры не совпадали с горами
    caveNoise.SetFrequency(0.05f); // Более высокая частота для маленьких пещер

    int baseGroundLevel = height / 2;

    for (int x = 0; x < width; ++x) {
        // Получаем высоту поверхности
        float surfaceValue = surfaceNoise.GetNoise((float)x, 0.0f);
        int groundY = baseGroundLevel + (int)(surfaceValue * 80);

        for (int y = 0; y < height; ++y) {
            if (y > groundY) {
                // ----- ПОД ЗЕМЛЕЙ -----
                // Получаем значение 2D шума для пещер (x, y)
                float caveValue = caveNoise.GetNoise((float)x, (float)y);

                // Если шум пещер выше порога (например, 0.3), ставим воздух
                if (caveValue > 0.3f) {
                    world[x][y] = AIR;
                }
                // Иначе ставим камень или землю в зависимости от глубины
                else if (y > groundY + 15) {
                    world[x][y] = STONE; // Глубоко под землей - камень
                }
                else {
                    world[x][y] = DIRT;  // Сразу под травой - земля
                }
            }
            else if (y == groundY) {
                // ----- ПОВЕРХНОСТЬ -----
                world[x][y] = GRASS; // Верхний слой - трава
            }
            else if (y < groundY) {
                // ----- НАД ЗЕМЛЕЙ -----
                world[x][y] = AIR;
            }
        }
    }
}

int main() {
    // Размеры мира и окна
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int worldWidth = 300;
    const int worldHeight = 600;

    InitWindow(screenWidth, screenHeight, "ohh is ts fr Terraria?");
    SetTargetFPS(60);

    // 1. Генерируем мир
    std::vector<std::vector<int>> world(worldWidth, std::vector<int>(worldHeight, AIR));
    generateWorld(world, worldWidth, worldHeight, 12345);

    // 2. Загружаем тайлсет (каждый тайл у нас 16x16 пикселей)
    // Представьте, что у вас есть файл tileset.png, где в ряд расположены: трава, земля, камень
    Texture2D tileset = LoadTexture("images/TileMap_ver1.png"); 

    // Координаты тайлов в текстуре (размер каждого тайла, например 16x16)
    Rectangle grassRect = { 0, 0, 16, 16 };
    Rectangle dirtRect = { 16, 0, 16, 16 };
    Rectangle stoneRect = { 32, 0, 16, 16 };

    // !не работает почему то, может изза размеров файла(15х46)?

    Camera2D camera = { 0 };
    camera.target = { 0, 0 };
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.zoom = 1.0f;

    // 3. Игровой цикл
    while (!WindowShouldClose()) {
        // --- Управление камерой (чтобы можно было ходить по миру) ---
        if (IsKeyDown(KEY_RIGHT)) camera.target.x += 5;
        if (IsKeyDown(KEY_LEFT))  camera.target.x -= 5;
        if (IsKeyDown(KEY_UP))    camera.target.y -= 5;
        if (IsKeyDown(KEY_DOWN))  camera.target.y += 5;

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);

        // --- Рисуем мир, начиная с видимой области ---
        // Определяем, какие блоки видны в камеру (чтобы не рисовать весь огромный мир)
        int startX = (int)(camera.target.x - screenWidth / 2) / 16 - 2;
        int endX = (int)(camera.target.x + screenWidth / 2) / 16 + 2;
        int startY = (int)(camera.target.y - screenHeight / 2) / 16 - 2;
        int endY = (int)(camera.target.y + screenHeight / 2) / 16 + 2;

        // Ограничиваем границы, чтобы не выйти за пределы массива
        startX = std::max(0, startX);
        endX = std::min(worldWidth, endX);
        startY = std::max(0, startY);
        endY = std::min(worldHeight, endY);

        for (int x = startX; x < endX; ++x) {
            for (int y = startY; y < endY; ++y) {
                Rectangle* rect = nullptr;
                // Определяем, какой тайл рисовать
                switch (world[x][y]) {
                case GRASS: rect = &grassRect; break;
                case DIRT:  rect = &dirtRect;  break;
                case STONE: rect = &stoneRect; break;
                default: continue; // AIR или неизвестный тип - пропускаем
                }

                // Рисуем тайл в нужной позиции
                Vector2 position = { (float)x * 16, (float)y * 16 };
                DrawTextureRec(tileset, *rect, position, WHITE);
            }
        }

        EndMode2D();

        // Можно добавить простой UI
        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadTexture(tileset);
    CloseWindow();

    return 0;
}
