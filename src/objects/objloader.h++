#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "mesh.h++"


/**
 * @brief 3D vertex position extracted from an OBJ file.
 */
typedef struct Vertex3D {

    float x;
    float y;
    float z;

} Vertex3D;


/**
 * @brief Parsed indices from an OBJ face token.
 *
 * The OBJFaceIndex stores zero-based vertex and texture coordinate indices.
 * Texture indices may be omitted and remain -1 when not provided.
 */
typedef struct OBJFaceIndex {

    int vertex = -1;
    int texture = -1;

} OBJFaceIndex;


/**
 * @brief Loader for Wavefront OBJ mesh files.
 *
 * OBJLoader supports vertex positions, texture coordinates, and face
 * triangulation. It converts OBJ face indices to zero-based references and
 * builds textured mesh triangles.
 */
class OBJLoader {

    private:

    /**
     * @brief Resolve an OBJ index to zero-based form.
     *
     * OBJ supports positive and negative indices. Positive values are
     * converted by subtracting one, while negative values are resolved from
     * the end of the referenced list.
     *
     * @param rawIndex :: Raw index from the OBJ file.
     * @param valueCount :: Number of values in the referenced list.
     * @return int :: Zero-based index into the list.
     * @throws std::runtime_error if the index is zero.
     */
    static int resolve_index(
        int rawIndex,
        size_t valueCount
    ) {

        if(rawIndex > 0) {
            return rawIndex - 1;
        }

        if(rawIndex < 0) {
            return int(valueCount) + rawIndex;
        }

        throw std::runtime_error(
            "FATAL: OBJ indices cannot be zero."
        );
    }


    /**
     * @brief Parse a face token into vertex and texture indices.
     *
     * Face tokens have the form "v", "v/t", or "v/t/n". Only vertex and
     * texture indices are parsed, and missing texture indices are left as -1.
     *
     * @param token :: Face token from an OBJ face line.
     * @param vertexCount :: Number of parsed vertex positions.
     * @param textureCount :: Number of parsed texture coordinates.
     * @return OBJFaceIndex :: Zero-based vertex and texture indices.
     */
    static OBJFaceIndex parse_face_token(
        const std::string &token,
        size_t vertexCount,
        size_t textureCount
    ) {

        OBJFaceIndex result;

        size_t firstSlash =
            token.find('/');

        if(firstSlash == std::string::npos) {

            result.vertex = resolve_index(
                std::stoi(token),
                vertexCount
            );

            return result;
        }

        std::string vertexText =
            token.substr(
                0,
                firstSlash
            );

        if(vertexText.empty()) {
            throw std::runtime_error(
                "FATAL: OBJ face token has no vertex index."
            );
        }

        result.vertex = resolve_index(
            std::stoi(vertexText),
            vertexCount
        );

        size_t secondSlash =
            token.find(
                '/',
                firstSlash + 1
            );

        size_t textureEnd =
            secondSlash == std::string::npos ?
            token.size() :
            secondSlash;

        std::string textureText =
            token.substr(
                firstSlash + 1,
                textureEnd - firstSlash - 1
            );

        if(!textureText.empty()) {

            result.texture = resolve_index(
                std::stoi(textureText),
                textureCount
            );
        }

        return result;
    }


