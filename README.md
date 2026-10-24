# Basit – 3D Engine
Welcome to my second project on GitHub – my 3D Engine named Basit. In Arabic language, "basit" means "simple". So this project is SUPER SIMPLE (so simple that it's a CPU engine).

This engine can draw 3D objects and parse `.obj` files. It uses flat shading, culling optimization, Painter's algorithm and other stuff.

Some screenshots:

<img width="912" height="744" alt="Screenshot of a cube rendered by Basit Engine" src="https://github.com/user-attachments/assets/ed09f383-e44a-4461-a0ac-7e0a13d2f705" />

<img width="912" height="744" alt="Screenshot of Utah's Teapot rendered by Basit Engine" src="https://github.com/user-attachments/assets/f714ad48-c0d8-4879-a9c2-b7e075d18adb" />

## How to use this thing?
It's as simple as the project.

### `Point` class
Standard 3D point/vector class. It has three variables: `float x`, `float y`, `float z`.

Operations:

- `Point operator+(const Point& other)`: returns component-wise sum of two `Points`;
- `Point operator-(const Point& other)`: returns component-wise difference of two `Points`;
- `float operator*(const Point& other)`: returns dot product of two `Points`;
- `Point operator*(float k)`: returns original `Point` multiplied by a float value;

Methods:

- `float modulus()`: returns modulus (magnitude/length) of vector;
- `Point normalize()`: returns normalized (unit) vector made from the original one;
- `Point cross_product(const Point& other)`: returns cross product of two `Points`;
- `Point rotate(float deg_x, float deg_y, float deg_z)`: returns rotated `Point`, angles must be in degrees.

Code example:
```c++

// initialization
Point p(12.0f, 0.0f, 0.0f);

// some operations with vector p
Point q(1.0f, 2.0f, 0.0f);
Point v1 = q + p;
std::cout << v1.x << std::endl; // 13

Point v2 = q - p;
std::cout << v2.x << std::endl; // -11

Point p3 = p * 3.0f;
std::cout << p3.x << std::endl; // 36

float dot_product = p * q;
std::cout << dot_product << std::endl; // 12

// length of vector p
float magnitude = p.modulus();
std::cout << "Magnitude of the vector: " << magnitude << std::endl; // Magnitude of the vector: 12

// normalizing the vector
Point normalized_p = p.normalize();
std::cout << normalized_p.x << std::endl; // 1

// rotating the cube around Oy by 180 degrees
Point rotated_p = p.rotate(0.0f, 180.0f, 0.0f);
std::cout << rotated_p.x << std::endl; // -12

// cross product of {1, 2, 3} and {4, 5, 6}
Point a(1.0f, 2.0f, 3.0f);
Point b(4.0f, 5.0f, 6.0f);
Point c = a.cross_product(b);
std::cout << c.z << std::endl; // -3
```

### `Camera` class

### `DirectedLight` class

### `Triangle` class

### `Mesh` class

## Credits
Everything is mine, except SFML – a library I used to do not write additional thousands lines of code.
