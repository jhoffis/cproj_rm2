#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

layout(location = 1) uniform int anchor;
layout(location = 2) uniform float aspect;
layout(location = 3) uniform vec2 og_pos;
layout(location = 4) uniform vec2 pos;
layout(location = 5) uniform float offset;
layout(location = 6) uniform vec2 img_sizes;

#define ANCHOR_LEFT 0
#define ANCHOR_RIGHT 1
#define ANCHOR_MID 2
#define ANCHOR_MID_BOTTOM 3
#define ANCHOR_TOP_LEFT 4
#define ANCHOR_TOP_RIGHT 5

void main() {
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    gl_Position.x += pos.x;
    gl_Position.y += pos.y;
    gl_Position.x += offset;
    gl_Position.y += offset;
    if (anchor == ANCHOR_LEFT) {
        gl_Position.x += 2 * og_pos.x;
        gl_Position.y += 2 * og_pos.y;
        gl_Position.x *= aspect;
        gl_Position.x +=  - 1; // So that origo is at the bottom left and that it goes from 0 -> 1
        gl_Position.y +=  - 1;
    } else if (anchor == ANCHOR_RIGHT) {
        // gl_Position.x += vertex.x*aspect + 1; // So that origo is at the bottom left and that it goes from 0 -> 1
        // gl_Position.y += vertex.y + .5;
        gl_Position.x -= img_sizes.x;
        gl_Position.x += 2 * og_pos.x;
        gl_Position.y += 2 * og_pos.y;
        gl_Position.x *= aspect;
        gl_Position.x += 1;
        gl_Position.y += -1;
        // gl_Position.x += 2*vertex.x*aspect + 1; // So that origo is at the bottom left and that it goes from 0 -> 1
        // gl_Position.y += 2*pos.y + 1;
    } else if (anchor == ANCHOR_MID) {
        gl_Position.x -= .5*img_sizes.x;
        gl_Position.y -= .5*img_sizes.y;
        gl_Position.x += og_pos.x;
        gl_Position.y += og_pos.y;
        gl_Position.x *= aspect;
        // gl_Position.x += vertex.x*aspect; // So that origo is at the bottom left and that it goes from 0 -> 1
        // gl_Position.y += vertex.y;
    } else if (anchor == ANCHOR_MID_BOTTOM) {
        gl_Position.x -= .5*img_sizes.x;
        gl_Position.y += 2 * og_pos.y;
        gl_Position.x += og_pos.x;
        gl_Position.y += -1;
        gl_Position.x *= aspect;
    } else if (anchor == ANCHOR_TOP_LEFT) {
        gl_Position.y -= img_sizes.y;
        gl_Position.x += 2 * og_pos.x;
        gl_Position.y += 2 * og_pos.y;
        gl_Position.x *= aspect;
        gl_Position.x += -1;
        gl_Position.y += 1;
    } else if (anchor == ANCHOR_TOP_RIGHT) {
        gl_Position.x -= img_sizes.x;
        gl_Position.y -= img_sizes.y;
        gl_Position.x += 2 * og_pos.x;
        gl_Position.y += 2 * og_pos.y;
        gl_Position.x *= aspect;
        gl_Position.x += 1;
        gl_Position.y += 1;
    }
    TexCoords = vertex.zw;
}
