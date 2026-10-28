# Basit – 3D Engine
Welcome to my second project on GitHub – my 3D Engine named Basit. In Arabic language, "basit" means "simple". So this project is SUPER SIMPLE (so simple that it's a CPU engine).

This engine can draw 3D objects and parse `.obj` files. It uses flat shading, culling optimization, Painter's algorithm and other stuff.

Some screenshots:

<img width="912" height="744" alt="Screenshot of 3D Cube rendered using Basit Engine" src="https://github.com/user-attachments/assets/71d82d07-6005-415f-b1bc-704e6fff193b" title="hello world!"/>

<img width="912" height="744" alt="Screenshot of Utah's Teapot rendered using Basit Engine" src="https://github.com/user-attachments/assets/7f0abaf2-bab1-41f9-b06f-6868a1903a73" title="classic"/>

<img width="912" height="744" alt="Screenshot of Jungle Mario Kart level rendered using Basit Engine" src="https://github.com/user-attachments/assets/46425f01-765e-4762-91a9-1d7094f1215f" title="this thing made my processor die"/>

## How to use this thing?
It's as simple as the project. Before working with Basit, you must install SFML – Simple and Fast Multimedia Library (should be version 3.0.0 or newer), here are the links where you can download it:

- https://www.sfml-dev.org/download/
- https://github.com/SFML/SFML

After that don't forget to include Basit itself:

`#include "basit.hpp"`

Namespace of the engine is `bst`. You can check `main.cpp` for example of code or read a little documentation below.

### `Point` class
Standard 3D point/vector class. It has three variables: `float x`, `float y`, `float z`.

Operations:

- `Point operator+(const Point& other)`: returns component-wise sum of two `Points`;
- `Point operator-(const Point& other)`: returns component-wise difference of two `Points`;
- `float operator*(const Point& other)`: returns dot product of two `Points`;
- `Point operator*(float k)`: returns original `Point` multiplied by a float value.

Methods:

- `float modulus()`: returns modulus (magnitude/length) of vector;
- `Point normalize()`: returns normalized (unit) vector made from the original one;
- `Point cross_product(const Point& other)`: returns cross product of two `Points`;
- `Point rotate(float deg_x, float deg_y, float deg_z)`: returns rotated `Point`, angles must be in degrees.

Code example:
```c++

// initialization
bst::Point p(12.0f, 0.0f, 0.0f);

// some operations with vector p
bst::Point q(1.0f, 2.0f, 0.0f);
bst::Point v1 = q + p;
std::cout << v1.x << std::endl; // 13

bst::Point v2 = q - p;
std::cout << v2.x << std::endl; // -11

bst::Point p3 = p * 3.0f;
std::cout << p3.x << std::endl; // 36

float dot_product = p * q;
std::cout << dot_product << std::endl; // 12

// length of vector p
float magnitude = p.modulus();
std::cout << "Magnitude of the vector: " << magnitude << std::endl; // Magnitude of the vector: 12

// normalizing the vector
bst::Point normalized_p = p.normalize();
std::cout << normalized_p.x << std::endl; // 1

// rotating the cube around Oy by 180 degrees
bst::Point rotated_p = p.rotate(0.0f, 180.0f, 0.0f);
std::cout << rotated_p.x << std::endl; // -12

// cross product of {1, 2, 3} and {4, 5, 6}
bst::Point a(1.0f, 2.0f, 3.0f);
bst::Point b(4.0f, 5.0f, 6.0f);
bst::Point c = a.cross_product(b);
std::cout << c.z << std::endl; // -3
```

### `Camera` class
That's a class that projects `Points` onto a 2D canvas. It contains:

Varibales:
- `Point position`: position of the camera;
- `float rot_x, float rot_y, float rot_z`: rotation of the camera (direction);
- `float fov`: field of view;
- `int screen_width, int screen_height`: width and height of the canvas.

Methods:
- `sf::Vector2f project(const Point& point)`: returns `x` and `y` coordinates of `Point's` projection; if the `Point` is behind the camera, it returns `{-1, -1}`; `{0, 0}` is center of the canvas.

Code example:
```c++
// width and height of the canvas
const int WIDTH = 800, HEIGHT = 600;

// just a random point
bst::Point p(10.0f, -10.0f, 100.0f);

// creating the camera
bst::Point camera_pos(0.0f, 0.0f, 50.0f);
bst::Camera camera(camera_pos, 0.0f, 0.0f, 0.0f, 400.0f, WIDTH, HEIGHT);

// projecting the camera
sf::Vector2f prj = camera.project(point);
std::cout << prj.x << ' ' << prj.y << std::endl; // 480 220
```

### `DirectedLight` class

### `Triangle` class

### `Mesh` class

## Credits
Everything is mine, except SFML, of course =).
