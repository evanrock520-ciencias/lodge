#include "Solver.hpp"
#include "algorithm"
#include <Eigen/Dense>

namespace Lodge {
    Eigen::ArrayXXf Solver::advect(const Grid &grid, const Eigen::ArrayXXf &field, float dt) {
        const int width = grid.width();
        const int height = grid.height();
        Eigen::ArrayXXf result(width, height);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                const float x = static_cast<float>(i);
                const float y = static_cast<float>(j);
                const float vx = grid.velocityX(i, j);
                const float vy = grid.velocityY(i, j);
                const float srcX = x - dt * vx;
                const float srcY = y - dt * vy;
                result(i, j) = sampleBilinear(field, srcX, srcY);
            }
        }

        return result;
    }

    void Solver::step(Grid &grid, float dt) {
        grid.densityField() = advect(grid, grid.densityField(), dt);
    }

    float Solver::sampleBilinear(const Eigen::ArrayXXf &field, float x, float y) {
        const int width = field.rows();
        const int height = field.cols();

        x = std::max(0.0f, std::min(x, static_cast<float>(width - 1)));
        y = std::max(0.0f, std::min(y, static_cast<float>(height - 1)));

        const int i0 = static_cast<int>(x);
        const int j0 = static_cast<int>(y);
        const int i1 = std::min(i0 + 1, width - 1);
        const int j1 = std::min(j0 + 1, height - 1);

        const int sx = x - i0;
        const int sy = y - j0;
        const float top = field(i0, j0) * (1 - sx) + field(i1, j0) * sx;
        const float botton = field(i0, j1) * (1 - sx) + field(i1, j1) * sx;
        return top * (1 - sy) + botton * sy;
    }
}
