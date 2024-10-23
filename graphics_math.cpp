#include "graphics_math.h"
#include <cmath>
#include <algorithm>

Vec2 Vec2::Create(float x, float y) {
    Vec2 v = {};
    v.x = x;
    v.y = y;
    return v;
}

Vec2 operator+(Vec2 A, Vec2 B)
{
    return Vec2::Create(A.x + B.x, A.y + B.y);
}

Vec2 operator-(Vec2 A, Vec2 B)
{
    return Vec2::Create(A.x - B.x, A.y - B.y);
}

Vec2 operator*(Vec2 A, Vec2 B)
{
    return Vec2::Create(A.x * B.x, A.y * B.y);
}

Vec2 operator*(float B, Vec2 A)
{
    return Vec2::Create(A.x * B, A.y * B);
}

Vec2 operator/(Vec2 A, float B)
{
    return Vec2::Create(A.x / B, A.y / B);
}

Vec3 Vec3::Create(float x, float y, float z) {
    Vec3 v = {};
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vec3 Vec3::Normalize()
{
    float Len = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    return Vec3::Create(this->x, this->y, this->z) / Len;
}

Vec3 operator+(Vec3 A, Vec3 B)
{
    return Vec3::Create(A.x + B.x, A.y + B.y, A.z + B.z);
}

Vec3 operator-(Vec3 A, Vec3 B)
{
    return Vec3::Create(A.x - B.x, A.y - B.y, A.z - B.z);
}

Vec3 operator-(Vec3 A)
{
    return Vec3::Create(-A.x, -A.y, -A.z);
}

Vec3 operator*(float A, Vec3 B)
{
    return Vec3::Create(B.x * A, B.y * A, B.z * A);
}

Vec3 operator*(Vec3 A, float B)
{
    return Vec3::Create(A.x * B, A.y * B, A.z * B);
}

Vec3 operator/(Vec3 A, float B)
{
    return Vec3::Create(A.x / B, A.y / B, A.z / B);
}

Vec3 operator+=(Vec3 &A, Vec3 B)
{
    A = A + B;
    return A;
}

Vec3 operator-=(Vec3 &A, Vec3 B)
{
    A = A - B;
    return A;
}

Vec2 ProjectPoint(Vec3 Pos, uint32_t Width, uint32_t Height)
{
    Vec2 Result = Vec2::Create(Pos.x, Pos.y) / Pos.z;
    Result = 0.5 * (Result + Vec2::Create(1.0, 1.0)) * Vec2::Create(Width, Height);
    return Result;
}

float CrossProduct2d(Vec2 A, Vec2 B)
{
    return A.x * B.y - A.y * B.x;
}

Vec4 Vec4::Create(float x, float y, float z, float w)
{
    Vec4 v = {};
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}

Vec4 Vec4::FromPoint(Vec3 Point)
{
    Vec4 v = {};
    v.x = Point.x;
    v.y = Point.y;
    v.z = Point.z;
    v.w = 1.0f;
    return v;
}

Vec3 Vec4::ToVec3()
{
    return Vec3::Create(this->x, this->y, this->z);
}

Vec4 operator+(Vec4 A, Vec4 B)
{
    return Vec4::Create(
        A.x + B.x,
        A.y + B.y,
        A.z + B.z,
        A.w + B.w
    );
}

Vec4 operator*(Vec4 A, float B)
{
    return Vec4::Create(A.x * B, A.y * B, A.z * B, A.w * B);
}

Vec4 operator*(float A, Vec4 B)
{
    return Vec4::Create(B.x * A, B.y * A, B.z * A, B.w * A);
}

Vec4 operator*(Mat4 A, Vec4 B)
{
    Vec4 Result = A.v[0] * B.x + A.v[1] * B.y + A.v[2] * B.z + A.v[3] * B.w;
    return Result;
}

Mat4 operator*(Mat4 A, Mat4 B)
{
    Mat4 Result = {};
    Result.v[0] = A * B.v[0];
    Result.v[1] = A * B.v[1];
    Result.v[2] = A * B.v[2];
    Result.v[3] = A * B.v[3];
    return Result;
}
Mat4 Mat4::Identity()
{
    Mat4 Result = {};
    Result.v[0].x = 1.0f;
    Result.v[1].y = 1.0f;
    Result.v[2].z = 1.0f;
    Result.v[3].w = 1.0f;
    return Result;
}

Mat4 Mat4::ScaleMatrix(float x, float y, float z)
{
    Mat4 Result = Mat4::Identity();
    Result.v[0].x = x;
    Result.v[1].y = y;
    Result.v[2].z = z;
    return Result;
}

Mat4 Mat4::TranslationMatrix(float x, float y, float z)
{
    Mat4 Result = Mat4::Identity();
    Result.v[3].x = x;
    Result.v[3].y = y;
    Result.v[3].z = z;
    return Result;
}

Mat4 Mat4::TranslationMatrixFromVec3(Vec3 V)
{
    Mat4 Result = Mat4::Identity();
    Result.v[3].x = V.x;
    Result.v[3].y = V.y;
    Result.v[3].z = V.z;
    return Result;
}

Mat4 Mat4::RotationMatrix(float x, float y, float z)
{
    Mat4 RotateX = Mat4::Identity();
    RotateX.v[1].y = cos(x);
    RotateX.v[2].y = -sin(x);
    RotateX.v[1].z = sin(x);
    RotateX.v[2].z = cos(x);


    Mat4 RotateY = Mat4::Identity();
    RotateY.v[0].x = cos(y);
    RotateY.v[2].x = -sin(y);
    RotateY.v[0].z = sin(y);
    RotateY.v[2].z = cos(y);

    Mat4 RotateZ = Mat4::Identity();
    RotateZ.v[0].x = cos(z);
    RotateZ.v[1].x = -sin(z);
    RotateZ.v[0].y = sin(z);
    RotateZ.v[1].y = cos(z);

    return RotateX * RotateY * RotateZ;
}

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
    Mat4 Transform)
{
    Vec3 TransformPoint0 = (Transform * Vec4::FromPoint(Point0)).ToVec3();
    Vec3 TransformPoint1 = (Transform * Vec4::FromPoint(Point1)).ToVec3();
    Vec3 TransformPoint2 = (Transform * Vec4::FromPoint(Point2)).ToVec3();

    Vec2 PointA = ProjectPoint(TransformPoint0, Width, Height);
    Vec2 PointB = ProjectPoint(TransformPoint1, Width, Height);
    Vec2 PointC = ProjectPoint(TransformPoint2, Width, Height);

    Vec2 Edge0 = PointB - PointA;
    Vec2 Edge1 = PointC - PointB;
    Vec2 Edge2 = PointA - PointC;

    bool IsTopLeft0 = (Edge0.x >= 0.0f && Edge0.y >= 0.0f) || (Edge0.x > 0.0f && Edge0.y == 0.0f);
    bool IsTopLeft1 = (Edge1.x >= 0.0f && Edge1.y >= 0.0f) || (Edge1.x > 0.0f && Edge1.y == 0.0f);
    bool IsTopLeft2 = (Edge2.x >= 0.0f && Edge2.y >= 0.0f) || (Edge2.x > 0.0f && Edge2.y == 0.0f);

    float BaryCentricDiv = CrossProduct2d(PointB - PointA, PointC - PointA);

    int32_t MinX = std::min(std::min(PointA.x, PointB.x), PointC.x);
    int32_t MinY = std::min(std::min(PointA.y, PointB.y), PointC.y);

    int32_t MaxX = std::max(std::max(round(PointA.x), round(PointB.x)), round(PointC.x));
    int32_t MaxY = std::max(std::max(round(PointA.y), round(PointB.y)), round(PointC.y));

    MinX = std::max(0, MinX);
    MinY = std::max(0, MinY);

    MaxX = std::min((int32_t)(Width - 1), MaxX);
    MaxY = std::min((int32_t)(Height - 1), MaxY);

    for (uint32_t Y = MinY; Y <= MaxY; ++Y) {
        for (uint32_t X = MinX; X <= MaxX; ++X) {
            Vec2 PixelPoint = Vec2::Create(X, Y) + Vec2::Create(0.5f, 0.5f);

            Vec2 PixelEdge0 = PixelPoint - PointA;
            Vec2 PixelEdge1 = PixelPoint - PointB;
            Vec2 PixelEdge2 = PixelPoint - PointC;

            float CrossLength0 = CrossProduct2d(PixelEdge0, Edge0);
            float CrossLength1 = CrossProduct2d(PixelEdge1, Edge1);
            float CrossLength2 = CrossProduct2d(PixelEdge2, Edge2);

            if ((CrossLength0 > 0.0f || (IsTopLeft0 && CrossLength0 == 0.0f)) &&
                (CrossLength1 > 0.0f || (IsTopLeft1 && CrossLength1 == 0.0f)) &&
                (CrossLength2 > 0.0f || (IsTopLeft2 && CrossLength2 == 0.0f)))
            {
                // Pixel inside triangle
                uint32_t PixelId = Y * Width + X;

                float T0 = - CrossLength1 / BaryCentricDiv;
                float T1 = - CrossLength2 / BaryCentricDiv;
                float T2 = - CrossLength0 / BaryCentricDiv;

                float Depth = T0 * (1.0f / TransformPoint0.z) +
                              T1 * (1.0f / TransformPoint1.z) +
                              T2 * (1.0f / TransformPoint2.z); 
                Depth = 1.0f / Depth;

                if (Depth < DepthBuffer[PixelId]) {
                    DepthBuffer[PixelId] = Depth;

                    Vec3 FinalColor = T0 * Color0 + T1 * Color1 + T2 * Color2;
                    uint8_t r = FinalColor.x * 255.0f;
                    uint8_t g = FinalColor.y * 255.0f;
                    uint8_t b = FinalColor.z * 255.0f;
                    uint32_t FinalColorU32 = (0xff << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b);
                    Pixels[PixelId] = FinalColorU32;
                }
            }
        }
    }
}

