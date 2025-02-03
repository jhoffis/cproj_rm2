#include "text.h"
#include "allocator.h"
#include "str_util.h"
#include <GL/gl.h>
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

    FT_Set_Pixel_Sizes(face, 0, 48);  

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
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // disable byte-alignment restriction
}
