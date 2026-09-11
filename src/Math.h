#ifndef MATH_H
#define MATH_H
#include <cmath>
#include <limits>
#include <random>
#include <array>

int randomize(int min, int max);

inline float clamp(float val, float min, float max)
{
    return std::fmax(min, std::fmin(val, max));
}

template <typename T>
bool isNearZero(T value, T epsilon = std::numeric_limits<T>::epsilon()) 
{
    return std::abs(value) < epsilon;
}

struct Vec3
{
    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(Vec3 other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(Vec3 other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float s)    const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator*(Vec3 other) const { return Vec3(x * other.x, y * other.y, z * other.z); }
    Vec3 operator/(float s)    const { return Vec3(x / s, y / s, z / s); }
    Vec3 operator/(Vec3 other) const { return Vec3(x / other.x, y / other.y, z / other.z); }

    static float randomFloat() {
        static thread_local std::mt19937 generator(std::random_device{}());
        static thread_local std::uniform_real_distribution<float> distribution(0.0, 1.0);
        return distribution(generator);
    }

    static float randomFloat(float min, float max) {
        return min + (max-min)*randomFloat();
    }

    static Vec3 random()
    {
        return Vec3(randomFloat(), randomFloat(), randomFloat());
    }

    static Vec3 random(float min, float max)
    {
        return Vec3(randomFloat(min, max), randomFloat(min, max), randomFloat(min, max));
    }

    static Vec3 randomInUnitSphere()
    {
        static thread_local std::mt19937 gen(std::random_device{}());
        static thread_local std::uniform_real_distribution<float> distrib(-1.0f, 1.0f);

        Vec3 point;
        do
        {
            point = Vec3(distrib(gen), distrib(gen), distrib(gen));
        } while (point.dot(point) >= 1.0f);

        return point;
    }

    static Vec3 randomUnitVector()
    {
        return randomInUnitSphere().normalize();
    }

    Vec3 reflect(Vec3 normal) const
    {
        float projection = 2.0f * dot(normal);
        return Vec3(x - projection * normal.x,
                    y - projection * normal.y,
                    z - projection * normal.z);
    }

    Vec3 refract(Vec3 normal, float eta) const
    {
        Vec3 unit = normalize();
        Vec3 opposite = Vec3(-unit.x, -unit.y, -unit.z);
        float cosTheta = std::fmin(opposite.dot(normal), 1.0f);
        Vec3 perpendicular = (unit + normal * cosTheta) * eta;
        float parallelLength = std::sqrt(std::fabs(1.0f - perpendicular.dot(perpendicular)));
        Vec3 parallel = normal * -parallelLength;

        return perpendicular + parallel;
    }

    bool nearZero() const
    {
        const float epsilon = 1e-8f;
        return std::fabs(x) < epsilon &&
               std::fabs(y) < epsilon &&
               std::fabs(z) < epsilon;
    }

    float dot(Vec3 other) const { return x*other.x + y*other.y + z*other.z; }
    Vec3  cross(Vec3 other) const {
        return Vec3(y*other.z - z*other.y,
                    z*other.x - x*other.z,
                    x*other.y - y*other.x);
    }
    float length() const { return std::sqrt(x*x + y*y + z*z); }
    Vec3  normalize() const {
        float len = length();
        return len > 0.0f ? *this / len : *this;
    }
};

struct Vec4
{
    float x,y,z,w;
    Vec4(float x, float y, float z, float w);
    Vec4(Vec3 v , float w);
};

struct Mat4
{
    float m[4][4];
    Mat4();
    static Mat4 identity();
    Mat4 operator*(Mat4 other);
    Vec4 operator*(Vec4 v);
    static Mat4 translate(float x, float y, float z);
    static Mat4 scale(float x , float y , float z);
    static Mat4 rotateX(float angle);
    static Mat4 rotateY(float angle);
    static Mat4 rotateZ(float angle);
    static Mat4 perspective(float fov, float aspect, float near , float far);
    static Mat4 lookAt(Vec3 eye , Vec3 target , Vec3 up);
};

// Extensions for ray tracing
inline Vec3 operator*(float t, const Vec3& v) {
    return Vec3(v.x * t, v.y * t, v.z * t);
}

inline float dot(const Vec3& u, const Vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline Vec3 cross(const Vec3& u, const Vec3& v) {
    return Vec3(u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}
#endif // MATH_H
