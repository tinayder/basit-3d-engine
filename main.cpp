/*
 * 3D ENGINE
 *  ______      _______    _______    _______    _______
 * |      |    |       |  |  _____|  |__   __|  |__   __|  Alpha Version
 * |  [ ] |_   |  [ ]  |  |  |____      | |        | |
 * |        |  |   _   |  |____   |     | |        | |
 * |  [ ]   |  |  | |  |   ____|  |   __| |__      | |
 * |________|  |__| |__|  |_______|  |_______|     |_|
 *
 * [ arab. basit – "simple" ]
 * (c) by tinayder
 */

#include <vector>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

const float PI = 3.14159265f;
const float MOUSE_SENSITIVITY = 0.4f;
const float TO_RAD = PI / 180.0f;
const int WIDTH = 800, HEIGHT = 600;

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

    // input parameters must be in degrees only
    [[nodiscard]] Point rotate(float deg_x, float deg_y, float deg_z) const {
        // converting to radians
        float rad_x = deg_x * TO_RAD;
        float rad_y = deg_y * TO_RAD;
        float rad_z = deg_z * TO_RAD;
        // x-rotation
        float x1 = this->x;
        float y1 = this->y * std::cos(rad_x) - this->z * std::sin(rad_x);
        float z1 = this->y * std::sin(rad_x) + this->z * std::cos(rad_x);
        // y-rotation
        float x2 = x1 * std::cos(rad_y) + z1 * std::sin(rad_y);
        float y2 = y1;
        float z2 = z1 * std::cos(rad_y) - x1 * std::sin(rad_y);
        // z-rotation
        float x3 = x2 * std::cos(rad_z) - y2 * std::sin(rad_z);
        float y3 = x2 * std::sin(rad_z) + y2 * std::cos(rad_z);
        float z3 = z2;
        return {x3, y3, z3};
    }
};

// camera
class Camera {
public:
    Point position;
    float rot_x, rot_y, rot_z;
    float fov;
    int screen_width, screen_height;

    Camera(const Point& define_position, float define_rot_x, float define_rot_y, float define_rot_z, float define_fov, int define_screen_width, int define_screen_height) {
        this->position = define_position;
        this->rot_x = define_rot_x;
        this->rot_y = define_rot_y;
        this->rot_z = define_rot_z;
        this->fov = define_fov;
        this->screen_width = define_screen_width;
        this->screen_height = define_screen_height;
    }

