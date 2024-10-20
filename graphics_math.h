#include <stdint.h>

# define PI_32 3.14159265358979323846f

class Vec2 {
    public:
        float x;
        float y;

        static Vec2 Create(float x, float y);
};

class Vec3 {
    public:
        float x;
        float y;
        float z;

        static Vec3 Create(float x, float y, float z);
};

class Vec4 {
    public:
        float x;
        float y;
        float z;
        float w;
    
        static Vec4 Create(float x, float y, float z, float w);
        static Vec4 FromPoint(Vec3 Point);
        Vec3 ToVec3();
};

class Mat4 {
    public:
        Vec4 v[4];

        static Mat4 Identity();
        static Mat4 ScaleMatrix(float x, float y, float z);
        static Mat4 TranslationMatrix(float x, float y, float z);
        static Mat4 RotationMatrix(float x, float y, float z);
};

Vec3 operator+(Vec3 A, Vec3 B);
Mat4 operator*(Mat4 A, Mat4 B);

Vec2 ProjectPoint(Vec3 Pos, uint32_t Width, uint32_t Height);
void DrawTriangle(
    Vec3 Point0,
    Vec3 Point1,
    Vec3 Point2,
    Vec3 Color0,
    Vec3 Color1,
    Vec3 Color2,
    uint32_t *Pixels,
    uint32_t Width,
    uint32_t Height,
    float *DepthBuffer,
    Mat4 Transform);