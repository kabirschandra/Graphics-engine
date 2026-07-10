#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>

/**
 * @class Matrix
 * @brief A simple row-major matrix for 3D transformations and projection.
 *
 * This class stores matrix values in a flat std::vector<float> and provides
 * matrix multiplication, addition, subtraction, transform generation, and
 * viewport/projection helpers used by the graphics engine.
 */
class Matrix {
    private:
    size_t rows;
    size_t cols;
    std::vector<float> data;


    public:
    /**
     * @brief Construct a matrix with the given dimensions
     *
     * @param rows :: Number of rows in the matrix
     * @param cols :: Number of columns in the matrix
     */
    Matrix(size_t rows, size_t cols) {
        this->rows = rows;
        this->cols = cols;
        this->data.resize(rows * cols);
    }

    /**
     * @brief Construct a matrix from a constant data vector
     *
     * @param rows :: Number of rows in the matrix
     * @param cols :: Number of columns in the matrix
     * @param data :: Flattened matrix values in row-major order
     * @throws std::invalid_argument if rows * cols does not match data size
     */
    Matrix (const size_t rows, const size_t cols, const std::vector<float> &data) {
        this->rows = rows;
        this->cols = cols;

        if(rows * cols != data.size()) {
            throw std::invalid_argument("FATAL: Incompatible matrix fill.\n");
        }

        this->data = data;
    }

    /**
     * @brief Construct a matrix from a mutable data vector
     *
     * @param rows :: Number of rows in the matrix
     * @param cols :: Number of columns in the matrix
     * @param data :: Flattened matrix values in row-major order
     * @throws std::invalid_argument if rows * cols does not match data size
     */
    Matrix (size_t rows, size_t cols, std::vector<float> &data) {
        this->rows = rows;
        this->cols = cols;

        if(rows * cols != data.size()) {
            throw std::invalid_argument("FATAL: Incompatible matrix fill.\n");
        }

        this->data = data;
    }

    /**
     * @brief Get the matrix row count
     *
     * @return size_t :: Number of rows in the matrix
     */
    size_t get_rows(void) {
        return this->rows;
    }

    /**
     * @brief Get the matrix column count
     *
     * @return size_t :: Number of columns in the matrix
     */
    size_t get_cols(void) {
        return this->cols;
    }

    /**
     * @brief Create a copy of the current matrix
     *
     * @return Matrix :: Copy of the matrix
     */
    Matrix copy(void) {
        return Matrix(this->rows, this->cols, this->data);
    }

    /**
     * @brief Fill the matrix with new values
     *
     * @param data :: Flattened matrix values in row-major order
     * @throws std::invalid_argument if rows * cols does not match data size
     * @return void :: None
     */
    void fill(std::vector<float> &data) {
        if(rows * cols != data.size()) {
            throw std::invalid_argument("FATAL: Incompatible matrix fill.\n");
        }
        this->data = data;
    }

    /**
     * @brief Access a matrix element by row and column
     *
     * @param row :: Row index of the element
     * @param col :: Column index of the element
     * @throws std::out_of_range if the requested row or column is out of bounds
     * @return float& :: Reference to the requested element
     */
    float &at(size_t row, size_t col) {

        if(row >= this->rows || col >= this->cols) {
            throw std::out_of_range(
                "FATAL: Out of bounds access in matrix.\n"
            );
        }

        return this->data[row * this->cols + col];
    }

    /**
     * @brief Multiply this matrix by another matrix
     *
     * @param rhs :: Matrix to multiply with this matrix
     * @throws std::invalid_argument if the matrix dimensions are incompatible
     * @return Matrix& :: Reference to this matrix after multiplication
     */
    Matrix &multiply(Matrix &rhs) {
        if (this->cols != rhs.rows) {
            throw std::invalid_argument("FATAL: Incompatible matrix dimensions for multiplication.\n");
        }

        Matrix result(this->rows, rhs.cols);
        for (size_t i = 0; i < this->rows; i++) {
            for (size_t j = 0; j < rhs.cols; j++) {
                float sum = 0.0f;
                for (size_t k = 0; k < this->cols; k++) {
                    sum += this->data[i * this->cols + k] * rhs.data[k * rhs.cols + j];
                }
                result.data[i * rhs.cols + j] = sum;
            }
        }

        this->data = result.data;
        this->cols = result.cols;

        return *this;
    }

