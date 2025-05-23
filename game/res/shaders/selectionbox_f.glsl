#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 resolution;
layout(location = 2) in vec2 size;
layout(location = 3) in vec2 coord;

layout(location = 0) out vec4 outColor;

void main() {

    const float borderSize = 3.0;

    float sizePixelX = size.x * resolution.y;
    float sizePixelY = size.y * resolution.y;

    vec2 pixelInBox = vec2(
    coord.x * sizePixelX,
    coord.y * sizePixelY
    );

    if (pixelInBox.x <= borderSize
    || pixelInBox.y <= borderSize
    || sizePixelX - pixelInBox.x <= borderSize
    || sizePixelY - pixelInBox.y <= borderSize
    ) {
        outColor = vec4(1);
    } else {
        outColor = vec4(1, 0.2, 0.6, 0.15);
    }
    // outColor = vec4(fragColor.rgb, 1.0);
}
