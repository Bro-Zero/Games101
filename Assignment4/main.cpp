#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;
std::vector<std::vector<int>> move = { {0,0}, {0,1},{0,-1},{1,0},{-1,0},{1,1},{-1,1},{1,-1},{-1,-1} };


void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;
        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm
    if (control_points.size() == 2) {
        return control_points[0] + (control_points[1] - control_points[0]) * t;
    }
    std::vector<cv::Point2f> next_control_points;
    for (int i = 0; i < control_points.size() - 1; i++) {
        next_control_points.emplace_back(control_points[i] + (control_points[i + 1] - control_points[i]) * t);
    }
    return recursive_bezier(next_control_points, t);
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    for (double t = 0.0; t < 1.0; t += 0.001) {
        auto point = recursive_bezier(control_points, t);
        window.at<cv::Vec3b>(point.y, point.x)[1] = 255;
    }
}

void anti_aliasing_bezier(const std::vector<cv::Point2f>& control_points, cv::Mat& window) {
    for (double t = 0.0; t < 1.0; t += 0.001) {
        auto point = recursive_bezier(control_points, t);

        // ·´×ßÑù
        for (auto m : move) {
            cv::Point2i draw_point(point.x + m[0], point.y + m[1]);
            float d_x = abs(point.x - draw_point.x), d_y = abs(point.y - draw_point.y);
            
            float temp = 1.0f - sqrt(d_x * d_x + d_y * d_y);
            int a=0;
            if (temp < 0) {
                int temp;
            }
            int color = std::max(0.0f, (1.0f - sqrt(d_x * d_x + d_y * d_y))) * 255;
            auto point_color = window.at<cv::Vec3b>(draw_point.y, draw_point.x);
            int draw_color = std::min(color + (int)point_color[2], 255);
            window.at<cv::Vec3b>(draw_point.y, draw_point.x)[2] = draw_color;
        }
    }
}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    cv::Mat window_anti_aliasing = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window_anti_aliasing, window_anti_aliasing, cv::COLOR_BGR2RGB);
    cv::namedWindow("Anti Aliasing Bezier Curve", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Anti Aliasing Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
            cv::circle(window_anti_aliasing, point, 3, { 255, 255, 255 }, 3);
        }

        if (control_points.size() == 4) 
        {
            naive_bezier(control_points, window);
            bezier(control_points, window);
            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);

            anti_aliasing_bezier(control_points, window_anti_aliasing);
            cv::imshow("Anti Aliasing Bezier Curve", window_anti_aliasing);
            cv::imwrite("my_anti_aliasing_bezier_curve.png", window_anti_aliasing);

            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        cv::imshow("Anti Aliasing Bezier Curve", window_anti_aliasing);
        key = cv::waitKey(20);
    }

return 0;
}
