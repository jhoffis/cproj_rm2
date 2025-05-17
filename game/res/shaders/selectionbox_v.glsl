#version 460 core

layout(binding = 0) uniform UniformBufferObject {
    vec2 posOriginal;
    vec2 posNew;
    vec2 posCam;
    vec2 resolution;
    float aspect;
} ubo;

layout(location = 0) in vec3 inPosition;
// layout(location = 1) in vec3 inColor;
// layout(location = 1) in vec2 inTexCoord;    

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 resolution;
layout(location = 2) out vec2 size;

vec2 positions[4] = vec2[](
vec2(-0.5, -0.5),
vec2(0.5, -0.5),
vec2(0.5, 0.5),
vec2(-0.5, 0.5)
);
//vec3 positions[4] = vec2[](
//vec3(-0.5, -0.5),
//vec3(0.5, -0.5),
//vec3(0.5, 0.5),
//vec3(-0.5, 0.5)
//);

void main() {
    //    gl_Position = vec4(inPosition.xyz, 1.0);// ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    //    gl_Position.x += ubo.pos.x;
    //    gl_Position.y += -ubo.pos.y;
    //    gl_Position.x /= ubo.aspect;
    //    gl_Position.x *= 2.;
    //    gl_Position.y *= 2.;
    //    gl_Position.x -= 1.;
    //    gl_Position.y += 1.;
    //zoom:
    //    gl_Position.x *= ubo.pos.z;
    //    gl_Position.y *= ubo.pos.z;

    resolution = ubo.resolution;

    switch (gl_VertexID) {
        case 0: // top left
        fragColor = vec3(1, 0, 0);
        gl_Position = vec4(ubo.posOriginal.x, -ubo.posOriginal.y, 0., 1.0);
        break;
        case 1: // top right
        fragColor = vec3(0, 1, 0);
        gl_Position = vec4(ubo.posNew.x, -ubo.posOriginal.y, 0.0, 1.0);
        break;
        case 2: // bottom right
        fragColor = vec3(0, 0, 1);
        gl_Position = vec4(ubo.posNew.x, -ubo.posNew.y, 0., 1.0);
        break;
        case 3: // bottom left
        fragColor = vec3(1, 1, 0);
        gl_Position = vec4(ubo.posOriginal.x, -ubo.posNew.y, 0.0, 1.0);
        break;
    }

    if (ubo.posOriginal.x >= ubo.posNew.x)
        size.x = ubo.posOriginal.x - ubo.posNew.x;
    else if (ubo.posOriginal.x < ubo.posNew.x)
        size.x = ubo.posNew.x - ubo.posOriginal.x;

    if (ubo.posOriginal.y >= ubo.posNew.y)
        size.y = ubo.posOriginal.y - ubo.posNew.y;
    else if (ubo.posOriginal.y < ubo.posNew.y)
        size.y = ubo.posNew.y - ubo.posOriginal.y;

    // gl_Position.x -= ubo.posCam.x;
    // gl_Position.y -= -ubo.posCam.y;
    // gl_Position.x /= ubo.aspect;
    // gl_Position.x *= 2.;
    // gl_Position.y *= 2.;
    // gl_Position.x -= 1.;
    // gl_Position.y += 1.;

    gl_Position.x = inPosition.x;
    gl_Position.y = inPosition.y;
    fragColor =  vec3(ubo.posNew, 1.0);
    // fragTexCoord = inTexCoord;
}
