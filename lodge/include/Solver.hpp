#pragma once

#include <Eigen/Dense>
#include "Grid.hpp"

namespace Lodge {
    class Solver {
    public:
        static Eigen::ArrayXXf advect(const Grid &grid, const Eigen::ArrayXXf &field, float dt);

        static void step(Grid &grid, float dt);

    private:
        static float sampleBilinear(const Eigen::ArrayXXf &field, float x, float y);
    };
}
