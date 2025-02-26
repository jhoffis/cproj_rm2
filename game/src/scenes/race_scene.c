#include "race_scene.h"
#include "car.h"
#include "game_state.h"
#include "mesh3d.h"
#include "nums.h"
#include "renderer.h"
#include "shader.h"
#include "sprite2d.h"
#include "text.h"
#include "window.h"
#include <stdio.h>
#include <stdlib.h>

static image_data img;
static image_data img_tire;
static u32 tachometer;

void race_init(u32 my_car) {
    // Load model
    auto model_mesh = load_model("oldsroyal");
    if (!model_mesh) {
        printf("Failed to load model\n");
        exit(1);
    }

    gfx_set_shader(shader_oldsroyal_mesh3d);
    bind_model_group(model_mesh, 0);

    gfx_set_shader(shader_oldsroyal_tire_mesh3d);
    bind_model_from_group(model_mesh, 1);

    img = load_image("OldsroyalPaint");
    gfx_finalize_image(&img);
    img_tire = load_image("Tires");
    gfx_finalize_image(&img_tire);
    destroy_model(model_mesh);

    tachometer = sprite2D_create("tachometer", 0);
}

void race_scene_render(void) {

    game_state.cam_pos.x = 0.5;
    game_state.cam_pos.y = .7;
    game_state.cam_pos.z = -.12;
    game_state.cam_rot.y = M_PI / 2;
    game_state.cam_rot.x = 0;

    f32_m4x4 mvp = {0};
    mat4x4_unit(mvp);
    mat4x4_translate(mvp, mvp, (f32_v4) {0,0.,0,0}); 
    // pos -= 0.1;

    f32_m4x4 view = {0};
    create_world_view(view, game_state.cam_pos, game_state.cam_rot);

    f32_m4x4 persp = {0};
    mat4x4_perspective(persp, 70, window_aspect_ratio(), .1, 10000);
    // printf("%f\n", rot);
    mat4x4_multiply(mvp, mvp, view);
    mat4x4_multiply(mvp, mvp, persp);


    gfx_set_depth(true);
    // Draw 3D model
    gfx_set_shader(shader_oldsroyal_mesh3d);
    gfx_activate_texture(0, img.texture); 
    gfx_uniform_f32_mat4x4(1, mvp);
    gfx_draw();

    gfx_set_shader(shader_oldsroyal_tire_mesh3d);
    gfx_activate_texture(0, img_tire.texture); 
    gfx_uniform_f32_mat4x4(1, mvp);
    gfx_draw();

    gfx_set_depth(false);
    sprite2D_scale(tachometer, 0.75);
    sprites[tachometer].anchor = anchor_right;
    sprite2D_draw(tachometer);


    char speed_str[50];
    sprintf(speed_str, "%d%s", (i32) my_car.speed, " km/h");

    render_text(speed_str, (f32_v2) {-.1, .1}, anchor_right);
}