    [[nodiscard]] sf::Vector2f project(const Point& point) const {
        Point local = point - this->position;
        Point rotated = local.rotate(-rot_x, -rot_y, -rot_z);
        // if the point is behind the camera, we don't need it
        if (rotated.z <= 0) return {-1, -1};
        float prx = rotated.x, pry = rotated.y;
        prx /= rotated.z;
        pry /= rotated.z;
        prx *= fov;
        pry *= fov;
        // add half of screen parameters to make (0, 0) in the center
        prx += (float)screen_width / 2.0f;
        pry += (float)screen_height / 2.0f;
        return {prx, pry};
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
    Point normal;
    sf::Color color;

    Triangle(const Point& vertice1, const Point& vertice2, const Point& vertice3, const sf::Color& define_color) {
        this->vertices[0] = vertice1;
        this->vertices[1] = vertice2;
        this->vertices[2] = vertice3;
        this->color = define_color;
        Point edge1 = this->vertices[1] - this->vertices[0];
        Point edge2 = this->vertices[2] - this->vertices[0];
        Point cross_product = edge1.cross_product(edge2);
        this->normal = cross_product.normalize();
    }

    // for light
    [[nodiscard]] sf::Color calculate_color(const DirectedLight& light) const {
        float dot = normal * light.direction;
        if (dot < 0.0f) dot = 0.0f;
        float final_intensity = dot * light.intensity;
        float r = (float)this->color.r / 255.0f * ((float)light.color.r / 255.0f) * final_intensity * 255.0f;
        float g = (float)this->color.g / 255.0f * ((float)light.color.g / 255.0f) * final_intensity * 255.0f;
        float b = (float)this->color.b / 255.0f * ((float)light.color.b / 255.0f) * final_intensity * 255.0f;
        std::uint8_t final_r = static_cast<std::uint8_t>(std::clamp(r, 0.0f, 255.0f));
        std::uint8_t final_g = static_cast<std::uint8_t>(std::clamp(g, 0.0f, 255.0f));
        std::uint8_t final_b = static_cast<std::uint8_t>(std::clamp(b, 0.0f, 255.0f));
        return {final_r, final_g, final_b};
    }

    // for a lot of lights
    [[nodiscard]] sf::Color calculate_color(const std::vector<DirectedLight>& lights) const {
        float total_r = 0.0f;
        float total_g = 0.0f;
        float total_b = 0.0f;

        for (const auto& light : lights) {
            float dot = normal * light.direction;
            if (dot < 0.0f) dot = 0.0f;
            float final_intensity = dot * light.intensity;
            total_r += (float)this->color.r / 255.0f * ((float)light.color.r / 255.0f) * final_intensity;
            total_g += (float)this->color.g / 255.0f * ((float)light.color.g / 255.0f) * final_intensity;
            total_b += (float)this->color.b / 255.0f * ((float)light.color.b / 255.0f) * final_intensity;
        }
        std::uint8_t final_r = static_cast<std::uint8_t>(std::clamp(total_r * 255.0f, 0.0f, 255.0f));
        std::uint8_t final_g = static_cast<std::uint8_t>(std::clamp(total_g * 255.0f, 0.0f, 255.0f));
        std::uint8_t final_b = static_cast<std::uint8_t>(std::clamp(total_b * 255.0f, 0.0f, 255.0f));
        return {final_r, final_g, final_b};
    }

    // for Painter's algorithm
    [[nodiscard]] float average_depth(const Point& camera_position) const {
        Point center = (this->vertices[0] + this->vertices[1] + this->vertices[2]) * (1.0f / 3.0f);
        float result = (center - camera_position).modulus();
        return result;
    }

    // render the triangle on the screen
    void render(sf::RenderWindow& window, const Camera& camera, const DirectedLight& light) const {
        sf::Color render_color = calculate_color(light);
        sf::Vector2f p0 = camera.project(vertices[0]);
        sf::Vector2f p1 = camera.project(vertices[1]);
        sf::Vector2f p2 = camera.project(vertices[2]);
        // checking if the point is not visible
        if (p0 == sf::Vector2f(-1.0f, -1.0f) ||
        p1 == sf::Vector2f(-1.0f, -1.0f) ||
        p2 == sf::Vector2f(-1.0f, -1.0f)) return;
        sf::VertexArray va(sf::PrimitiveType::Triangles, 3);
        va[0].position = p0;
        va[1].position = p1;
        va[2].position = p2;
        va[0].color = render_color;
        va[1].color = render_color;
        va[2].color = render_color;
        window.draw(va);
    }

    // for a lot of lights
    void render(sf::RenderWindow& window, const Camera& camera, const std::vector<DirectedLight>& lights) const {
        sf::Color render_color = calculate_color(lights);
        sf::Vector2f p0 = camera.project(vertices[0]);
        sf::Vector2f p1 = camera.project(vertices[1]);
        sf::Vector2f p2 = camera.project(vertices[2]);
        // checking if the point is not visible
        if (p0 == sf::Vector2f(-1.0f, -1.0f) ||
        p1 == sf::Vector2f(-1.0f, -1.0f) ||
        p2 == sf::Vector2f(-1.0f, -1.0f)) return;
        sf::VertexArray va(sf::PrimitiveType::Triangles, 3);
        va[0].position = p0;
        va[1].position = p1;
        va[2].position = p2;
        va[0].color = render_color;
        va[1].color = render_color;
        va[2].color = render_color;
        window.draw(va);
    }
};

// mesh
class Mesh {
private:
    // mtl parser (only colors, our 3D engine is simple, remember?)
    std::map<std::string, sf::Color> parse_mtl(const std::string& mtl_path) const  {
        std::map<std::string, sf::Color> materials;
        std::ifstream file(mtl_path);
        if (!file.is_open()) {
            std::cerr << "[Warning] Could not open .mtl file: " << mtl_path << std::endl;
            return materials;
        }

        std::string line;
        std::string current_material = "";

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
public:
    Point position = {0, 0, 0};
    float rot_x = 0, rot_y = 0, rot_z = 0;
    std::vector<Triangle> polygons;

    Mesh(const Point& define_position, float define_rot_x, float define_rot_y, float define_rot_z, const std::vector<Triangle>& define_polygons) {
        this->position = define_position;
        this->rot_x = define_rot_x;
        this->rot_y = define_rot_y;
        this->rot_z = define_rot_z;
        this->polygons = define_polygons;
        for (Triangle& tr : this->polygons) {
            for (Point& vertex : tr.vertices) {
                vertex = vertex + this->position;
                vertex = vertex.rotate(rot_x, rot_y, rot_z);
            }
        }
    }

    void rotate(float deg_x, float deg_y, float deg_z) {
        rot_x += deg_x;
        rot_y += deg_y;
        rot_z += deg_z;
        for (Triangle& tr : this->polygons) {
            for (Point& vertex : tr.vertices) {
                vertex = vertex.rotate(deg_x, deg_y, deg_z);
            }
            Point edge1 = tr.vertices[1] - tr.vertices[0];
            Point edge2 = tr.vertices[2] - tr.vertices[0];
            Point cross_product = edge1.cross_product(edge2);
            tr.normal = cross_product.normalize();
        }
    }

    void move(const Point& d) {
        this->position = this->position + d;
        for (Triangle& tr : this->polygons) {
            for (Point& vertex : tr.vertices) {
                vertex = vertex + d;
            }
            Point edge1 = tr.vertices[1] - tr.vertices[0];
            Point edge2 = tr.vertices[2] - tr.vertices[0];
            Point cross_product = edge1.cross_product(edge2);
            tr.normal = cross_product.normalize();
        }
    }

    // Painter's algorithm (yup I know Bob Ross draws faster)
    void render(sf::RenderWindow& window, const Camera& camera, const DirectedLight& light) const {
        // some optimizations to don't calculate depths while sorting
        struct tr_and_depth {
            const Triangle* triangle;
            float depth;
        };

        std::vector<tr_and_depth> sorted;
        sorted.reserve(polygons.size());
        for (const Triangle& tr : polygons) {
            Point view_vector = tr.vertices[0] - camera.position;
            float culling = tr.normal * view_vector;
            if (culling >= 0.0f) {
                float dist = tr.average_depth(camera.position);
                sorted.push_back({&tr, dist});
            }
        }

        std::sort(sorted.begin(), sorted.end(), [&camera](const tr_and_depth a, const tr_and_depth b) {
            return a.depth > b.depth;
        });
        for (const tr_and_depth tr : sorted) {
            tr.triangle->render(window, camera, light);
        }
    }

    void render(sf::RenderWindow& window, const Camera& camera, const std::vector<DirectedLight>& lights) const {
        std::vector<const Triangle*> sorted;
        sorted.reserve(polygons.size());
        for (const auto& tr : polygons) {
            Point view_vector = tr.vertices[0] - camera.position;
            float culling = tr.normal * view_vector;
            if (culling >= 0.0f) {
                sorted.push_back(&tr);
            }
        }
        std::sort(sorted.begin(), sorted.end(), [&camera](const Triangle* a, const Triangle* b) {
            return a->average_depth(camera.position) > b->average_depth(camera.position);
        });
        for (const Triangle* tr : sorted) {
            tr->render(window, camera, lights);
        }
    }

    // .obj parser (oh no...)
    Mesh(const std::string& obj_path) {
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
        std::string mtl = "";

        // first while: searching mtl file ans parse it
        while (std::getline(file, line)) {
            if (line.rfind("mtllib ", 0) == 0) {
                std::istringstream ss(line);
                std::string prefix;
                ss >> prefix >> mtl;
                std::string dir = "";
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
                            vertices[idx2],
                            vertices[idx1],
                            current_color
                        ));
                    }
                }
            }
        }
    }
};

