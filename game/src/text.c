#include "text.h"
#include "allocator.h"
#include "str_util.h"
#include <ft2build.h>
#include FT_FREETYPE_H  

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
}
