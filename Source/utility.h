#ifndef __utility_h__
#define __utility_h__

#include "definitions.h"
class Utility
{
private:
    friend std::ostream &operator<<(std::ostream &os, const Vec3f &vec);
    friend std::ostream &operator<<(std::ostream &os, const Vec3i &vec);
    friend std::ostream &operator<<(std::ostream &os, const Vec4f &plane);
    friend std::ostream &operator<<(std::ostream &os, const Camera &cam);
    friend std::ostream &operator<<(std::ostream &os, const PointLight &pl);
    friend std::ostream &operator<<(std::ostream &os, const Material &mat);
    friend std::ostream &operator<<(std::ostream &os, const Scene &scene);

public:
    void PrintSceneDetails(Scene scene);

    // Geometry related methods
    Vec3f Cross(const Vec3f &lhs, const Vec3f &rhs);

    float Dot(const Vec3f &lhs, const Vec3f &rhs);

    // Multiplies rhs vector with the given float number on left-hand-side.
    Vec3f Scale(const float &lhs, const Vec3f &rhs);

    // Multiplies rhs vector with the components of the lhs vector.
    Vec3f VectorScale(const Vec3f &lhs, const Vec3f &rhs);

    // Divides lhs vector with the given float number on right-hand-side.
    Vec3f Circumsize(const Vec3f &lhs, const float &rhs);

    // Adds two vectors.
    Vec3f Add(const Vec3f &lhs, const Vec3f &rhs);

    // Subtracts two vectors.
    Vec3f Subtract(const Vec3f &lhs, const Vec3f &rhs);

    // Negates a vector.
    Vec3f Negate(const Vec3f &vector);

    // Calculates the length of a vector.
    float Length(const Vec3f &vector);

    // Normalizes a vector.
    Vec3f Normalize(const Vec3f &vector);

    // Calculates normal vector for the given vertex.
    Vec3f GetNormal(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2);
};

#endif // __utility_h__