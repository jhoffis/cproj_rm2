#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 ourColor2;
out vec2 TexCoord;

layout(location = 1) uniform vec2 pos;
layout(location = 2) uniform float aspect;
layout(location = 3) uniform float img_ratio;
layout(location = 4) uniform float scale;
layout(location = 5) uniform int anchor;

#define ANCHOR_LEFT 0
#define ANCHOR_RIGHT 1
#define ANCHOR_MID 2
#define ANCHOR_MID_BOTTOM 3
#define ANCHOR_TOP_LEFT 4
#define ANCHOR_TOP_RIGHT 5

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);
    gl_Position.x *= scale;
    gl_Position.y *= scale;
    gl_Position.x *= aspect;
    gl_Position.x *= img_ratio;
    if (anchor == ANCHOR_LEFT) {
        gl_Position.x += 2*pos.x*aspect - 1; // So that origo is at the bottom left and that it goes from 0 -> 1
        gl_Position.y += 2*pos.y - 1;
    } else if (anchor == ANCHOR_RIGHT) {
        gl_Position.x -= img_ratio * aspect * scale;
        gl_Position.x += 2*pos.x*aspect + 1; // So that origo is at the bottom left and that it goes from 0 -> 1
        gl_Position.y += 2*pos.y - 1;
    } else if (anchor == ANCHOR_MID) {
        gl_Position.x -= .5*img_ratio * aspect * scale;
        gl_Position.y -= .5 * scale;
        gl_Position.x += pos.x*aspect; // So that origo is at the bottom left and that it goes from 0 -> 1
        gl_Position.y += pos.y;
    } else if (anchor == ANCHOR_MID_BOTTOM) {
        gl_Position.x -= .5*img_ratio * aspect * scale;
        gl_Position.x += pos.x*aspect; // So that origo is at the bottom left and that it goes from 0 -> 1
        gl_Position.y += 2*pos.y - 1;
    } else if (anchor == ANCHOR_TOP_LEFT) {
        gl_Position.y -= scale;
        gl_Position.x += 2*pos.x*aspect - 1; // So that origo is at the bottom left and that it goes from 0 -> 1
        gl_Position.y += 2*pos.y + 1;
    } else if (anchor == ANCHOR_TOP_RIGHT) {
        gl_Position.x -= img_ratio * aspect * scale;
        gl_Position.y -= scale;
        gl_Position.x += 2*pos.x*aspect + 1; // So that origo is at the bottom left and that it goes from 0 -> 1
        gl_Position.y += 2*pos.y + 1;
    }
    ourColor2 = vec3(1);
    TexCoord = aTexCoord;
}
