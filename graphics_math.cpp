#include "graphics_math.h"
#include <cmath>

Vec2 Vec2::create(float x, float y) {
    Vec2 v = {};
    v.x = x;
    v.y = y;
    return  v;
}

Vec2 operator+(Vec2 A, Vec2 B)
{
    return Vec2::create(A.x + B.x, A.y + B.y);
}

Vec2 operator-(Vec2 A, Vec2 B)
{
    return Vec2::create(A.x - B.x, A.y - B.y);
}

Vec2 operator*(Vec2 A, Vec2 B)
{
    return Vec2::create(A.x * B.x, A.y * B.y);
}

Vec2 operator*(float B, Vec2 A)
{
    return Vec2::create(A.x * B, A.y * B);
}

Vec2 operator/(Vec2 A, float B)
{
    return Vec2::create(A.x / B, A.y / B);
}

Vec3 Vec3::create(float x, float y, float z) {
    Vec3 v = {};
    v.x = x;
    v.y = y;
    v.z = z;
    return  v;
}

Vec3 operator+(Vec3 A, Vec3 B)
{
    return Vec3::create(A.x + B.x, A.y + B.y, A.z + B.z);
}

Vec3 operator*(float A, Vec3 B)
{
    return Vec3::create(B.x * A, B.y * A, B.z * A);
}

Vec3 operator*(Vec3 A, float B)
{
    return Vec3::create(A.x * B, A.y * B, A.z * B);
}

Vec2 ProjectPoint(Vec3 Pos, uint32_t Width, uint32_t Height)
{
    Vec2 Result = Vec2::create(Pos.x, Pos.y) / Pos.z;
    Result = 0.5 * (Result + Vec2::create(1.0, 1.0)) * Vec2::create(Width, Height);
    return Result;
}

float CrossProduct2d(Vec2 A, Vec2 B)
{
    return A.x * B.y - A.y * B.x;
}

void DrawTriangle(
    Vec3 *Points,
    Vec3 *Color,
    uint32_t *Pixels,
    uint32_t Width,
    uint32_t Height,
    float *DepthBuffer)
{
    Vec2 PointA = ProjectPoint(Points[0], Width, Height);
    Vec2 PointB = ProjectPoint(Points[1], Width, Height);
    Vec2 PointC = ProjectPoint(Points[2], Width, Height);

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
            Vec2 PixelPoint = Vec2::create(X, Y) + Vec2::create(0.5f, 0.5f);

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

                float Depth = T0 * (1.0f / Points[0].z) +
                              T1 * (1.0f / Points[1].z) +
                              T2 * (1.0f / Points[2].z); 
                Depth = 1.0f / Depth;

                if (Depth < DepthBuffer[PixelId]) {
                    DepthBuffer[PixelId] = Depth;

                    Vec3 FinalColor = T0 * Color[0] + T1 * Color[1] + T2 * Color[2];
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

