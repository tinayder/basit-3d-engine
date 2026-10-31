/*
 *  /$$$$$$$$$$$   /$$$$$$$$$$$$  /$$$$$$$$$$$$  /$$$$$$$$$$$$  /$$$$$$$$$$$$
 * | $$$$ | $$$$  | $$$$  | $$$$ | $$$$_______/ |/__  $$$$___/ |/__  $$$$___/
 * | $$$$ | $$$$  | $$$$  | $$$$ | $$$$             | $$$$         | $$$$
 * | $$$$$$$$$$$$ | $$$$$$$$$$$$ | $$$$$$$$$$$$     | $$$$         | $$$$
 * | $$$$  | $$$$ | $$$$  | $$$$ |/________$$$$     | $$$$         | $$$$
 * | $$$$  | $$$$ | $$$$  | $$$$           $$$$     | $$$$         | $$$$
 * | $$$$$$$$$$$$ | $$$$  | $$$$  /$$$$$$$$$$$$  /$$$$$$$$$$$$     | $$$$
 * |/___________/ |/___/  |/___/ |/___________/ |/___________/     |/___/
 *
 * Basit 3D Engine v1.7.3
 * [ arabic: basit - "simple" ]
 * (c) 2026 tinayder - MIT License
 * https://github.com/tinayder/basit-3d-engine
 */

#pragma once

#define BASIT_VERSION_MAJOR 1
#define BASIT_VERSION_MINOR 7
#define BASIT_VERSION_PATCH 3
#define BASIT_VERSION "1.7.3"
#define BASIT_VERSION_HEX ((BASIT_VERSION_MAJOR << 16) | (BASIT_VERSION_MINOR << 8) | BASIT_VERSION_PATCH)

