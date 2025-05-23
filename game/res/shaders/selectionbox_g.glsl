// quad.geom
#version 460 core

/*─── layout ───*/
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

/*─── the same UBO you already use ───*/
layout(binding = 0) uniform UniformBufferObject {
    vec2 posOriginal;
    vec2 posNew;
    vec2 posCam;
    vec2 resolution;
    float aspect;
} ubo;

/*─── varyings that go to the fragment stage ───*/
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 vResolution;
layout(location = 2) out vec2 vSize;
layout(location = 3) out vec2 vCoord;

/* Helper: map from your 2-D world coords to clip space exactly like
   the bottom of your old vertex shader. */
vec4 toClipSpace(vec2 p)
{
    vec4 outPos = vec4(p.x, -p.y, 0.0, 1.0);

    outPos.x -= ubo.posCam.x;
    outPos.y += ubo.posCam.y;

    outPos.x /= ubo.aspect;
    outPos.x *= 2.0;
    outPos.y *= 2.0;

    outPos.x -= 1.0;
    outPos.y += 1.0;

    return outPos;
}

void main() {
    /* resolution is identical for every corne1r */
    vResolution = ubo.resolution;

    /* size is identical for every corner */
    vSize = abs(ubo.posNew - ubo.posOriginal);

    /*──────────────── 0 : top-right ────────────────*/
    vCoord = vec2(0, 0);
    fragColor   = vec3(1.0, 1.0, 1.0);
    gl_Position = toClipSpace(vec2(ubo.posNew.x,
                                   ubo.posOriginal.y));
    EmitVertex();

    /*──────────────── 1 : top-left ────────────────*/
    vCoord = vec2(1, 0);
    fragColor   = vec3(1.0, 0.0, 0.0);
    gl_Position = toClipSpace(vec2(ubo.posOriginal.x,
                                   ubo.posOriginal.y));   // note: +Y is up in your world
    EmitVertex();

    /*──────────────── 2 : bottom-right ────────────────*/
    vCoord = vec2(0, 1);
    fragColor   = vec3(0.0, 0.0, 1.0);
    gl_Position = toClipSpace(vec2(ubo.posNew.x,
                                   ubo.posNew.y));
    EmitVertex();

    /*──────────────── 3 : bottom-left ────────────────*/
    vCoord = vec2(1, 1);
    fragColor   = vec3(ubo.posNew, 1.0);          // exactly as in your case 3
    gl_Position = toClipSpace(vec2(ubo.posOriginal.x,
                                   ubo.posNew.y));
    EmitVertex();

    EndPrimitive();   // finishes the triangle strip
}

