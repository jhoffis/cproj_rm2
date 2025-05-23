#include "selection_box.h"
#include "game_state.h"
#include "shader.h"
#include <GL/gl.h>
// #include "camera.h"
// #include "rendering/Model.h"
// #include "Villager.h"
// #include "shaders/Shaders.h"
// #include "vk/presentation/gra_swap_chain.h"

typedef struct {
    alignas(8) f32_v2 pos_og;
    alignas(8) f32_v2 pos_new;
    alignas(8) f32_v2 pos_cam;
    alignas(8) f32_v2 resolution;
    alignas(4) f32 aspect;
} sel_box_UBO_t;

static sel_box_UBO_t sb_ubo = {0};
static bool visible = false;
static u32 buffer;

void sel_box_init(void) {
    f32_v3 vertices[] = {
       // { 1.0f, -0.0f, 0.0f},
       {-0.0f, -0.0f, 0.0f},
       // { 1.0f,  1.0f, 0.0f},
       // {-0.0f,  1.0f, 0.0f},
    };    

    u32 indices[] = {  // note that we start from 0!
        0
    };  
    gfx_bind_vertices(shader_selection_box, vertices, 1, indices, 1);

    buffer = gfx_create_buffer();
}

void sel_box_render(void) {
    if (!visible) return;
    gfx_temp_disable_culling();
    sb_ubo.aspect = game_state.window.aspect;
    sb_ubo.resolution.x = game_state.window.width; // kanskje monitor size istedet?
    sb_ubo.resolution.y = game_state.window.height;
    sb_ubo.pos_cam.x = game_state.cam2D.x;
    sb_ubo.pos_cam.y = game_state.cam2D.y;
    gfx_set_shader(shader_selection_box);
    gfx_uniform_void(shader_selection_box, 
            buffer, "UniformBufferObject", 0, (void*) &sb_ubo, sizeof(sel_box_UBO_t));
    glDrawArrays(GL_POINTS, 0, 1);        // count = 1  ← minimum;
    gfx_temp_reenable_culling();
}

void sel_box_start_dragging(const f32_v2 world_pos) {
    visible = true;
    sb_ubo.pos_og.x  = world_pos.x;
    sb_ubo.pos_og.y  = world_pos.y;
    sb_ubo.pos_new.x = world_pos.x;
    sb_ubo.pos_new.y = world_pos.y;
}

void sel_box_drag(const f32_v2 world_pos) {
    sb_ubo.pos_new.x = world_pos.x; 
    sb_ubo.pos_new.y = world_pos.y; 
}

void sel_box_stop_dragging(bool select) {
    visible = false;
    // m_selected.clear();
    
    if (!select) return;

    float x0, y0, x1, y1;

    if (sb_ubo.pos_og.x < sb_ubo.pos_new.x) {
        x0 = sb_ubo.pos_og.x;
        x1 = sb_ubo.pos_new.x;
    } else {
        x1 = sb_ubo.pos_og.x;
        x0 = sb_ubo.pos_new.x;
    }

    if (sb_ubo.pos_og.y < sb_ubo.pos_new.y) {
        y0 = sb_ubo.pos_og.y;
        y1 = sb_ubo.pos_new.y;
    } else {
        y1 = sb_ubo.pos_og.y;
        y0 = sb_ubo.pos_new.y;
    }

    // Villager::unselectAll();
    // Villager::villsWithinBounds(x0, y0, x1, y1, true);
    //        for (auto &vill : vills) {
    //            vill->entity->selected = true;
    //            std::cout << "vill: " << vill->age << std::endl;
    //        }
}

bool sel_box_is_visible(void) {
    return visible;
}

void sel_box_destroy(void) {
    // Shaders::m_selectionBoxModel.destroy();
}