#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <map>
#include <thread>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace bst {
    inline constexpr float PI = 3.14159265f;
    inline constexpr float TO_RAD = PI / 180.0f;
    // THREADS
    inline const size_t THREADS_NUM = std::thread::hardware_concurrency();

    // AAAAGH
    class Matrix {
    public:
        float m[16] {
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0
        };

        // identity matrix
        static Matrix identity() {
            return Matrix{
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            };
        }

        // matrix for moving the points
        static Matrix translation(float dx, float dy, float dz) {
            return Matrix{
                1, 0, 0, dx,
                0, 1, 0, dy,
                0, 0, 1, dz,
                0, 0, 0, 1
            };
        }

        // matrix for scaling the points
        static Matrix scale(float sx, float sy, float sz) {
            return Matrix{
                sx, 0, 0, 0,
                0, sy, 0, 0,
                0, 0, sz, 0,
                0, 0, 0, 1
            };
        }

        // matrices for rotating the points
        // x
        static Matrix rotation_x(float rot) {
            float cos = std::cos(rot), sin = std::sin(rot);
            return Matrix{
                1, 0, 0, 0,
                0, cos, -sin, 0,
                0, sin, cos, 0,
                0, 0, 0, 1
            };
        }

        // y
        static Matrix rotation_y(float rot) {
            float cos = std::cos(rot), sin = std::sin(rot);
            return Matrix{
                cos, 0, sin, 0,
                0, 1, 0, 0,
                -sin, 0, cos, 0,
                0, 0, 0, 1
            };
        }

        // and z
        static Matrix rotation_z(float rot) {
            float cos = std::cos(rot), sin = std::sin(rot);
            return Matrix{
                cos, -sin, 0, 0,
                sin, cos, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            };
        }

        // matrix of projection (help)
        static Matrix projection(float fov, float aspect, float near, float far) {
            float fov_scale = 1.0f / std::tan(fov / 2.0f);
            return Matrix{
                fov_scale / aspect, 0, 0, 0,
                0, fov_scale, 0, 0,
                0, 0, -(far + near) / (far - near), -(2.0f * far * near) / (far - near),
                0, 0, 1, 0
            };
        }

        [[nodiscard]] Matrix operator*(const Matrix& other) const {
            Matrix result;
            for (size_t i = 0; i < 4; i++) {
                for (size_t j = 0; j < 4; j++) {
                    for (size_t k = 0; k < 4; k++) {
                        result.m[4 * i + j] += this->m[4 * i + k] * other.m[4 * k + j];
                    }
                }
            }
            return result;
        }
    };

    // 3D point/vector
    class Point {
    public:
        float x, y, z;

        Point(float define_x, float define_y, float define_z) {
            this->x = define_x;
            this->y = define_y;
            this->z = define_z;
        }

        Point() : x(0), y(0), z(0) {}

        Point operator+(const Point& other) const {
            return {this->x + other.x, this->y + other.y, this->z + other.z};
        }

        Point operator-(const Point& other) const {
            return {this->x - other.x, this->y - other.y, this->z - other.z};
        }

        Point operator*(const float k) const {
            return {this->x * k, this->y * k, this->z * k};
        }

        float operator*(const Point& other) const {
            return this->x * other.x + this->y * other.y + this->z * other.z;
        }

        [[nodiscard]] Point cross_product(const Point& other) const {
            Point result;
            result.x = this->y * other.z - this->z * other.y;
            result.y = this->z * other.x - this->x * other.z;
            result.z = this->x * other.y - this->y * other.x;
            return result;
        }

        // length of the vector
        [[nodiscard]] float modulus() const {
            return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
        }

        [[nodiscard]] Point normalize() const {
            float modulus = this->modulus();
            if (modulus < 0.00001f) return {0, 0, 0};
            return {this->x / modulus, this->y / modulus, this->z / modulus};
        }

        [[nodiscard]] Point operator*(const Matrix& mat) const {
            float rx = mat.m[0] * this->x + mat.m[1] * this->y + mat.m[2] * this->z + mat.m[3];
            float ry = mat.m[4] * this->x + mat.m[5] * this->y + mat.m[6] * this->z + mat.m[7];
            float rz = mat.m[8] * this->x + mat.m[9] * this->y + mat.m[10] * this->z + mat.m[11];
            float rw = mat.m[12] * this->x + mat.m[13] * this->y + mat.m[14] * this->z + mat.m[15];
            // making ndc here hehehehe
            if (std::abs(rw) > 0.000001f) return {rx / rw, ry / rw, rz / rw};
            return {rx, ry, rz};
        }
    };

    // camera (fov is in degrees!)
    class Camera {
    public:
        Point position;
        float rot_x, rot_y, rot_z;
        float fov, near, far;
        int screen_width, screen_height;
        float aspect;

        Camera(const Point& define_position,
            float define_rot_x, float define_rot_y, float define_rot_z,
            float define_near, float define_far,
            float define_fov, int define_screen_width, int define_screen_height) {
            this->position = define_position;
            this->rot_x = define_rot_x;
            this->rot_y = define_rot_y;
            this->rot_z = define_rot_z;
            this->fov = define_fov * TO_RAD;
            this->screen_width = define_screen_width;
            this->screen_height = define_screen_height;
            this->near = define_near;
            this->far = define_far;
            this->aspect = static_cast<float>(this->screen_width) / static_cast<float>(this->screen_height);
        }

        [[nodiscard]] Matrix view_matrix() const {
            Matrix translation = Matrix::translation(-position.x, -position.y, -position.z);
            Matrix rotation = Matrix::rotation_z(rot_z * TO_RAD) * Matrix::rotation_y(rot_y * TO_RAD) * Matrix::rotation_x(rot_x * TO_RAD);
            return translation * rotation;
        }

        [[nodiscard]] Matrix projection() const {
            return Matrix::projection(this->fov, this->aspect, this->near, this->far);
        }
    };

    // light that has a direction
    class DirectedLight {
    public:
        Point direction;
        sf::Color color;
        float intensity;

        DirectedLight(const Point& define_direction, const sf::Color& define_color, float define_intensity) {
            this->direction = define_direction;
            this->color = define_color;
            this->intensity = define_intensity;
        }
    };

    // alpha and omega of polygonal 3D engine
    class Triangle {
    public:
        Point vertices[3];
        Point local_normal;
        sf::Color color;

        Triangle(const Point& vertice1, const Point& vertice2, const Point& vertice3, const sf::Color& define_color) {
            this->vertices[0] = vertice1;
            this->vertices[1] = vertice2;
            this->vertices[2] = vertice3;
            this->color = define_color;
            Point edge1 = this->vertices[1] - this->vertices[0];
            Point edge2 = this->vertices[2] - this->vertices[0];
            Point cross_product = edge1.cross_product(edge2);
            this->local_normal = cross_product.normalize();
        }

        // flat shading
        [[nodiscard]] sf::Color calculate_color(const Point& normal, const std::vector<DirectedLight>& lights) const {
            // color of the triangle
            float base_color_r = static_cast<float>(this->color.r) / 255.0f;
            float base_color_g = static_cast<float>(this->color.g) / 255.0f;
            float base_color_b = static_cast<float>(this->color.b) / 255.0f;

            float total_r = 0.0f;
            float total_g = 0.0f;
            float total_b = 0.0f;

            for (const auto& light : lights) {
                float dot = normal * light.direction;
                if (dot < 0.0f) dot = 0.0f;
                float final_intensity = dot * light.intensity;
                float r = static_cast<float>(light.color.r) / 255.0f;
                float g = static_cast<float>(light.color.g) / 255.0f;
                float b = static_cast<float>(light.color.b) / 255.0f;
                total_r += r * r * final_intensity * base_color_r;
                total_g += g * g * final_intensity * base_color_g;
                total_b += b * b * final_intensity * base_color_b;
            }
            std::uint8_t final_r = static_cast<std::uint8_t>(std::clamp(total_r * 255.0f, 0.0f, 255.0f));
            std::uint8_t final_g = static_cast<std::uint8_t>(std::clamp(total_g * 255.0f, 0.0f, 255.0f));
            std::uint8_t final_b = static_cast<std::uint8_t>(std::clamp(total_b * 255.0f, 0.0f, 255.0f));
            return {final_r, final_g, final_b};
        }

        // appends vertices of the triangle to the buffer
        void append_to_buffer(const Matrix& world, const Matrix& view_matrix, const Matrix& projection,
            const Camera& camera, const std::vector<DirectedLight>& lights, sf::VertexArray& buffer) const {
            Point w0 = vertices[0] * world;
            Point w1 = vertices[1] * world;
            Point w2 = vertices[2] * world;

            Point center = (w0 + w1 + w2) * (1.0f / 3.0f);
            Point view = (center - camera.position).normalize();

            // making rotation matrix
            Matrix rotation = world;
            rotation.m[3] = 0;
            rotation.m[7] = 0;
            rotation.m[11] = 0;
            Point world_normal = (this->local_normal * rotation).normalize();

            // culling
            if (world_normal * view > 0.0f) return;

            Point p0 = w0 * view_matrix;
            Point p1 = w1 * view_matrix;
            Point p2 = w2 * view_matrix;
            // inverting z
            p0.z *= -1.0f; p1.z *= -1.0f; p2.z *= -1.0f;

            // if it's really near from the camera, we don't need it
            if (p0.z <= camera.near || p1.z <= camera.near || p2.z <= camera.near) return;

            Point ndc0 = p0 * projection;
            Point ndc1 = p1 * projection;
            Point ndc2 = p2 * projection;

            sf::Color render_color = calculate_color(world_normal, lights);

            for (auto p : {ndc0, ndc1, ndc2}) {
                float prx = (p.x + 1.0f) * 0.5f * static_cast<float>(camera.screen_width);
                float pry = (1.0f - p.y) * 0.5f * static_cast<float>(camera.screen_height);
                buffer.append(sf::Vertex({prx, pry}, render_color));
            }
        }
    };

    // mesh
    class Mesh {
    private:
        // mtl parser (only colors, our 3D engine is simple, remember?)
        static std::map<std::string, sf::Color> parse_mtl(const std::string& mtl_path)   {
            std::map<std::string, sf::Color> materials;
            std::ifstream file(mtl_path);
            if (!file.is_open()) {
                std::cerr << "[Warning] Could not open .mtl file: " << mtl_path << std::endl;
                return materials;
            }

            std::string line;
            std::string current_material;

            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') continue;

                std::istringstream ss(line);
                std::string type;
                ss >> type;

                if (type == "newmtl") {
                    ss >> current_material;
                } else if (type == "Kd" && !current_material.empty()) {
                    float r, g, b;
                    ss >> r >> g >> b;

                    std::uint8_t color_r = static_cast<std::uint8_t>(std::clamp(r * 255.0f, 0.0f, 255.0f));
                    std::uint8_t color_g = static_cast<std::uint8_t>(std::clamp(g * 255.0f, 0.0f, 255.0f));
                    std::uint8_t color_b = static_cast<std::uint8_t>(std::clamp(b * 255.0f, 0.0f, 255.0f));

                    materials[current_material] = sf::Color(color_r, color_g, color_b);
                }
            }
            return materials;
        }

        struct tr_and_depth {
            const Triangle* triangle;
            float depth;
        };
        std::vector<tr_and_depth> sorted;

        size_t polygon_num;
    public:
        Point position = {0, 0, 0};
        float rot_x = 0, rot_y = 0, rot_z = 0;
        float scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f;
        std::vector<Triangle> polygons;

        Mesh(const Point& define_position, float define_rot_x, float define_rot_y, float define_rot_z,
            const std::vector<Triangle>& define_polygons) {
            this->position = define_position;
            this->rot_x = define_rot_x;
            this->rot_y = define_rot_y;
            this->rot_z = define_rot_z;
            this->polygons = define_polygons;
            polygon_num = polygons.size();
        }

        // must be in degrees!
        void rotate(float drx, float dry, float drz) {
            rot_x += drx;
            rot_y += dry;
            rot_z += drz;
        }

        void move(const Point& d) {
            this->position = this->position + d;
        }

        void scale(float sx, float sy, float sz) {
            this->scale_x = sx;
            this->scale_y = sy;
            this->scale_z = sz;
        }

        // to don't change the original mesh (and optimize hehehe)
        [[nodiscard]] Matrix get_world() const {
            Matrix scale = Matrix::scale(scale_x, scale_y, scale_z);
            Matrix rotation = Matrix::rotation_z(rot_z * TO_RAD) * Matrix::rotation_y(rot_y * TO_RAD) * Matrix::rotation_x(rot_x * TO_RAD);
            Matrix translation = Matrix::translation(position.x, position.y, position.z);

            return scale * rotation * translation;
        }

        // render...
        void render(sf::RenderWindow& window, const Camera& camera, const std::vector<DirectedLight>& lights) {
            // calculating matrices
            Matrix world = get_world();
            Matrix view_matrix = camera.view_matrix();
            Matrix projection = camera.projection();
            sf::VertexArray buffer(sf::PrimitiveType::Triangles);

            sorted.resize(polygon_num);

            auto worker = [&](size_t start_i, size_t end_i) {
                for (size_t i = start_i; i < end_i; i++) {
                    const Triangle& tr = polygons[i];
                    Point w0 = tr.vertices[0] * world;
                    Point w1 = tr.vertices[1] * world;
                    Point w2 = tr.vertices[2] * world;

                    // Painter's algorithm (yup I know Bob Ross draws faster)
                    Point center = (w0 + w1 + w2) * (1.0f / 3.0f);
                    float dist = (center - camera.position).modulus();
                    sorted[i] = {&tr, dist};
                }
            };

            // THREEEEAAAADS
            std::vector<std::jthread> threads;
            size_t chunk = polygon_num / THREADS_NUM;

            for (size_t i = 0; i < THREADS_NUM; i++) {
                size_t start_i = i * chunk;
                size_t end_i = i == THREADS_NUM - 1 ? polygon_num : start_i + chunk;
                if (start_i < end_i) threads.emplace_back(worker, start_i, end_i);
            }

            threads.clear();

            std::sort(sorted.begin(), sorted.end(), [](const tr_and_depth& a, const tr_and_depth& b) {
                return a.depth > b.depth;
            });
            for (const auto& tr : sorted) {
                tr.triangle->append_to_buffer(world, view_matrix, projection, camera, lights, buffer);
            }

            window.draw(buffer);
        }

        // .obj parser (oh no...)
        explicit Mesh(const std::string& obj_path) {
            std::ifstream file(obj_path);
            // for situations when someone wants to play tricks on me
            if (!file.is_open()) {
                std::cerr << "[Error] Failed to open .obj file: " << obj_path << std::endl;
                return;
            }
            std::vector<Point> vertices;
            std::map<std::string, sf::Color> materials;
            sf::Color current_color = sf::Color::White;
            std::string line;
            std::string mtl;

            // first while: searching mtl file ans parse it
            while (std::getline(file, line)) {
                if (line.rfind("mtllib ", 0) == 0) {
                    std::istringstream ss(line);
                    std::string prefix;
                    ss >> prefix >> mtl;
                    std::string dir;
                    size_t pos = obj_path.find_last_of("/\\");
                    if (pos != std::string::npos) {
                        dir = obj_path.substr(0, pos + 1);
                    }
                    materials = parse_mtl(dir + mtl);
                    break;
                }
            }

            file.clear();
            file.seekg(0, std::ios::beg);

            // second while: PARSING
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') continue;
                std::istringstream ss(line);
                std::string type;
                ss >> type;

                if (type == "v") {
                    float x, y, z;
                    ss >> x >> y >> z;
                    vertices.push_back(Point(x, y, z));
                } else if (type == "usemtl") {
                    std::string mat_name;
                    ss >> mat_name;
                    if (materials.find(mat_name) != materials.end()) {
                        current_color = materials[mat_name];
                    }
                } else if (type == "f") {
                    std::vector<int> face_indices;
                    std::string vertex_block;

                    while (ss >> vertex_block) {
                        size_t slash_pos = vertex_block.find('/');
                        std::string index_str = slash_pos == std::string::npos ? vertex_block : vertex_block.substr(0, slash_pos);
                        int idx = std::stoi(index_str);
                        if (idx < 0) {
                            idx = static_cast<int>(vertices.size()) + idx + 1;
                        }
                        face_indices.push_back(idx);
                    }

                    for (size_t i = 1; i < face_indices.size() - 1; i++) {
                        int idx0 = face_indices[0] - 1;
                        int idx1 = face_indices[i] - 1;
                        int idx2 = face_indices[i + 1] - 1;
                        if (idx0 < vertices.size() &&
                            idx1 < vertices.size() &&
                            idx2 < vertices.size())
                        {
                            this->polygons.push_back(Triangle(
                                vertices[idx0],
                                vertices[idx1],
                                vertices[idx2],
                                current_color
                            ));
                        }
                    }
                }
            }
            polygon_num = polygons.size();
        }
    };
} // namespace bst
