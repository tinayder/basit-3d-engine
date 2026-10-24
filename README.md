# Basit – 3D Engine
Welcome to my second project on GitHub – my 3D Engine named Basit. In Arabic language, "basit" means "simple". So this project is SUPER SIMPLE (so simple that it's a CPU engine).

This engine can draw 3D objects and parse `.obj` files. It uses flat shading, culling optimization, Painter's algorithm and other stuff.

Some screenshots:

<img width="912" height="744" alt="Снимок экрана — 2026-10-23 в 19 16 44" src="https://github.com/user-attachments/assets/ed09f383-e44a-4461-a0ac-7e0a13d2f705" />

<img width="912" height="744" alt="Снимок экрана — 2026-10-23 в 19 11 02" src="https://github.com/user-attachments/assets/f714ad48-c0d8-4879-a9c2-b7e075d18adb" />

## How to use this thing?
It's as simple as the project.

### `Point` class
Standard 3D point/vector class. It has three variables: `float x`, `float y`, `float z`;

You can add `Point` to other one, subtract them and do dot product. Also you can multiply `Point` by `float`.

Methods:

- `modulus()`: returns modulus (magnitude/length) of vector;
- `normalize()`: returns normalized (unit) vector made from the original one;
- `cross_product(const Point& other)`: returns cross product of two `Points`;
- `rotate(float deg_x, float deg_y, float deg_z)`: return rotated `Point`; all angles must be in degrees!

### `Camera` class

### `DirectedLight` class

### `Triangle` class

### `Mesh` class

## Credits
Everything is mine, except SFML – a library I used to do not write additional thousands lines of code.
