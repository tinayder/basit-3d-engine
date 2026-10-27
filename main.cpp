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
 * Basit 3D Engine v1.5.1 – Code Example
 * [ arabic: basit - "simple" ]
 * (c) 2026 tinayder - MIT License
 * https://github.com/tinayder/basit-3d-engine
 */

#include "basit.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <filesystem>

const int WIDTH = 800, HEIGHT = 600;
const float MOUSE_SENSITIVITY = 0.7f;

int main() {
    // test: a cube that you can rotate by mouse
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "3D Cube Test - Basit Engine");
    window.setFramerateLimit(60);

    bst::Point camera_pos(0.0f, 0.0f, 300.0f);
    bst::Camera camera(camera_pos, 0.0f, 0.0f, 0.0f, 0.1f, 500.0f, 50.0f, WIDTH, HEIGHT);
    std::vector<bst::DirectedLight> lights;

    // two lights to don't see a shadow from the other side
    lights.push_back(bst::DirectedLight(bst::Point(0.25f, 0.25f, 1.0f).normalize(), sf::Color::White, 1.0f));
    lights.push_back(bst::DirectedLight(bst::Point(-0.25f, -0.25f, -1.0f).normalize(), sf::Color::White, 1.0f));

    // some rain dancing for my CLion compiler
    std::string path_str = std::filesystem::current_path().string();
    size_t pos = path_str.find("cmake-build-debug");
    std::string path = (pos != std::string::npos ? path_str.substr(0, pos) : path_str) + "assets/cube.obj";

    // *insert epic gif of a guy making a cube in blender*

    bst::Mesh cube(path);

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
                    auto delta_x = static_cast<float>(current_mouse_pos.x - last_mouse_pos.x);
                    auto delta_y = static_cast<float>(current_mouse_pos.y - last_mouse_pos.y);

                    // Don't walk around the cube, just rotate it
                    // – Confucius
                    cube.rotate(delta_y * MOUSE_SENSITIVITY, delta_x * MOUSE_SENSITIVITY, 0.0f);

                    last_mouse_pos = current_mouse_pos;
                }
            }
        }

        window.clear(sf::Color::Black);

        cube.render(window, camera, lights);

        window.display();
    }

    return 0;
}
