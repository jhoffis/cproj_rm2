#pragma once
#include "nums.h"
#define GFX_TRIANGLES 4
#define GFX_WIRE 2
typedef u32 gfx_method;

void gfx_init_graphics(void);
void gfx_set_depth(bool depth);
void gfx_update_viewport(void);
void gfx_clear_color(f32 r, f32 g, f32 b, f32 a);
void gfx_swap(void);
void gfx_temp_disable_culling(void);
void gfx_temp_reenable_culling(void);
