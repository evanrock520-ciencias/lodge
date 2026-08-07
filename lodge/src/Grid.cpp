#include "Grid.hpp"

namespace Lodge {
    Grid::Grid(int width, int height)
        : m_width(width), m_height(height),
          m_velocityX(Eigen::ArrayXXf::Zero(width, height)),
          m_velocityY(Eigen::ArrayXXf::Zero(width, height)),
          m_density(Eigen::ArrayXXf::Zero(width, height)),
          m_temperature(Eigen::ArrayXXf::Zero(width, height)),
          m_velocityXOld(Eigen::ArrayXXf::Zero(width, height)),
          m_velocityYOld(Eigen::ArrayXXf::Zero(width, height)),
          m_densityOld(Eigen::ArrayXXf::Zero(width, height)),
          m_temperatureOld(Eigen::ArrayXXf::Zero(width, height)) {
    }
}
