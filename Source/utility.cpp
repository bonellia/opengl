#include "utility.h"
#include <iostream>
/*
Overloads << operator for Vec3f object, allowing it to be more human readable.
*/
std::ostream &operator<<(std::ostream &os, const parser::Vec3f &vec)
{
    os << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
    return os;
}
/*
Overloads << operator for Vec3i object, allowing it to be more human readable.
*/
std::ostream &operator<<(std::ostream &os, const parser::Vec3i &vec)
{
    os << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
    return os;
}
/*
Overloads << operator for Vec4f object, allowing it to be more human readable.
*/
std::ostream &operator<<(std::ostream &os, const parser::Vec4f &plane)
{
    os << '(' << plane.x << ", " << plane.y << ", " << plane.z << ", " << plane.w << ')';
    return os;
}
/*
Overloads << operator for Camera object, allowing it to be more human readable.
*/
std::ostream &operator<<(std::ostream &os, const parser::Camera &cam)
{
    os << "H:" << cam.image_height << ", W: " << cam.image_width << '\n'
       << "Near plane: " << cam.near_plane << '\n'
       << "Near distance:" << cam.near_distance << '\n'
       << "Position: " << cam.position << '\n'
       << "Up vector: " << cam.up << '\n'
       << "Gaze vector: " << cam.gaze << '\n';
    return os;
}
/*
Overloads << operator for PointLight object, allowing it to be more human readable.
*/
std::ostream &operator<<(std::ostream &os, const parser::PointLight &pl)
{
    os << "Intensity: " << pl.intensity << '\n'
       << "Position:" << pl.position << '\n';
    return os;
}
/*
Overloads << operator for Material object, allowing it to be more human readable.
*/
std::ostream &operator<<(std::ostream &os, const parser::Material &mat)
{
    os << "Ambient: " << mat.ambient << '\n'
       << "Diffuse: " << mat.diffuse << '\n'
       << "Specular: " << mat.specular << '\n'
       << "Phong exponent: " << mat.phong_exponent << '\n';
    return os;
}
/*
Overloads << operator for Scene object, allowing it to be more human readable.
*/
std::ostream &operator<<(std::ostream &os, const parser::Scene &scene)
{
    int counter = 0;
    os
        << "------------------------------\n"
        << "       SCENE PROPERTIES       \n"
        << "------------------------------\n"
        << "Background color: " << scene.background_color << '\n'
        << "Ambient light: " << scene.ambient_light << '\n'
        << "Vertex count: " << scene.vertex_data.size() << '\n'
        << "Mesh count: " << scene.meshes.size() << '\n'        
        << "Culling enabled: " << scene.culling_enabled << '\n'
        << "Culling face: " << scene.culling_face << '\n'
        << "------------------------------\n"
        << "          Camera           \n"
        << "------------------------------\n";

    os << "Camera " << counter++ << ":\n---------\n"
       << scene.camera;

    counter = 1;
    os
        << "------------------------------\n"
        << "        Point light(s)        \n"
        << "------------------------------\n";
    for (parser::PointLight pl : scene.point_lights)
    {
        os << "Point Light " << counter++ << ":\n--------------\n"
           << pl;
    }
    counter = 1;
    os
        << "------------------------------\n"
        << "         Material(s)          \n"
        << "------------------------------\n";
    for (parser::Material mat : scene.materials)
    {
        os << "Material " << counter++ << ":\n-----------\n"
           << mat;
    }
    return os;
}
/*
Prints the scene configuration in a human readable format.
*/
void Utility::PrintSceneDetails(parser::Scene scene)
{
    std::cout << scene << std::endl;
}
Vec3f Utility::Cross(const Vec3f &lhs, const Vec3f &rhs)
{
    return {
        lhs.y * rhs.z - rhs.y * lhs.z,
        rhs.x * lhs.z - lhs.x * rhs.z,
        lhs.x * rhs.y - rhs.x * lhs.y};
}

float Utility::Dot(const Vec3f &lhs, const Vec3f &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec3f Utility::Scale(const float &lhs, const Vec3f &rhs)
{
    Vec3f result;
    result.x = lhs * rhs.x;
    result.y = lhs * rhs.y;
    result.z = lhs * rhs.z;
    return result;
}

Vec3f Utility::VectorScale(const Vec3f &lhs, const Vec3f &rhs)
{
    Vec3f result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    result.z = lhs.z * rhs.z;
    return result;
}

Vec3f Utility::Circumsize(const Vec3f &lhs, const float &rhs)
{
    Vec3f result;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    result.z = lhs.z / rhs;
    return result;
}

Vec3f Utility::Add(const Vec3f &lhs, const Vec3f &rhs)
{
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z};
}

Vec3f Utility::Subtract(const Vec3f &lhs, const Vec3f &rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z};
}

Vec3f Utility::Negate(const Vec3f &vector)
{
    Vec3f result;
    result.x = -vector.x;
    result.y = -vector.y;
    result.z = -vector.z;
    return result;
}

float Utility::Length(const Vec3f &vector)
{
    return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

Vec3f Utility::Normalize(const Vec3f &vector)
{
    Vec3f normalized_vector;
    float vector_length = Length(vector);
    normalized_vector = Circumsize(vector, vector_length);
    return normalized_vector;
}

Vec3f Utility::GetNormal(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2)
{
    const Vec3f ba = Subtract(v0, v1);
    const Vec3f ca = Subtract(v0, v2);
    return Normalize(Cross(ba, ca));
}