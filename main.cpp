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
#include <cstdint>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

const float PI = 3.14159265f;
const float TO_RAD = PI / 180.0f;

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

    // for shadows
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

    // render the triangle on the screen
    void render(sf::RenderWindow& window, const Camera& camera, const DirectedLight& light) const {
        Point view_vector = vertices[0] - camera.position;
        float culling = normal * view_vector;
        if (culling < 0.0f) return;
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
};

int main() {
    // test: a cube that you can rotate by mouse
    sf::RenderWindow window(sf::VideoMode({800, 600}), "3D Cube Test");
    window.setFramerateLimit(60);

    Camera camera({0.0f, 0.0f, -300.0f}, 0.0f, 0.0f, 0.0f, 400.0f, 800, 600);
    DirectedLight light(Point(0.25f, 0.25f, 1.0f).normalize(), sf::Color::White, 1.0f);

    // *insert epic gif of a guy making a cube in blender*
    float init_x = 30.0f;
    float init_y = 45.0f;
    float init_z = 0.0f;

    Point v0 = Point(-50.0f, -50.0f, -50.0f).rotate(init_x, init_y, init_z);
    Point v1 = Point( 50.0f, -50.0f, -50.0f).rotate(init_x, init_y, init_z);
    Point v2 = Point( 50.0f,  50.0f, -50.0f).rotate(init_x, init_y, init_z);
    Point v3 = Point(-50.0f,  50.0f, -50.0f).rotate(init_x, init_y, init_z);
    Point v4 = Point(-50.0f, -50.0f,  50.0f).rotate(init_x, init_y, init_z);
    Point v5 = Point( 50.0f, -50.0f,  50.0f).rotate(init_x, init_y, init_z);
    Point v6 = Point( 50.0f,  50.0f,  50.0f).rotate(init_x, init_y, init_z);
    Point v7 = Point(-50.0f,  50.0f,  50.0f).rotate(init_x, init_y, init_z);

    // *the gif has ended*

    std::vector<Triangle> cube;

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

                    float rot_step_x = delta_y * 0.3f;
                    float rot_step_y = -delta_x * 0.3f;

                    v0 = v0.rotate(rot_step_x, rot_step_y, 0.0f);
                    v1 = v1.rotate(rot_step_x, rot_step_y, 0.0f);
                    v2 = v2.rotate(rot_step_x, rot_step_y, 0.0f);
                    v3 = v3.rotate(rot_step_x, rot_step_y, 0.0f);
                    v4 = v4.rotate(rot_step_x, rot_step_y, 0.0f);
                    v5 = v5.rotate(rot_step_x, rot_step_y, 0.0f);
                    v6 = v6.rotate(rot_step_x, rot_step_y, 0.0f);
                    v7 = v7.rotate(rot_step_x, rot_step_y, 0.0f);

                    last_mouse_pos = current_mouse_pos;
                }
            }
        }

        cube.clear();

        cube.push_back(Triangle(v0, v1, v2, sf::Color::Red));
        cube.push_back(Triangle(v0, v2, v3, sf::Color::Red));

        cube.push_back(Triangle(v5, v4, v7, sf::Color::Blue));
        cube.push_back(Triangle(v5, v7, v6, sf::Color::Blue));

        cube.push_back(Triangle(v4, v0, v3, sf::Color::Yellow));
        cube.push_back(Triangle(v4, v3, v7, sf::Color::Yellow));

        cube.push_back(Triangle(v1, v5, v6, sf::Color::Green));
        cube.push_back(Triangle(v1, v6, v2, sf::Color::Green));

        cube.push_back(Triangle(v4, v5, v1, sf::Color::Cyan));
        cube.push_back(Triangle(v4, v1, v0, sf::Color::Cyan));

        cube.push_back(Triangle(v3, v2, v6, sf::Color::Magenta));
        cube.push_back(Triangle(v3, v6, v7, sf::Color::Magenta));

        window.clear(sf::Color::Black);
        for (const auto& triangle : cube) {
            triangle.render(window, camera, light);
        }
        window.display();
    }

    return 0;
}
