#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 
                 0, 1, 0, -eye_pos[1],
                 0, 0, 1, -eye_pos[2], 
                 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    Eigen::Matrix4f translate;
    double alpha = rotation_angle / 180 * MY_PI;
    //X
    /*translate << 1, 0, 0, 0,
                 0, cos(alpha), -sin(alpha), 0,
                 0, sin(alpha), cos(alpha), 0,
                 0, 0, 0, 1;*/

    //Y
    translate << cos(alpha), 0, sin(alpha), 0,
                 0, 1, 0, 0,
                 -sin(alpha), 0, cos(alpha), 0,
                 0, 0, 0, 1;

    //Z
    /*translate << cos(alpha), -sin(alpha), 0, 0,
                 sin(alpha), cos(alpha), 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1;*/
    // 

    model = translate * model;
    return model;
}

Eigen::Matrix4f get_model_matrix_by_axis(Eigen::Vector3f axis, float rotation_angle) {
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    double alpha = rotation_angle / 180 * MY_PI;

    Eigen::Matrix3f I = Eigen::Matrix3f::Identity();
    Eigen::Matrix3f M;
    Eigen::Matrix3f Rk;
    Rk << 0, -axis[2], axis[1],
        axis[2], 0, -axis[0],
        -axis[1], axis[0], 0;

    M = I + (1 - cos(alpha)) * Rk * Rk + sin(alpha) * Rk;

    model << M(0, 0), M(0, 1), M(0, 2), 0,
        M(1, 0), M(1, 1), M(1, 2), 0,
        M(2, 0), M(2, 1), M(2, 2), 0,
        0, 0, 0, 1;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    //https://zhuanlan.zhihu.com/p/264778638
    //Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    //Eigen::Matrix4f m;      // Í¸ÊÓ¾ØÕó
    //Eigen::Matrix4f n, p;
    //m << zNear, 0, 0, 0,
    //    0, zNear, 0, 0,
    //    0, 0, zNear + zFar, -zNear * zFar,
    //    0, 0, -1, 0;
    //float halve = eye_fov / 2 * MY_PI / 180;
    //float top = tan(halve) * zNear;
    //float bottom = -top;
    //float right = top * aspect_ratio;
    //float left = -right;
    //n << 2 / (right - left), 0, 0, 0,
    //    0, 2 / (top - bottom), 0, 0,
    //    0, 0, 2 / (zNear - zFar), 0,
    //    0, 0, 0, 1;
    //p << 1, 0, 0, -(right + left) / 2,
    //    0, 1, 0, -(top + bottom) / 2,
    //    0, 0, 1, (zFar + zNear) / 2,
    //    0, 0, 0, 1;
    //projection = n * p * m;

    eye_fov = eye_fov / 180 * MY_PI;
    Eigen::Matrix4f aspect_fovY;
    float ty = -1.0f / tan(eye_fov / 2.0f);
    aspect_fovY << (ty / aspect_ratio), 0, 0, 0,
        0, ty, 0, 0,
        0, 0, (zNear + zFar) / (zNear - zFar), (-2 * zNear * zFar) / (zNear - zFar),
        0, 0, 1, 0;
    projection = aspect_fovY * projection;

    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = { 0, 0, 5 };

    std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };

    std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        //r.set_model(get_model_matrix(angle));
        r.set_model(get_model_matrix_by_axis(Eigen::Vector3f(-1, 1, 0), angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
