#include "Grid.hpp"
#include "Solver.hpp"
#include "raylib.h"
#include <vector>

using namespace Lodge;

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
                float d = grid.density(i, j);
                if (d < 0.0f) d = 0.0f;
                if (d > 1.0f) d = 1.0f;
                const auto v = static_cast<unsigned char>(d * 255);
                m_pixels[j * grid.width() + i] = {v, v, v, 255};
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
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 800;
    constexpr int gridSize = 64;

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
                grid.setDensity(i, j, 1.0f);
                grid.setTemperature(i, j, 5.0f);
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
