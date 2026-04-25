#pragma once
#include <cmath>
#include "Vector3.h"
#include "Quat.h"

// Column-major matrix
// Memory: 0-3 = col0, 4-7 = col1, 8-11 = col2, 12-15 = col3
// Multiply as: M * v  (column vector on right)
// TRS order: T * R * S
// Upload to GLSL/MSL: memcpy directly, no transpose needed

class Mat4 {
public:
    float data[16];

    Mat4() {
        for (int i = 0; i < 16; i++) data[i] = 0.0f;
        data[0] = data[5] = data[10] = data[15] = 1.0f;
    }

    // Multiplication
    Mat4 operator*(const Mat4& other) const {
        Mat4 r;
        for (int col = 0; col < 4; col++)
            for (int row = 0; row < 4; row++) {
                r.data[col * 4 + row] = 0;
                for (int k = 0; k < 4; k++)
                    r.data[col * 4 + row] += data[k * 4 + row] * other.data[col * 4 + k];
            }
        return r;
    }

    // Translation
    // Column-major: Tx Ty Tz live in last COLUMN -> indices 12, 13, 14
    //  1  0  0  Tx
    //  0  1  0  Ty
    //  0  0  1  Tz
    //  0  0  0   1
    static Mat4 translate(const Vector3& v) {
        Mat4 m;
        m.data[12] = v.x;
        m.data[13] = v.y;
        m.data[14] = v.z;
        return m;
    }

    // Scale
    static Mat4 scale(const Vector3& v) {
        Mat4 m;
        m.data[0] = v.x;
        m.data[5] = v.y;
        m.data[10] = v.z;
        return m;
    }

    // Rotation X
    //  1   0   0   0
    //  0   c  -s   0
    //  0   s   c   0
    //  0   0   0   1
    // col0=[1,0,0,0] col1=[0,c,s,0] col2=[0,-s,c,0] col3=[0,0,0,1]
    static Mat4 rotateX(float deg) {
        float c = cos(radians(deg)), s = sin(radians(deg));
        Mat4 m;
        m.data[5] = c; m.data[6] = s;
        m.data[9] = -s; m.data[10] = c;
        return m;
    }

    // Rotation Y
    //  c   0   s   0
    //  0   1   0   0
    // -s   0   c   0
    //  0   0   0   1
    // col0=[c,0,-s,0] col1=[0,1,0,0] col2=[s,0,c,0] col3=[0,0,0,1]
    static Mat4 rotateY(float deg) {
        float c = cos(radians(deg)), s = sin(radians(deg));
        Mat4 m;
        m.data[0] = c; m.data[2] = -s;
        m.data[8] = s; m.data[10] = c;
        return m;
    }

    // Rotation Z
    //  c  -s   0   0
    //  s   c   0   0
    //  0   0   1   0
    //  0   0   0   1
    // col0=[c,s,0,0] col1=[-s,c,0,0] col2=[0,0,1,0] col3=[0,0,0,1]
    static Mat4 rotateZ(float deg) {
        float c = cos(radians(deg)), s = sin(radians(deg));
        Mat4 m;
        m.data[0] = c; m.data[1] = s;
        m.data[4] = -s; m.data[5] = c;
        return m;
    }

    // Perspective (right-handed, depth -1 to 1 for GL/Vulkan)
    static Mat4 perspective(float fovDeg, float aspect, float near, float far) {
        float f = 1.0f / tan(radians(fovDeg) / 2.0f);
        Mat4 m;
        m.data[0] = f / aspect;
        m.data[5] = f;
        m.data[10] = (far + near) / (near - far);
        m.data[11] = -1.0f;
        m.data[14] = (2.0f * far * near) / (near - far);
        m.data[15] = 0.0f;
        return m;
    }

    // Orthographic
    static Mat4 ortho(float left, float right, float bottom, float top, float near, float far) {
        Mat4 m;
        m.data[0] = 2.0f / (right - left);
        m.data[5] = 2.0f / (top - bottom);
        m.data[10] = -2.0f / (far - near);
        m.data[12] = -(right + left) / (right - left);
        m.data[13] = -(top + bottom) / (top - bottom);
        m.data[14] = -(far + near) / (far - near);
        m.data[15] = 1.0f;
        return m;
    }

    // LookAt
    // Column-major: basis vectors go in COLUMNS
    //  rx  ux  -fx  -r.eye
    //  ry  uy  -fy  -u.eye
    //  rz  uz  -fz   f.eye
    //   0   0    0    1
    static Mat4 lookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
        Vector3 f = (center - eye).normalized();           // forward
        Vector3 r = f.cross(up).normalized();                      // right
        Vector3 u = r.cross(f).normalized();                       // actual up (corrected)

        Mat4 m;  // starts as identity

        m.data[0] = r.x;   m.data[1] = u.x;   m.data[2] = -f.x;   m.data[3] = 0.0f;
        m.data[4] = r.y;   m.data[5] = u.y;   m.data[6] = -f.y;   m.data[7] = 0.0f;
        m.data[8] = r.z;   m.data[9] = u.z;   m.data[10] = -f.z;   m.data[11] = 0.0f;

        m.data[12] = -r.dot(eye);
        m.data[13] = -u.dot(eye);
        m.data[14] = f.dot(eye);   // note + here because we use -f as forward direction
        m.data[15] = 1.0f;
        return m;
    }

    // From Quaternion
    static Mat4 fromQuat(const Quat& q) {
        float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
        float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
        float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

        Mat4 m;
        // col0
        m.data[0] = 1 - 2 * (yy + zz);
        m.data[1] = 2 * (xy + wz);
        m.data[2] = 2 * (xz - wy);
        m.data[3] = 0;
        // col1
        m.data[4] = 2 * (xy - wz);
        m.data[5] = 1 - 2 * (xx + zz);
        m.data[6] = 2 * (yz + wx);
        m.data[7] = 0;
        // col2
        m.data[8] = 2 * (xz + wy);
        m.data[9] = 2 * (yz - wx);
        m.data[10] = 1 - 2 * (xx + yy);
        m.data[11] = 0;
        // col3
        m.data[12] = 0;
        m.data[13] = 0;
        m.data[14] = 0;
        m.data[15] = 1;
        return m;
    }

    // TRS combined
    // Column-major: T * R * S
    // Applies scale first, then rotation, then translation (right to left)
    static Mat4 trs(const Vector3& pos, const Quat& rot, const Vector3& scl) {
        return translate(pos) * fromQuat(rot) * scale(scl);
    }

    // Vector multiply: M * v  (column vector on right)
    Vector3 multiplyVec(const Vector3& v, float w = 1.0f) const {
        Vector3 r;
        r.x = data[0] * v.x + data[4] * v.y + data[8] * v.z + data[12] * w;
        r.y = data[1] * v.x + data[5] * v.y + data[9] * v.z + data[13] * w;
        r.z = data[2] * v.x + data[6] * v.y + data[10] * v.z + data[14] * w;
        return r;
    }

    float* getData() { return data; }
    const float* getData() const { return data; }

    static float radians(float deg) { return deg * 3.14159265359f / 180.0f; }
};