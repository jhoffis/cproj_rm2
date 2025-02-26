#include "main_scene.h"
#include "btn.h"
#include "game_state.h"
#include "input_handler.h"
#include "nums.h"
#include "renderer.h"
#include "scenes.h"
#include "shader.h"
#include "sprite2d.h"
#include "timer_util.h"
#include "window.h"

static void test_btn(void) {
    printf("click\n");
    // change_scene(scene_options, false);
}

static image_data img;
static image_data img_tire;
static u32 yinyang;
static f32_v2 pos;

void main_scene_init(void) {
    yinyang = sprite2D_create("yinyang", 0);
    img = load_image("DecentraPaint");
    gfx_finalize_image(&img);
    img_tire = load_image("Tires");
    gfx_finalize_image(&img_tire);
}

void main_scene_render(void) {

        f32 x = sin(game_state.cam_rot.y);
        f32 z = cos(game_state.cam_rot.y);
        f32 moveX = move_left + move_right;
        f32 moveY = move_up + move_down;
        f32 moveZ = move_forward + move_back;
        // Forward and backwards + side to side
        game_state.cam_pos.x += (x * moveZ) + (z * moveX);
        game_state.cam_pos.z += (z * moveZ) - (x * moveX); 
        game_state.cam_pos.y += moveY; 

        f32_m4x4 mvp = {0};
        mat4x4_unit(mvp);
        mat4x4_translate(mvp, mvp, (f32_v4) {0,0.5,-15.0,0}); 
        // pos -= 0.1;

        f32_m4x4 view = {0};
        create_world_view(view, game_state.cam_pos, game_state.cam_rot);

        f32_m4x4 persp = {0};
        mat4x4_perspective(persp, 70, window_aspect_ratio(), 1, 10000);
        // printf("%f\n", rot);
        mat4x4_multiply(mvp, mvp, view);
        mat4x4_multiply(mvp, mvp, persp);

        gfx_set_depth(true);
        // Draw 3D model
        gfx_set_shader(shader_mesh3d);
        gfx_activate_texture(0, img.texture); 
        gfx_uniform_f32_mat4x4(1, mvp);
        gfx_draw();

        gfx_set_shader(shader_tire_mesh3d);
        gfx_activate_texture(0, img_tire.texture); 
        gfx_uniform_f32_mat4x4(1, mvp);
        gfx_draw();

        gfx_set_depth(false);
        // sprite2D_draw(yinyang);
        pos.x = 0.2;
        pos.y = 0.2;
        // pos.y += timer_delta();
        // if (pos.y > 10) {
        //     pos.y = 0;
        // }
        render_btn("Button", pos, test_btn, anchor_left);
        render_btn("Play", (f32_v2){0}, test_btn, anchor_mid);
        render_btn("asdflasd", (f32_v2){-.3, 0.2}, test_btn, anchor_right);
        // render_text("Hello world!", 100, 100, 1, (f32_v3) {.2, 0, 0.5});
        // render_text("yoyoyo!", 200, 400, 1, (f32_v3) {.2, 0, 0.5});

}
