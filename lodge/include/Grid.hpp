#pragma once

#include "Eigen/Dense"

namespace Lodge {
    class Grid {
    public:
        Grid(int width, int height);

        int width() const { return m_width; }
        int height() const { return m_height; }

        float density(const int i, const int j) const { return m_density(i, j); }
        void setDensity(const int i, const int j, const float value) { m_density(i, j) = value; }

        float velocityX(const int i, const int j) const { return m_velocityX(i, j); }
        float velocityY(const int i, const int j) const { return m_velocityY(i, j); }
        float temperature(const int i, const int j) const { return m_temperature(i, j); }
        float fuel(const int i, const int j) const { return m_fuel(i, j); }

        void setVelocity(const int i, const int j, const float vx, const float vy) {
            m_velocityX(i, j) = vx;
            m_velocityY(i, j) = vy;
        }

        void setTemperature(const int i, const int j, const float t) {
            m_temperature(i, j) = t;
        }

        void setFuel(const int i, const int j, const float f) {
            m_fuel(i, j) = f;
        }

        Eigen::ArrayXXf &densityField() { return m_density; }
        Eigen::ArrayXXf &velocityXField() { return m_velocityX; }
        Eigen::ArrayXXf &velocityYField() { return m_velocityY; }
        Eigen::ArrayXXf &temperatureField() { return m_temperature; }
        Eigen::ArrayXXf &fuelField() { return m_fuel; }

    private:
        int m_width;
        int m_height;

        Eigen::ArrayXXf m_velocityX;
        Eigen::ArrayXXf m_velocityY;
        Eigen::ArrayXXf m_density;
        Eigen::ArrayXXf m_temperature;
        Eigen::ArrayXXf m_fuel;
    };
}
