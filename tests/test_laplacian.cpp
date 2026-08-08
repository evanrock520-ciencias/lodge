#include <gtest/gtest.h>
#include "Solver.hpp"

namespace Lodge {
    class LaplacianTest : public ::testing::Test {
    protected:
        Solver solver;
    };

    TEST_F(LaplacianTest, Dimensions) {
        constexpr int width = 4;
        constexpr int height = 5;
        solver.buildLaplacian(width, height);

        const auto &laplacian = solver.laplacian();
        EXPECT_EQ(laplacian.rows(), width * height);
        EXPECT_EQ(laplacian.cols(), width * height);
    }

    TEST_F(LaplacianTest, ConstantFieldZeroResult) {
        constexpr int width = 4;
        constexpr int height = 4;
        solver.buildLaplacian(width, height);

        const auto &L = solver.laplacian();
        const int size = width * height;

        Eigen::VectorXf constantField = Eigen::VectorXf::Ones(size) * 7.5f;
        Eigen::VectorXf result = L * constantField;

        for (int i = 0; i < size; ++i) {
            EXPECT_NEAR(result(i), 0.0f, 1e-5f);
        }
    }

    TEST_F(LaplacianTest, StencilValues3x3) {
        constexpr int width = 3;
        constexpr int height = 3;
        solver.buildLaplacian(width, height);

        const auto &L = solver.laplacian();

        // Center cell (1, 1) -> idx = 4
        int centerIdx = Solver::idx(1, 1, width);
        EXPECT_FLOAT_EQ(L.coeff(centerIdx, centerIdx), -4.0f);
        EXPECT_FLOAT_EQ(L.coeff(centerIdx, Solver::idx(0, 1, width)), 1.0f);
        EXPECT_FLOAT_EQ(L.coeff(centerIdx, Solver::idx(2, 1, width)), 1.0f);
        EXPECT_FLOAT_EQ(L.coeff(centerIdx, Solver::idx(1, 0, width)), 1.0f);
        EXPECT_FLOAT_EQ(L.coeff(centerIdx, Solver::idx(1, 2, width)), 1.0f);

        // Corner cell (0, 0) -> idx = 0 (2 neighbors: (1,0) and (0,1))
        int cornerIdx = Solver::idx(0, 0, width);
        EXPECT_FLOAT_EQ(L.coeff(cornerIdx, cornerIdx), -2.0f);
        EXPECT_FLOAT_EQ(L.coeff(cornerIdx, Solver::idx(1, 0, width)), 1.0f);
        EXPECT_FLOAT_EQ(L.coeff(cornerIdx, Solver::idx(0, 1, width)), 1.0f);

        // Edge cell (1, 0) -> 3 neighbors
        int edgeIdx = Solver::idx(1, 0, width);
        EXPECT_FLOAT_EQ(L.coeff(edgeIdx, edgeIdx), -3.0f);
    }

    TEST_F(LaplacianTest, MatrixSymmetry) {
        constexpr int width = 4;
        constexpr int height = 4;
        solver.buildLaplacian(width, height);

        const auto &L = solver.laplacian();
        Eigen::SparseMatrix<float> L_transpose = L.transpose();

        Eigen::SparseMatrix<float> diff = L - L_transpose;
        EXPECT_NEAR(diff.norm(), 0.0f, 1e-6f);
    }
}
