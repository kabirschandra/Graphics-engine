#ifndef RENDER3D_H
#define RENDER3D_H

#include <vector>
#include <array>
#include <algorithm>
#include <cmath>

#include "../framebuffer/framebuffer.h++"
#include "../matrix/matrix.h++"
#include "../objects/mesh.h++"


/**
 * @brief 3D renderer capable of drawing textured meshes.
 *
 * Render3D provides a simple pipeline for transforming, culling, sorting,
 * projecting, and rasterizing textured mesh triangles onto a framebuffer.
 */
class Render3D {

    private:

    /**
     * @brief Triangle data prepared for rendering.
     *
     * The RenderTriangle stores transformed vertex positions, texture
     * coordinates, and depth information used for painter's-order sorting.
     */
    typedef struct RenderTriangle {

        Matrix vertices;

        std::array<UV, 3> textureCoordinates;

        float depth;

    } RenderTriangle;


    public:

    /**
     * @brief Render a textured mesh.
     *
     * @param mesh :: Mesh to render
     * @param texture :: Texture applied using the OBJ UV coordinates (optional)
     * @param framebuffer :: Destination framebuffer
     *
     * @return void :: None
     */
    void draw_mesh(
        Mesh &mesh,
        FrameBuffer &framebuffer,
        Texture *texture = nullptr
    ) {

        int screenWidth =
            framebuffer.get_width();

        int screenHeight =
            framebuffer.get_height();

        float fovDegrees =
            70.0f;

        float fovRadians =
            fovDegrees *
            float(M_PI) /
            180.0f;

        float aspect =
            float(screenWidth) /
            float(screenHeight);

        float nearPlane =
            0.1f;

        float farPlane =
            1000.0f;

        std::vector<RenderTriangle> renderTriangles;

        for(MeshTriangle &sourceTriangle : mesh.tris) {

            Matrix triangle =
                sourceTriangle.vertices.copy();

            triangle = triangle.scale(
                mesh.scale.x,
                mesh.scale.y,
                mesh.scale.z
            );

            triangle = triangle.rotate_x(
                mesh.rotation.x
            );

            triangle = triangle.rotate_y(
                mesh.rotation.y
            );

            triangle = triangle.rotate_z(
                mesh.rotation.z
            );

            triangle = triangle.translate(
                mesh.position.x,
                mesh.position.y,
                mesh.position.z
            );

            /*
             * Minimal near-plane handling.
             *
             * Proper clipping can be added later. For now, reject a
             * triangle if any vertex crosses the near plane.
             */
            bool crossesNearPlane =
                false;

            for(size_t vertex = 0; vertex < 3; vertex++) {

                if(
                    triangle.at(vertex, 2) <=
                    nearPlane
                ) {
                    crossesNearPlane =
                        true;

                    break;
                }
            }

            if(crossesNearPlane) {
                continue;
            }

            /*
             * Back-face culling.
             */
            float edge1X =
                triangle.at(1, 0) -
                triangle.at(0, 0);

            float edge1Y =
                triangle.at(1, 1) -
                triangle.at(0, 1);

            float edge1Z =
                triangle.at(1, 2) -
                triangle.at(0, 2);

            float edge2X =
                triangle.at(2, 0) -
                triangle.at(0, 0);

            float edge2Y =
                triangle.at(2, 1) -
                triangle.at(0, 1);

            float edge2Z =
                triangle.at(2, 2) -
                triangle.at(0, 2);

            float normalX =
                edge1Y * edge2Z -
                edge1Z * edge2Y;

            float normalY =
                edge1Z * edge2X -
                edge1X * edge2Z;

            float normalZ =
                edge1X * edge2Y -
                edge1Y * edge2X;

            float cameraRayX =
                triangle.at(0, 0);

            float cameraRayY =
                triangle.at(0, 1);

            float cameraRayZ =
                triangle.at(0, 2);

            float facing =
                normalX * cameraRayX +
                normalY * cameraRayY +
                normalZ * cameraRayZ;

            if(facing >= 0.0f) {
                continue;
            }

            float averageDepth =
                (
                    triangle.at(0, 2) +
                    triangle.at(1, 2) +
                    triangle.at(2, 2)
                ) /
                3.0f;

            renderTriangles.push_back({
                triangle,
                sourceTriangle.textureCoordinates,
                averageDepth
            });
        }

        /*
         * Draw farther triangles first.
         */
        std::sort(
            renderTriangles.begin(),
            renderTriangles.end(),

            [](
                const RenderTriangle &left,
                const RenderTriangle &right
            ) {

                return left.depth > right.depth;
            }
        );

        for(RenderTriangle &renderTriangle : renderTriangles) {

            Matrix triangle =
                renderTriangle.vertices.project(
                    fovRadians,
                    aspect,
                    nearPlane,
                    farPlane
                );

            triangle =
                triangle.perspective_divide();

            triangle =
                triangle.viewport(
                    screenWidth,
                    screenHeight
                );

            if(texture != nullptr) {

                framebuffer.fill_textured_triangle(
                    triangle,
                    renderTriangle.textureCoordinates,
                    *texture
                );

            } else {

                framebuffer.draw_triangle(
                    triangle,
                    {
                        255,
                        255,
                        255,
                        255
                    }
                );
            }
        }

        return;
    }
};


#endif


