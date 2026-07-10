#ifndef MESH_H
#define MESH_H

#include <vector>
#include <array>

#include "../matrix/matrix.h++"


/**
 * @brief Texture coordinate.
 */
typedef struct UV {

    float u = 0.0f;
    float v = 0.0f;

} UV;


/**
 * @brief Triangle geometry and its three texture coordinates.
 */
typedef struct MeshTriangle {

    Matrix vertices;

    std::array<UV, 3> textureCoordinates;


    /**
     * @brief Construct a textured triangle from vertex and UV data.
     *
     * @param vertices :: Triangle vertex positions as a 3x4 matrix.
     * @param uv0 :: Texture coordinate for the first vertex.
     * @param uv1 :: Texture coordinate for the second vertex.
     * @param uv2 :: Texture coordinate for the third vertex.
     */
    MeshTriangle(
        const Matrix &vertices,
        const UV &uv0,
        const UV &uv1,
        const UV &uv2
    ) :
        vertices(vertices),
        textureCoordinates({
            uv0,
            uv1,
            uv2
        })
    {
    }

} MeshTriangle;


/**
 * @brief Simple three-dimensional transform values.
 */
typedef struct Transform3D {

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

} Transform3D;


/**
 * @brief Mesh containing textured triangles and transform state.
 */
typedef struct Mesh {

    std::vector<MeshTriangle> tris;

    Transform3D position = {
        0.0f,
        0.0f,
        3.0f
    };

    Transform3D rotation = {
        0.0f,
        0.0f,
        0.0f
    };

    Transform3D scale = {
        1.0f,
        1.0f,
        1.0f
    };


    /**
     * @brief Move the mesh by the specified offset.
     *
     * @param x :: Offset along the X axis.
     * @param y :: Offset along the Y axis.
     * @param z :: Offset along the Z axis.
     */
    void translate(float x, float y, float z) {

        this->position.x += x;
        this->position.y += y;
        this->position.z += z;

        return;
    }


    /**
     * @brief Set the mesh position directly.
     *
     * @param x :: Absolute X coordinate.
     * @param y :: Absolute Y coordinate.
     * @param z :: Absolute Z coordinate.
     */
    void set_position(float x, float y, float z) {

        this->position.x = x;
        this->position.y = y;
        this->position.z = z;

        return;
    }


    /**
     * @brief Rotate the mesh by the given angles.
     *
     * @param x :: Rotation increment around the X axis (radians).
     * @param y :: Rotation increment around the Y axis (radians).
     * @param z :: Rotation increment around the Z axis (radians).
     */
    void rotate(float x, float y, float z) {

        this->rotation.x += x;
        this->rotation.y += y;
        this->rotation.z += z;

        return;
    }


    /**
     * @brief Set the mesh rotation directly.
     *
     * @param x :: Absolute rotation around the X axis (radians).
     * @param y :: Absolute rotation around the Y axis (radians).
     * @param z :: Absolute rotation around the Z axis (radians).
     */
    void set_rotation(float x, float y, float z) {

        this->rotation.x = x;
        this->rotation.y = y;
        this->rotation.z = z;

        return;
    }


    /**
     * @brief Scale the mesh by the specified factors.
     *
     * @param x :: Scale multiplier on the X axis.
     * @param y :: Scale multiplier on the Y axis.
     * @param z :: Scale multiplier on the Z axis.
     */
    void resize(float x, float y, float z) {

        this->scale.x *= x;
        this->scale.y *= y;
        this->scale.z *= z;

        return;
    }


    /**
     * @brief Set the mesh scale directly.
     *
     * @param x :: Absolute scale on the X axis.
     * @param y :: Absolute scale on the Y axis.
     * @param z :: Absolute scale on the Z axis.
     */
    void set_scale(float x, float y, float z) {

        this->scale.x = x;
        this->scale.y = y;
        this->scale.z = z;

        return;
    }

} Mesh;


#endif

