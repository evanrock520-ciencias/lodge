#include "Grid.hpp"

namespace Lodge {
    Grid::Grid(int width, int height)
        : m_width(width), m_height(height),
          m_velocityX(Eigen::ArrayXXf::Zero(width, height)),
          m_velocityY(Eigen::ArrayXXf::Zero(width, height)),
          m_density(Eigen::ArrayXXf::Zero(width, height)),
          m_temperature(Eigen::ArrayXXf::Zero(width, height)),
          m_fuel(Eigen::ArrayXXf::Zero(width, height)) {
    };
}
