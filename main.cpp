#include "Grid.hpp"
#include "Solver.hpp"
#include "raylib.h"
#include <vector>

using namespace Lodge;

constexpr float MAX_TEMP = 6.0f; // ← falta

Color temperatureToColor(float temp, float maxTemp) {
    float t = std::clamp(temp / maxTemp, 0.0f, 1.0f);

    struct Stop {
        float pos;
        unsigned char r, g, b;
    };
    static const Stop stops[] = {
        {0.0f, 0, 0, 0},
        {0.3f, 180, 0, 0},
        {0.6f, 255, 120, 0},
        {1.0f, 255, 255, 100},
    };

    for (int i = 0; i < 3; i++) {
        if (t >= stops[i].pos && t <= stops[i + 1].pos) {
            float localT = (t - stops[i].pos) / (stops[i + 1].pos - stops[i].pos);
            unsigned char r = static_cast<unsigned char>(stops[i].r + (stops[i + 1].r - stops[i].r) * localT);
            unsigned char g = static_cast<unsigned char>(stops[i].g + (stops[i + 1].g - stops[i].g) * localT);
            unsigned char b = static_cast<unsigned char>(stops[i].b + (stops[i + 1].b - stops[i].b) * localT);
            return {r, g, b, 255};
        }
    }
    return {255, 255, 100, 255};
}

class GridRenderer {
public:
    GridRenderer(int gridWidth, int gridHeight)
        : m_pixels(gridWidth * gridHeight) {
        const Image img = GenImageColor(gridWidth, gridHeight, BLACK);
        m_texture = LoadTextureFromImage(img);
        UnloadImage(img);
        SetTextureFilter(m_texture, TEXTURE_FILTER_BILINEAR);
    }

    ~GridRenderer() {
        UnloadTexture(m_texture);
    }

    void draw(const Grid &grid, const int screenWidth, const int screenHeight) {
        for (int j = 0; j < grid.height(); j++) {
            for (int i = 0; i < grid.width(); i++) {
                float temp = grid.temperature(i, j);
                float dens = grid.density(i, j);

                Color c = temperatureToColor(temp, MAX_TEMP);
                c.a = static_cast<unsigned char>(std::clamp(dens, 0.0f, 1.0f) * 255);

                m_pixels[j * grid.width() + i] = c;
            }
        }
        UpdateTexture(m_texture, m_pixels.data());

        const Rectangle src = {0, 0, static_cast<float>(grid.width()), static_cast<float>(grid.height())};
        const Rectangle dst = {0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight)};
        DrawTexturePro(m_texture, src, dst, {0, 0}, 0.0f, WHITE);
    }

private:
    Texture2D m_texture;
    std::vector<Color> m_pixels;
};

int main() {
    constexpr int screenWidth = 1200;
    constexpr int screenHeight = 1200;
    constexpr int gridSize = 128;

    InitWindow(screenWidth, screenHeight, "Fire Solver");
    SetTargetFPS(60);

    Grid grid(gridSize, gridSize);
    Solver solver;
    GridRenderer renderer(gridSize, gridSize);

    const int sourceX = gridSize / 2;
    const int sourceY = gridSize - 5;
    const int sourceRadius = 4;

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();

        for (int j = sourceY - sourceRadius; j < sourceY + sourceRadius; j++) {
            for (int i = sourceX - sourceRadius; i < sourceX + sourceRadius; i++) {
                grid.setFuel(i, j, 1.0f);
            }
        }

        solver.step(grid, dt);

        BeginDrawing();
        ClearBackground(BLACK);
        renderer.draw(grid, screenWidth, screenHeight);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
