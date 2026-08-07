add_library(lodge
        src/Grid.cpp
        src/Solver.cpp
)

target_include_directories(lodge PUBLIC include)

target_link_libraries(lodge PUBLIC raylib Eigen3::Eigen)