int main() {
    // test: a cube that you can rotate by mouse
    // UPD: now you rotate the camera
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "3D Cube Test - Basit Engine");
    window.setFramerateLimit(60);

    float camera_yaw = 0.0f;
    float camera_pitch = 0.0f;
    float radius = 300.0f;
    Point camera_pos(0.0f, 0.0f, -radius);
    Camera camera(camera_pos, 0.0f, 0.0f, 0.0f, 400.0f, WIDTH, HEIGHT);
    std::vector<DirectedLight> lights;

    // two lights to don't see a shadow from the other side
    lights.push_back(DirectedLight(Point(0.25f, 0.25f, 1.0f).normalize(), sf::Color::White, 1.0f));
    lights.push_back(DirectedLight(Point(-0.25f, -0.25f, -1.0f).normalize(), sf::Color::White, 1.0f));

    // some rain dancing for my CLion compiler
    std::string path_str = std::filesystem::current_path().string();
    size_t pos = path_str.find("cmake-build-debug");
    std::string path = (pos != std::string::npos ? path_str.substr(0, pos) : path_str) + "models/cube.obj";

    // *insert epic gif of a guy making a cube in blender*

    Mesh cube(path);

    // *the gif has ended*

    bool is_dragging = false;
    sf::Vector2i last_mouse_pos;

    // the main cycle
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    is_dragging = true;
                    last_mouse_pos = sf::Mouse::getPosition(window);
                }
            }

            if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseReleased->button == sf::Mouse::Button::Left) {
                    is_dragging = false;
                }
            }

            if (event->is<sf::Event::MouseMoved>()) {
                if (is_dragging) {
                    sf::Vector2i current_mouse_pos = sf::Mouse::getPosition(window);
                    float delta_x = static_cast<float>(current_mouse_pos.x - last_mouse_pos.x);
                    float delta_y = static_cast<float>(current_mouse_pos.y - last_mouse_pos.y);

                    // Don't rotate the cube, but walk around it
                    // – Confucius
                    camera_yaw += delta_x * MOUSE_SENSITIVITY;
                    camera_pitch += -delta_y * MOUSE_SENSITIVITY;

                    last_mouse_pos = current_mouse_pos;
                }
            }
        }

        // rotating the camera
        Point yaw_only = camera_pos.rotate(0.0f, camera_yaw, 0.0f);
        camera.position = yaw_only.rotate(camera_pitch, 0.0f, 0.0f);
        camera.rot_x = camera_pitch;
        camera.rot_y = camera_yaw;
        camera.rot_z = 0.0f;

        window.clear(sf::Color::Black);

        cube.render(window, camera, lights);

        window.display();
    }

    return 0;
}
