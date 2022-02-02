// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>

#define SSAA_MUL 2
#define MSAA_MUL 2

rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(float x, float y, const Vector3f* _v)
{
    Eigen::Vector2f p0p1(_v[0].x() - _v[1].x(), _v[0].y() - _v[1].y());
    Eigen::Vector2f p1p2(_v[1].x() - _v[2].x(), _v[1].y() - _v[2].y());
    Eigen::Vector2f p2p0(_v[2].x() - _v[0].x(), _v[2].y() - _v[0].y());

    Eigen::Vector2f p0p(_v[0].x() - x, _v[0].y() - y);
    Eigen::Vector2f p1p(_v[1].x() - x, _v[1].y() - y);
    Eigen::Vector2f p2p(_v[2].x() - x, _v[2].y() - y);

    float cross1 = p0p1.x() * p0p.y() - p0p1.y() * p0p.x();
    float cross2 = p1p2.x() * p1p.y() - p1p2.y() * p1p.x();
    float cross3 = p2p0.x() * p2p.y() - p2p0.y() * p2p.x();

    return (cross1 > 0 && cross2 > 0 && cross3 > 0) || (cross1 < 0 && cross2 < 0 && cross3 < 0);
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();    
    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle

    //If so, use the following code to get the interpolated z value.
    //auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //z_interpolated *= w_reciprocal;

    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
    std::vector<float> x_arry{ v[0].x(), v[1].x(), v[2].x() };
    std::vector<float> y_arry{ v[0].y(), v[1].y(), v[2].y() };
    std::sort(x_arry.begin(), x_arry.end());
    std::sort(y_arry.begin(), y_arry.end());
    int min_x = floor(x_arry[0]), max_x = ceil(x_arry[2]),
        min_y = floor(y_arry[0]), max_y = ceil(y_arry[2]);

    for (int x = min_x; x < max_x; x++) {
        for (int y = min_y; y < max_y; y++) {

            int counter = 0;        //to count how many pixel in triangle
            for (int x_MSAA = 0; x_MSAA < MSAA_MUL; x_MSAA++) {
                for (int y_MSAA = 0; y_MSAA < MSAA_MUL; y_MSAA++) {
                    
                    float real_x = x + x_MSAA / MSAA_MUL + 1 / (2 * MSAA_MUL),
                        real_y = y + y_MSAA / MSAA_MUL + 1 / (2 * MSAA_MUL);
                    if (insideTriangle(real_x, real_y, t.v)) {                        
                        // get the interpolated z value.
                        // change x, y to real_x, real_y
                        auto [alpha, beta, gamma] = computeBarycentric2D(real_x, real_y, t.v);
                        float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                        float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                        z_interpolated *= w_reciprocal;

                        if (z_interpolated < depth_buf_MSAA[get_index_MSAA(x, y, x_MSAA, y_MSAA)]) {
                            counter++;
                            depth_buf_MSAA[get_index_MSAA(x, y, x_MSAA, y_MSAA)] = z_interpolated;
                        }                        
                    }
                }
            }
            // downsampling
            if (counter) {
                float blend_rate = 1.0f * counter / (MSAA_MUL * MSAA_MUL);
                Eigen::Vector3f color = t.getColor() * blend_rate;
                //(t.getColor() * 1.0f * counter + frame_buf[get_index(x, y)] * (MSAA_MUL * MSAA_MUL - counter)) / (MSAA_MUL * MSAA_MUL);
                Eigen::Vector3f point(x, y, 1.0f);
                set_pixel(point, color);
            }
        }
    }
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{ 0, 0, 0 });
        //std::fill(frame_buf_SSAA.begin(), frame_buf_SSAA.end(), Eigen::Vector3f{ 0, 0, 0 });
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
        //std::fill(depth_buf_SSAA.begin(), depth_buf_SSAA.end(), std::numeric_limits<float>::infinity());
        std::fill(depth_buf_MSAA.begin(), depth_buf_MSAA.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    //frame_buf_SSAA.resize(SSAA_MUL * SSAA_MUL * w * h);
    depth_buf.resize(w * h);
    //depth_buf_SSAA.resize(SSAA_MUL * SSAA_MUL * w * h);
    depth_buf_MSAA.resize(MSAA_MUL * MSAA_MUL * w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

int rst::rasterizer::get_index_SSAA(int x, int y, int x_SSAA, int y_SSAA)
{
    return (height * SSAA_MUL - 1 - y * SSAA_MUL - y_SSAA) * width * SSAA_MUL + x * SSAA_MUL + x_SSAA;
}

int rst::rasterizer::get_index_MSAA(int x, int y, int x_MSAA, int y_MSAA)
{
    return (height * MSAA_MUL - 1 - y * MSAA_MUL - y_MSAA) * width * MSAA_MUL + x * MSAA_MUL + x_MSAA;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

void rst::rasterizer::set_pixel_SSAA(const Eigen::Vector3f& point, int x_SSAA, int y_SSAA, const Eigen::Vector3f& color)
{
    auto ind = get_index_SSAA(point.x(), point.y(), x_SSAA, y_SSAA);
    frame_buf_SSAA[ind] = color;
}
// clang-format on