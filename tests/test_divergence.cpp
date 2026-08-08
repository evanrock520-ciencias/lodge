#include "gtest/gtest.h"
#include "Solver.hpp"

namespace Lodge {
    class DivergenceTest : public ::testing::Test {
    protected:
        Solver solver;
    };

    TEST_F(DivergenceTest, Dimensions) {
        constexpr int width = 5;
        constexpr int height = 5;
        Eigen::ArrayXXf vx = Eigen::ArrayXXf::Zero(width, height);
        Eigen::ArrayXXf vy = Eigen::ArrayXXf::Zero(width, height);

        auto div = solver.computeDivergence(vx, vy);
        EXPECT_EQ(div.size(), width * height);
    }

    TEST_F(DivergenceTest, ConstantFieldZeroDivergence) {
        constexpr int width = 5, height = 5;
        Eigen::ArrayXXf vx = Eigen::ArrayXXf::Constant(width, height, 3.0f);
        Eigen::ArrayXXf vy = Eigen::ArrayXXf::Constant(width, height, -2.0f);

        auto div = solver.computeDivergence(vx, vy);

        for (int i = 0; i < div.size(); ++i) {
            EXPECT_NEAR(div(i), 0.0f, 1e-5f);
        }
    }

    TEST_F(DivergenceTest, LinearFieldConstantDivergence) {
        constexpr int width = 5, height = 5;
        Eigen::ArrayXXf vx(width, height);
        Eigen::ArrayXXf vy = Eigen::ArrayXXf::Zero(width, height);

        for (int j = 0; j < height; ++j)
            for (int i = 0; i < width; ++i)
                vx(i, j) = static_cast<float>(i);

        auto div = solver.computeDivergence(vx, vy);

        for (int i = 0; i < div.size(); ++i) {
            EXPECT_NEAR(div(i), 1.0f, 1e-5f);
        }
    }
}
