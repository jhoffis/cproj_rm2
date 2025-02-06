#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

layout (location = 1) uniform mat4 projection;
layout (location = 2) uniform float aspect;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    // gl_Position.x *= aspect / (800 / 600);
    TexCoords = vertex.zw;
}
