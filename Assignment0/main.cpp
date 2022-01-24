#include<cmath>
#include<Eigen/Core>
#include<Eigen/Dense>
#include<iostream>

constexpr double MY_PI = 3.1415926;

int main() {
    Eigen::Vector3f p(2, 1, 0);
    Eigen::Matrix3f rotation, transform;
    double theta = 45.0 / 180.0 * MY_PI; 
    rotation << cos(theta), -sin(theta), 0,
                sin(theta), cos(theta), 0,
                0, 0, 1;
    transform << 1, 0, 1,
                 0, 1, 2,
                 0, 0, 1; 

    p = transform * rotation * p; 
    std::cout << p << std::endl;

    return 0;
}