    /**
     * @brief Add another matrix to this matrix
     *
     * @param rhs :: Matrix to add to this matrix
     * @throws std::invalid_argument if the matrix dimensions do not match
     * @return Matrix& :: Reference to this matrix after addition
     */
    Matrix &add(Matrix &rhs) {
        if(this->cols != rhs.cols || this->rows != rhs.rows) {
            throw std::invalid_argument("FATAL: Incompatible matrix dimensions for addition.\n");
        }

        for(size_t i = 0; i < rhs.rows * rhs.cols; i++) {
            this->data[i] += rhs.data[i];
        }
        return *this;
    }

    /**
     * @brief Subtract another matrix from this matrix
     *
     * @param rhs :: Matrix to subtract from this matrix
     * @throws std::invalid_argument if the matrix dimensions do not match
     * @return Matrix& :: Reference to this matrix after subtraction
     */
    Matrix &subtract(Matrix &rhs) {

        if(this->cols != rhs.cols || this->rows != rhs.rows) {
            throw std::invalid_argument("FATAL: Incompatible matrix dimensions for subtraction.\n");
        }

        for(size_t i = 0; i < rhs.rows * rhs.cols; i++) {
            this->data[i] -= rhs.data[i];
        }
        return *this;
    }

    /**
     * @brief Apply a projection matrix to this matrix
     *
     * @param fov :: Field of view angle in radians
     * @param aspect :: Aspect ratio of the viewport
     * @param near :: Near clipping plane distance
     * @param far :: Far clipping plane distance
     * @return Matrix :: New matrix after projection
     */
    Matrix project(float fov, float aspect, float near, float far) {

        float f = 1.0f / std::tan(fov / 2.0f);
        float z = far / (far - near);

        Matrix projectionMatrix(4, 4, {
            f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, z, 1,
            0, 0, -z * near, 0
        });

        Matrix result = this->copy();

        return result.multiply(projectionMatrix);
    }


    Matrix translate(float x, float y, float z) {

        Matrix translation(4, 4, {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            x,y,z,1
        });

        Matrix result = this->copy();

        return result.multiply(translation);
    }



    Matrix perspective_divide(void) {

        Matrix result = this->copy();

        for(size_t i = 0; i < rows; i++) {

            float w = result.at(i, 3);

            if(w != 0.0f) {
                result.at(i, 0) /= w;
                result.at(i, 1) /= w;
                result.at(i, 2) /= w;
            }
        }

        return result;
    }

    Matrix viewport(int screenWidth, int screenHeight) {
        Matrix result = this->copy();

        for(size_t i = 0; i < rows; i++) {

            result.at(i,0) = (result.at(i,0) + 1.0f) * 0.5f * screenWidth;
            result.at(i,1) = (1.0f - result.at(i,1)) * 0.5f * screenHeight;
        }

        return result;
    }

    /**
     * @brief Rotate about the X axis
     *
     * @param angle :: Rotation angle in radians
     * @return Matrix :: Rotated matrix
     */
    Matrix rotate_x(float angle) {

        float c = std::cos(angle);
        float s = std::sin(angle);

        Matrix rotation(4, 4, {
            1, 0,  0, 0,
            0, c,  s, 0,
            0,-s,  c, 0,
            0, 0,  0, 1
        });

        Matrix result = this->copy();

        return result.multiply(rotation);
    }

    /**
     * @brief Rotate about the Y axis
     *
     * @param angle :: Rotation angle in radians
     * @return Matrix :: Rotated matrix
     */
    Matrix rotate_y(float angle) {

        float c = std::cos(angle);
        float s = std::sin(angle);

        Matrix rotation(4, 4, {
            c, 0,-s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1
        });

        Matrix result = this->copy();

        return result.multiply(rotation);
    }

    /**
     * @brief Rotate about the Z axis
     *
     * @param angle :: Rotation angle in radians
     * @return Matrix :: Rotated matrix
     */
    Matrix rotate_z(float angle) {

        float c = std::cos(angle);
        float s = std::sin(angle);

        Matrix rotation(4, 4, {
            c, s, 0, 0,
            -s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        });

        Matrix result = this->copy();

        return result.multiply(rotation);
    }

    /**
     * @brief Scale a matrix
     *
     * @param x :: Scale along X
     * @param y :: Scale along Y
     * @param z :: Scale along Z
     * @return Matrix :: Scaled matrix
     */
    Matrix scale(float x, float y, float z) {

        Matrix scaling(4, 4, {
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        });

        Matrix result = this->copy();

        return result.multiply(scaling);
    }
};

#endif
