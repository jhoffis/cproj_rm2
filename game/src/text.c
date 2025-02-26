#include "text.h"
#include "allocator.h"
#include "game_state.h"
#include "shader.h"
#include "str_util.h"
#include <glad/glad.h> // TODO move this to renderer/shader
#include "window.h"
#include <ft2build.h>
#include FT_FREETYPE_H  

typedef struct {
    u32 textureId, advance;
    i32_v2 size, bearing;
} character_t;

static character_t chars[128];

void init_text(void) {

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        printf("ERROR::FREETYPE: Could not init FreeType Library\n");
        exit(1);
    }
    
    FT_Face face;
    char *font_path = path_name("../res/fonts/", "arial", ".ttf");
    auto failed = FT_New_Face(ft, font_path, 0, &face);
    xfree(font_path);
    if (failed) {
        printf("ERROR::FREETYPE: Failed to load font\n");
        exit(1);
    }

    FT_Set_Pixel_Sizes(face, 0, 56);  

    printf("finished initializing text\n");

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    for (u8 c = 0; c < 128; c++) {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("ERROR::FREETYTPE: Failed to load Glyph\n");
            continue;
        }
        // generate texture
        u32 texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
                );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        character_t character = {
            texture, 
            face->glyph->advance.x,
            (i32_v2) {face->glyph->bitmap.width, face->glyph->bitmap.rows},
            (i32_v2) {face->glyph->bitmap_left, face->glyph->bitmap_top},
        };
        chars[c] = character;
    } 
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    auto vao = shaders[shader_text].vertex_attr_buffer;
    auto vbo = shaders[shader_text].vertex_buffer;
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);      

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // disable byte-alignment restriction
}

static void _render_text(const char *text, 
                 f32_v2 pos, 
                 f32 scale, 
                 f32 offset,
                 f32_v3 color, 
                 f32_v2 img_sizes, 
                 sprite2D_anchor anchor) {
    gfx_set_shader(shader_text);
    gfx_uniform_f32_v3(0, color);
    gfx_uniform_i8(1, anchor);
    gfx_uniform_f32(2, window_aspect_ratio());
    gfx_uniform_f32_v2(3, pos);
    gfx_uniform_f32(5, offset);
    gfx_uniform_f32_v2(6, img_sizes);
    gfx_activate_texture_pipe(0);
    // scale = window_aspect_ratio();
    // scale /= 600;
    // pos.x /= window_aspect_ratio();
    pos = (f32_v2){0};
    for (int i = 0; text[i] != '\0'; i++) {
        auto ch = chars[text[i]];

        f32 xpos = pos.x + ch.bearing.x * scale;
        f32 ypos = pos.y - (ch.size.y - ch.bearing.y) * scale;

        f32 w = ch.size.x * scale;
        f32 h = ch.size.y * scale;

        f32 vertices[6][4] = {
            {0,     0 + h, 0, 0},
            {0,     0,     0, 1},
            {0 + w, 0,     1, 1},

            {0,     0 + h, 0, 0},
            {0 + w, 0,     1, 1},
            {0 + w, 0 + h, 1, 0},
        };
        gfx_bind_texture2(ch.textureId);
        glBindBuffer(GL_ARRAY_BUFFER, shaders[shader_text].vertex_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        gfx_uniform_f32_v2(4, (f32_v2){xpos, ypos});
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        pos.x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    gfx_bind_texture2(0);
}


void render_btn_text(const char *text, 
                 f32_v2 pos, 
                 f32 scale, 
                 f32_v3 color, 
                 f32_v2 img_sizes, 
                 sprite2D_anchor anchor) {
    _render_text(text, pos, scale, scale * 80, color, img_sizes, anchor);
}

void render_text(const char *text, 
                 f32_v2 pos, 
                 sprite2D_anchor anchor) {
    f32 scale = .1 / 80;
    f32_v2 text_sizes = {0};
    character_t ch;
    for (int i = 0; text[i] != '\0'; i++) {
        ch = chars[text[i]];
        f32 h = ch.size.y * scale;
        if (h > text_sizes.y) {
            text_sizes.y = h;
        }
        text_sizes.x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    text_sizes.x += ch.size.x * scale;

    _render_text(text, pos, scale, 0, (f32_v3) {1, 1, 1}, text_sizes, anchor);
}