    /**
     * @brief Create a textured triangle from parsed OBJ indices.
     *
     * @param vertices :: Parsed vertex positions.
     * @param textureCoordinates :: Parsed UV coordinates.
     * @param index0 :: First face vertex index record.
     * @param index1 :: Second face vertex index record.
     * @param index2 :: Third face vertex index record.
     * @return MeshTriangle :: Triangle containing vertex positions and UVs.
     */
    static MeshTriangle make_triangle(
        const std::vector<Vertex3D> &vertices,
        const std::vector<UV> &textureCoordinates,
        const OBJFaceIndex &index0,
        const OBJFaceIndex &index1,
        const OBJFaceIndex &index2
    ) {

        const Vertex3D &vertex0 =
            vertices.at(index0.vertex);

        const Vertex3D &vertex1 =
            vertices.at(index1.vertex);

        const Vertex3D &vertex2 =
            vertices.at(index2.vertex);

        UV uv0 = {
            0.0f,
            0.0f
        };

        UV uv1 = {
            0.0f,
            0.0f
        };

        UV uv2 = {
            0.0f,
            0.0f
        };

        if(index0.texture >= 0) {
            uv0 = textureCoordinates.at(
                index0.texture
            );
        }

        if(index1.texture >= 0) {
            uv1 = textureCoordinates.at(
                index1.texture
            );
        }

        if(index2.texture >= 0) {
            uv2 = textureCoordinates.at(
                index2.texture
            );
        }

        Matrix triangleVertices(3, 4, {
            vertex0.x, vertex0.y, vertex0.z, 1.0f,
            vertex1.x, vertex1.y, vertex1.z, 1.0f,
            vertex2.x, vertex2.y, vertex2.z, 1.0f
        });

        return MeshTriangle(
            triangleVertices,
            uv0,
            uv1,
            uv2
        );
    }


    public:

    /**
     * @brief Load a mesh from a Wavefront OBJ file.
     *
     * Supports vertex positions, texture coordinates, and face definitions.
     * Faces with more than three vertices are triangulated using a fan around
     * the first vertex.
     *
     * @param filename :: Path to the OBJ file.
     * @return Mesh :: Loaded textured mesh.
     * @throws std::runtime_error on file IO or invalid OBJ format.
     */
    static Mesh load(const std::string &filename) {

        std::ifstream file(filename);

        if(!file.is_open()) {
            throw std::runtime_error(
                "FATAL: Could not open OBJ file: " +
                filename
            );
        }

        std::vector<Vertex3D> vertices;
        std::vector<UV> textureCoordinates;

        Mesh mesh;

        std::string line;

        while(std::getline(file, line)) {

            if(line.empty() || line[0] == '#') {
                continue;
            }

            std::stringstream stream(line);

            std::string type;

            stream >> type;

            if(type == "v") {

                Vertex3D vertex;

                if(
                    !(stream >>
                        vertex.x >>
                        vertex.y >>
                        vertex.z)
                ) {
                    throw std::runtime_error(
                        "FATAL: Invalid OBJ vertex."
                    );
                }

                vertices.push_back(
                    vertex
                );

            } else if(type == "vt") {

                UV textureCoordinate;

                if(
                    !(stream >>
                        textureCoordinate.u >>
                        textureCoordinate.v)
                ) {
                    throw std::runtime_error(
                        "FATAL: Invalid OBJ texture coordinate."
                    );
                }

                textureCoordinates.push_back(
                    textureCoordinate
                );

            } else if(type == "f") {

                std::vector<OBJFaceIndex> face;

                std::string token;

                while(stream >> token) {

                    OBJFaceIndex index =
                        parse_face_token(
                            token,
                            vertices.size(),
                            textureCoordinates.size()
                        );

                    if(
                        index.vertex < 0 ||
                        size_t(index.vertex) >= vertices.size()
                    ) {
                        throw std::runtime_error(
                            "FATAL: Invalid OBJ vertex index."
                        );
                    }

                    if(
                        index.texture >= 0 &&
                        size_t(index.texture) >= textureCoordinates.size()
                    ) {
                        throw std::runtime_error(
                            "FATAL: Invalid OBJ texture index."
                        );
                    }

                    face.push_back(
                        index
                    );
                }

                if(face.size() < 3) {
                    continue;
                }

                /*
                 * Triangle-fan triangulation:
                 *
                 * 0, 1, 2
                 * 0, 2, 3
                 * 0, 3, 4
                 */
                for(size_t i = 1; i + 1 < face.size(); i++) {

                    mesh.tris.push_back(
                        make_triangle(
                            vertices,
                            textureCoordinates,
                            face[0],
                            face[i],
                            face[i + 1]
                        )
                    );
                }
            }
        }

        if(mesh.tris.empty()) {
            throw std::runtime_error(
                "FATAL: OBJ file contains no renderable faces."
            );
        }

        return mesh;
    }
};


#endif

