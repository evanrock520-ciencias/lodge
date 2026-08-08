#include "Solver.hpp"
#include <algorithm>
#include <Eigen/Dense>
#include "iostream"

namespace Lodge {
    Eigen::ArrayXXf Solver::advect(const Eigen::ArrayXXf &field, const Eigen::ArrayXXf &velX,
                                   const Eigen::ArrayXXf &velY, float dt) {
        const int width = static_cast<int>(field.rows());
        const int height = static_cast<int>(field.cols());
        Eigen::ArrayXXf result(width, height);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                const float x = static_cast<float>(i);
                const float y = static_cast<float>(j);
                const float vx = velX(i, j);
                const float vy = velY(i, j);
                const float srcX = x - dt * vx;
                const float srcY = y - dt * vy;
                result(i, j) = sampleBilinear(field, srcX, srcY);
            }
        }

        return result;
    }

    Eigen::VectorXf Solver::computeDivergence(Eigen::ArrayXXf &vx, Eigen::ArrayXXf &vy) {
        const int width = vx.rows();
        const int height = vx.cols();
        const int size = width * height;

        Eigen::VectorXf divergence(size);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                const int matrix_idx = idx(i, j, width);

                // dvx/dx
                float dvxdx;
                if (i + 1 < width && i - 1 >= 0) {
                    dvxdx = (vx(i + 1, j) - vx(i - 1, j)) / 2.0f;
                } else if (i + 1 < width) {
                    dvxdx = vx(i + 1, j) - vx(i, j);
                } else if (i - 1 >= 0) {
                    dvxdx = vx(i, j) - vx(i - 1, j);
                } else {
                    dvxdx = 0.0f;
                }

                // dvy/dy
                float dvydy;
                if (j + 1 < height && j - 1 >= 0) {
                    dvydy = (vy(i, j + 1) - vy(i, j - 1)) / 2.0f;
                } else if (j + 1 < height) {
                    dvydy = vy(i, j + 1) - vy(i, j);
                } else if (j - 1 >= 0) {
                    dvydy = vy(i, j) - vy(i, j - 1);
                } else {
                    dvydy = 0.0f;
                }

                divergence(matrix_idx) = dvxdx + dvydy;
            }
        }

        return divergence;
    }

    std::pair<Eigen::ArrayXXf, Eigen::ArrayXXf>
    Solver::computeGradient(const Eigen::VectorXf &p, int width, int height) {
        Eigen::ArrayXXf gradientX(width, height);
        Eigen::ArrayXXf gradientY(width, height);

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                // dp/dx
                float dpdx;
                if (i + 1 < width && i - 1 >= 0) {
                    dpdx = (p(idx(i + 1, j, width)) - p(idx(i - 1, j, width))) / 2.0f;
                } else if (i + 1 < width) {
                    dpdx = p(idx(i + 1, j, width)) - p(idx(i, j, width));
                } else if (i - 1 >= 0) {
                    dpdx = p(idx(i, j, width)) - p(idx(i - 1, j, width));
                } else {
                    dpdx = 0.0f;
                }

                // dp/dy
                float dpdy;
                if (j + 1 < height && j - 1 >= 0) {
                    dpdy = (p(idx(i, j + 1, width)) - p(idx(i, j - 1, width))) / 2.0f;
                } else if (j + 1 < height) {
                    dpdy = p(idx(i, j + 1, width)) - p(idx(i, j, width));
                } else if (j - 1 >= 0) {
                    dpdy = p(idx(i, j, width)) - p(idx(i, j - 1, width));
                } else {
                    dpdy = 0.0f;
                }

                gradientX(i, j) = dpdx;
                gradientY(i, j) = dpdy;
            }
        }

        return {gradientX, gradientY};
    }

    void Solver::project(Grid &grid) {
        const int width = grid.width();
        const int height = grid.height();

        if (!m_laplacianBuilt) {
            buildLaplacian(width, height);
            m_pressureSolver.setMaxIterations(100);
            m_pressureSolver.setTolerance(1e-3f);
            m_pressureSolver.compute(m_laplacian);
            m_laplacianBuilt = true;
        }

        Eigen::VectorXf b = -computeDivergence(grid.velocityXField(), grid.velocityYField());
        b.array() -= b.mean(); // Project out the null space

        Eigen::VectorXf p = m_pressureSolver.solve(b);

        auto [gradX, gradY] = computeGradient(p, width, height);

        grid.velocityXField() -= gradX;
        grid.velocityYField() -= gradY;
    }

    void Solver::applyBuoyancy(Grid &grid, float dt, float buoyancyStrength) {
        const int width = grid.width();
        const int height = grid.height();

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                const float temperature = grid.temperature(i, j);
                const float force = buoyancyStrength * temperature * dt;
                float newVy = grid.velocityY(i, j) - force;
                grid.setVelocity(i, j, grid.velocityX(i, j), newVy);
            }
        }
    }

    void Solver::applyCombustion(Grid &grid, float dt, float burnRate, float heatRelease, float smokeRelease) {
        const int width = grid.width();
        const int height = grid.height();

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                const float fuel = grid.fuel(i, j);
                if (fuel <= 0.0f) continue;
                const float burned = std::min(fuel, burnRate * dt);

                grid.setFuel(i, j, fuel - burned);
                grid.setTemperature(i, j, grid.temperature(i, j) + heatRelease * burned);
                grid.setDensity(i, j, grid.density(i, j) + smokeRelease * burned);
            }
        }
    }

    void Solver::step(Grid &grid, float dt) {
        const Eigen::ArrayXXf velX = grid.velocityXField();
        const Eigen::ArrayXXf velY = grid.velocityYField();

        grid.densityField() = advect(grid.densityField(), velX, velY, dt);
        grid.temperatureField() = advect(grid.temperatureField(), velX, velY, dt);

        const Eigen::ArrayXXf newVelX = advect(velX, velX, velY, dt);
        const Eigen::ArrayXXf newVelY = advect(velY, velX, velY, dt);

        grid.velocityXField() = newVelX;
        grid.velocityYField() = newVelY;

        applyCombustion(grid, 0.016);
        applyBuoyancy(grid, dt, 1.0);
        project(grid);
    }

    float Solver::sampleBilinear(const Eigen::ArrayXXf &field, float x, float y) {
        const int width = static_cast<int>(field.rows());
        const int height = static_cast<int>(field.cols());

        x = std::max(0.0f, std::min(x, static_cast<float>(width - 1)));
        y = std::max(0.0f, std::min(y, static_cast<float>(height - 1)));

        const int i0 = static_cast<int>(x);
        const int j0 = static_cast<int>(y);
        const int i1 = std::min(i0 + 1, width - 1);
        const int j1 = std::min(j0 + 1, height - 1);

        const float sx = x - i0;
        const float sy = y - j0;
        const float top = field(i0, j0) * (1.0f - sx) + field(i1, j0) * sx;
        const float botton = field(i0, j1) * (1.0f - sx) + field(i1, j1) * sx;
        return top * (1.0f - sy) + botton * sy;
    }

    void Solver::buildLaplacian(int width, int height) {
        int size = width * height;
        m_laplacian = Eigen::SparseMatrix<float>(size, size);
        std::vector<Eigen::Triplet<float> > triplets;
        triplets.reserve(5 * size);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                int matrix_idx = idx(i, j, width);
                float diag = 0.0f;

                if (i + 1 < width) {
                    triplets.emplace_back(matrix_idx, idx(i + 1, j, width), -1.0f);
                    diag += 1.0f;
                }

                if (i - 1 >= 0) {
                    triplets.emplace_back(matrix_idx, idx(i - 1, j, width), -1.0f);
                    diag += 1.0f;
                }

                if (j + 1 < height) {
                    triplets.emplace_back(matrix_idx, idx(i, j + 1, width), -1.0f);
                    diag += 1.0f;
                }

                if (j - 1 >= 0) {
                    triplets.emplace_back(matrix_idx, idx(i, j - 1, width), -1.0f);
                    diag += 1.0f;
                }

                triplets.emplace_back(matrix_idx, matrix_idx, diag);
            }
        }
        m_laplacian.setFromTriplets(triplets.begin(), triplets.end());
    }
}
