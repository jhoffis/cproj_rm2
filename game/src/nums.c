#include "nums.h"

//4x4 matrices indices
#define	_11					0
#define	_12					1
#define	_13					2
#define	_14					3
#define	_21					4
#define	_22					5
#define	_23					6
#define	_24					7
#define	_31					8
#define	_32					9
#define	_33					10
#define	_34					11
#define	_41					12
#define	_42					13
#define	_43					14
#define	_44					15

void mat4x4_copy(f32_m4x4 output, f32_m4x4 input0) {
    memcpy(output, input0, sizeof(f32_m4x4));
}

void mat4x4_multiply(f32_m4x4 output, f32_m4x4 input0, f32_m4x4 input1) {
    f32_m4x4 work;
    
    work[_11] = input0[_11]*input1[_11] + input0[_12]*input1[_21] + input0[_13]*input1[_31] + input0[_14]*input1[_41];
    work[_12] = input0[_11]*input1[_12] + input0[_12]*input1[_22] + input0[_13]*input1[_32] + input0[_14]*input1[_42];
    work[_13] = input0[_11]*input1[_13] + input0[_12]*input1[_23] + input0[_13]*input1[_33] + input0[_14]*input1[_43];
    work[_14] = input0[_11]*input1[_14] + input0[_12]*input1[_24] + input0[_13]*input1[_34] + input0[_14]*input1[_44];
    work[_21] = input0[_21]*input1[_11] + input0[_22]*input1[_21] + input0[_23]*input1[_31] + input0[_24]*input1[_41];
    work[_22] = input0[_21]*input1[_12] + input0[_22]*input1[_22] + input0[_23]*input1[_32] + input0[_24]*input1[_42];
    work[_23] = input0[_21]*input1[_13] + input0[_22]*input1[_23] + input0[_23]*input1[_33] + input0[_24]*input1[_43];
    work[_24] = input0[_21]*input1[_14] + input0[_22]*input1[_24] + input0[_23]*input1[_34] + input0[_24]*input1[_44];
    work[_31] = input0[_31]*input1[_11] + input0[_32]*input1[_21] + input0[_33]*input1[_31] + input0[_34]*input1[_41];
    work[_32] = input0[_31]*input1[_12] + input0[_32]*input1[_22] + input0[_33]*input1[_32] + input0[_34]*input1[_42];
    work[_33] = input0[_31]*input1[_13] + input0[_32]*input1[_23] + input0[_33]*input1[_33] + input0[_34]*input1[_43];
    work[_34] = input0[_31]*input1[_14] + input0[_32]*input1[_24] + input0[_33]*input1[_34] + input0[_34]*input1[_44];
    work[_41] = input0[_41]*input1[_11] + input0[_42]*input1[_21] + input0[_43]*input1[_31] + input0[_44]*input1[_41];
    work[_42] = input0[_41]*input1[_12] + input0[_42]*input1[_22] + input0[_43]*input1[_32] + input0[_44]*input1[_42];
    work[_43] = input0[_41]*input1[_13] + input0[_42]*input1[_23] + input0[_43]*input1[_33] + input0[_44]*input1[_43];
    work[_44] = input0[_41]*input1[_14] + input0[_42]*input1[_24] + input0[_43]*input1[_34] + input0[_44]*input1[_44];

    // Output the result.
    mat4x4_copy(output, work);
}

void mat4x4_unit(f32_m4x4 output) {
    // Create a unit matrix.
    memset(output, 0, sizeof(f32_m4x4));
    output[_11] = 1.00f;
    output[_22] = 1.00f;
    output[_33] = 1.00f;
    output[_44] = 1.00f;
}

void mat4x4_translate(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1) {
    f32_m4x4 work;

    // Apply the translation.
    mat4x4_unit(work);
    work[_41] = input1.x;
    work[_42] = input1.y;
    work[_43] = input1.z;
    mat4x4_multiply(output, input0, work);
}

void mat4x4_rotate(f32_m4x4 output, f32_m4x4 input0, f32_v4 input1) {
    f32_m4x4 work;

    // Apply the z-axis rotation.
    mat4x4_unit(work);
    work[_11] =  cosf(input1.z);
    work[_12] =  sinf(input1.z);
    work[_21] = -sinf(input1.z);
    work[_22] =  cosf(input1.z);
    mat4x4_multiply(output, input0, work);
    
    // Apply the y-axis rotation.
    mat4x4_unit(work);
    work[_11] =  cosf(input1.y);
    work[_13] = -sinf(input1.y);
    work[_31] =  sinf(input1.y);
    work[_33] =  cosf(input1.y);
    mat4x4_multiply(output, output, work);

    // Apply the x-axis rotation.
    mat4x4_unit(work);
    work[_22] =  cosf(input1.x);
    work[_23] =  sinf(input1.x);
    work[_32] = -sinf(input1.x);
    work[_33] =  cosf(input1.x);
    mat4x4_multiply(output, output, work);
}

void create_world_view(f32_m4x4 world_view, f32_v4 translation, f32_v4 rotation) {
    f32_v4 work0, work1;

    // Reverse the translation.
    work0.x = -translation.x;
    work0.y = -translation.y;
    work0.z = -translation.z;
    work0.w =  translation.w;

    // Reverse the rotation.
    work1.x = -rotation.x;
    work1.y = -rotation.y;
    work1.z = -rotation.z;
    work1.w =  rotation.w;

    // Create the world_view matrix.
    mat4x4_unit(world_view);
    mat4x4_translate(world_view, world_view, work0);
    mat4x4_rotate(world_view, world_view, work1);
}

void mat4x4_perspective(f32_m4x4 mat, f32 fov_degrees, f32 aspect, f32 near, f32 far) {
    memset(mat, 0, sizeof(f32_m4x4));
    auto fov_radians = fov_degrees * (M_PI / 180.0f);
    f32 tan_half_fov = tanf(fov_radians / 2.0f);
    mat[_11] = aspect / ( tan_half_fov);
    mat[_22] = 1.0f / tan_half_fov;
    mat[_33] = -(far + near) / (far - near);
    mat[_34] = -1.0f;
    mat[_43] = -(2.0f * far * near) / (far - near);
}

void mat4x4_ortho(f32_m4x4 output, f32 left, f32 right, f32 bottom, f32 top) {
    // Start with identity matrix
    mat4x4_unit(output);
    
    // Scale
    output[0] = 2.0f / (right - left);
    output[5] = 2.0f / (top - bottom);
    output[10] = -1.0f;  // For standard OpenGL depth range [-1,1]
    
    // Translation
    output[12] = -(right + left) / (right - left);
    output[13] = -(top + bottom) / (top - bottom);
}
