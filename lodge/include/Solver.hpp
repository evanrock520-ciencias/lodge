#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/IterativeLinearSolvers>
#include "Grid.hpp"

namespace Lodge {
    class Solver {
    public:
        Eigen::ArrayXXf advect(
            const Eigen::ArrayXXf &field,
            const Eigen::ArrayXXf &velX,
            const Eigen::ArrayXXf &velY,
            float dt);

        void project(Grid &grid);

        void step(Grid &grid, float dt);

        void buildLaplacian(int width, int height);

        static Eigen::VectorXf computeDivergence(Eigen::ArrayXXf &vx, Eigen::ArrayXXf &vy);

        static std::pair<Eigen::ArrayXXf, Eigen::ArrayXXf> computeGradient(
            const Eigen::VectorXf &p, int width, int height);

        void applyBuoyancy(Grid &grid, float dt, float buoyancyStrength = 1.0);

        const Eigen::SparseMatrix<float> &laplacian() const { return m_laplacian; }

        static constexpr int idx(int i, int j, int width) { return j + width * i; }

    private:
        static float sampleBilinear(const Eigen::ArrayXXf &field, float x, float y);

        Eigen::SparseMatrix<float> m_laplacian;
        Eigen::ConjugateGradient<Eigen::SparseMatrix<float> >
        m_pressureSolver;
        bool m_laplacianBuilt = false;
    };
}
