#ifndef MATH_H
#define MATH_H
#include <cmath>
#include <array>
struct Vec3
{
    float x,y,z;
    Vec3(float x , float y , float z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vec3(){
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    Vec3 operator+(Vec3 other)
    {
        return Vec3(x + other.x , y + other.y , z + other.z);
    };
    Vec3 operator-(Vec3 other)
    {
        return Vec3(x - other.x , y - other.y , z - other.z);
    };
    Vec3 operator*(float scalar)
    {
        return Vec3(x * scalar , y * scalar , z * scalar);
    };
    Vec3 operator/(float scalar)
    {
        return Vec3(x / scalar , y / scalar , z / scalar);
    };

    float dot(Vec3 other)
    {
    return x * other.x + y * other.y + z * other.z;
    }

    Vec3 cross(Vec3 other)
    {
        return Vec3 (y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);

    }

    float length()
    {
        return std::sqrt(x*x + y*y + z*z);
    }

    Vec3 normalize()
    {
        float len = length();
        if(len > 0.0)
        {
            return *this / len;
        }
        return *this;
    }
    };


struct Vec4
{
    float x,y,z,w;
    Vec4(float x, float y, float z, float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    Vec4(Vec3 v , float w)
    {
        this -> x = v.x;
        this -> y = v.y;
        this -> z = v.z;
        this -> w = w;
    }

};

struct Mat4
{
    float m[4][4];
    Mat4()
    {
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                m[i][j] = 0.0f; // Assign 0 to the actual struct member
            }
        }
    }
    static Mat4 identity()
    {
        Mat4 result;
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                if(i == j)
                {
                    result.m[i][j] = 1;
                }else
                {
                    result.m[i][j] = 0;
                }
            }
        }
     return result;
    }
    Mat4 operator*(Mat4 other)
    {
        Mat4 result;
        for(int i = 0; i < 4; ++i){
            for(int j = 0; j < 4; ++j){
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                sum += this->m[i][k]* other.m[k][j];
            }
            result.m[i][j] = sum;
            }
        }
        return result;
    }
    Vec4 operator*(Vec4 v)
    {
        return Vec4(
            this->m[0][0] * v.x + this->m[0][1] * v.y + this->m[0][2] * v.z + this->m[0][3] * v.w,
            this->m[1][0] * v.x + this->m[1][1] * v.y + this->m[1][2] * v.z + this->m[1][3] * v.w,
            this->m[2][0] * v.x + this->m[2][1] * v.y + this->m[2][2] * v.z + this->m[2][3] * v.w,
            this->m[3][0] * v.x + this->m[3][1] * v.y + this->m[3][2] * v.z + this->m[3][3] * v.w
        );
    }
    static Mat4 translate(float x, float y, float z)
    {
        Mat4 result = Mat4::identity();

        result.m[0][3] = x;
        result.m[1][3] = y;
        result.m[2][3] = z;

        return result;
    }
    static Mat4 scale(float x , float y , float z)
    {
        Mat4 result = Mat4::identity();
        
        // Replace the 1s on the x, y, and z diagonals with our scale factors
        result.m[0][0] = x;
        result.m[1][1] = y;
        result.m[2][2] = z;
        
        return result;
    }
    static Mat4 rotateX(float angle)
    {
        Mat4 result = Mat4::identity();

        result.m[1][1] = std::cos(angle);
        result.m[1][2] = -(std::sin(angle));
        result.m[2][1] = std::sin(angle);
        result.m[2][2] = std::cos(angle);

        return result;
    }
    static Mat4 rotateY(float angle)
    {
        Mat4 result = Mat4::identity();

        result.m[0][0] = std::cos(angle);
        result.m[0][2] = std::sin(angle);
        result.m[2][0] = -(std::sin(angle));
        result.m[2][2] = std::cos(angle);

        return result;
    }
    static Mat4 rotateZ(float angle)
    {
        Mat4 result = Mat4::identity();
        
        result.m[0][0] = std::cos(angle);
        result.m[1][0] = -(std::sin(angle));
        result.m[0][1] = std::sin(angle);
        result.m[1][1] = std::cos(angle);

        return result;
    }
    static Mat4 perspective(float fov, float aspect, float near , float far)
    {
        Mat4 result = Mat4::identity();
        // FIXED: standard perspective projection math
        float tanHalfFov = std::tan(fov / 2.0f);
        
        result.m[0][0] = 1.0f / (aspect * tanHalfFov);
        result.m[1][1] = 1.0f / tanHalfFov;
        result.m[2][2] = -(far + near) / (far - near);
        result.m[2][3] = -(2.0f * far * near) / (far - near);
        result.m[3][2] = -1.0f; // This creates the 3D perspective divide
        result.m[3][3] = 0.0f;  
        
        return result;
    }
    static Mat4 lookAt(Vec3 eye , Vec3 target , Vec3 up)
    {
        // 1. Calculate the Forward vector
        Vec3 forward = (target - eye).normalize();
        
        // 2. Calculate the Right vector
        Vec3 right = forward.cross(up).normalize();
        
        // 3. Calculate the True Up vector
        // (No need to normalize again, crossing two perpendicular unit vectors gives a unit vector)
        Vec3 trueUp = right.cross(forward);
        
        Mat4 result = Mat4::identity();
        
        // Row 0: The Right vector (X-axis)
        result.m[0][0] = right.x;
        result.m[0][1] = right.y;
        result.m[0][2] = right.z;
        result.m[0][3] = -(right.dot(eye)); // Moves the world based on camera X position
        
        // Row 1: The True Up vector (Y-axis)
        result.m[1][0] = trueUp.x;
        result.m[1][1] = trueUp.y;
        result.m[1][2] = trueUp.z;
        result.m[1][3] = -(trueUp.dot(eye)); // Moves the world based on camera Y position
        
        // Row 2: The inverted Forward vector (Z-axis)
        // We invert it because standard OpenGL camera looks down the negative Z-axis
        result.m[2][0] = -forward.x;
        result.m[2][1] = -forward.y;
        result.m[2][2] = -forward.z;
        result.m[2][3] = forward.dot(eye);   // Moves the world based on camera Z position
        
        return result;
    }
};
#endif // MATH_H
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
