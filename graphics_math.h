#include <stdint.h>

# define PI_32 3.14159265358979323846f

class Vec2 {
    public:
        float x;
        float y;

        static Vec2 create(float x, float y);
};

class Vec3 {
    public:
        float x;
        float y;
        float z;

        static Vec3 create(float x, float y, float z);
};

Vec3 operator+(Vec3 A, Vec3 B);

Vec2 ProjectPoint(Vec3 Pos, uint32_t Width, uint32_t Height);
void DrawTriangle(Vec3 *Points, uint32_t Color, uint32_t *Pixels, uint32_t Width, uint32_t Height);