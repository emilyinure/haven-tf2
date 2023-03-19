#pragma once

enum e_font_flags
{
    font_flag_none,
    font_flag_italic = 0x001,
    font_flag_underline = 0x002,
    font_flag_strikeout = 0x004,
    font_flag_symbol = 0x008,
    font_flag_antialias = 0x010,
    font_flag_gaussianblur = 0x020,
    font_flag_rotary = 0x040,
    font_flag_dropshadow = 0x080,
    font_flag_additive = 0x100,
    font_flag_outline = 0x200,
    font_flag_custom = 0x400,
    font_flag_bitmap = 0x800,
};

class i_surface
{
    typedef void(__thiscall* start_drawing_fn)(void*);
    typedef void(__thiscall* finish_drawing_fn)(void*);

    enum e_indexes
    {
        DRAW_SET_COLOR = 11,
        DRAW_FILLED_RECT = 12,
        DRAW_OUTLINED_RECT = 14,
        DRAW_LINE = 15,
        DRAW_SET_TEXT_FONT = 17,
        DRAW_SET_TEXT_COLOR = 19,
        DRAW_SET_TEXT_POS = 20,
        DRAW_PRINT_TEXT = 22,
        UNLOCK_CURSOR = 61,
        CREATE_FONT = 66,
        SET_FONT_GLYPH_SET = 67,
        GET_TEXT_SIZE = 75
    };

public:
    __VFUNC(draw_set_color(color col), e_indexes::DRAW_SET_COLOR, void(__thiscall*)(void*, color), col);
    __VFUNC(draw_filled_rect(int x, int y, int x2, int y2), e_indexes::DRAW_FILLED_RECT,
            void(__thiscall*)(void*, int, int, int, int), x, y, x2, y2);
    __VFUNC(draw_outlined_rect(int x, int y, int x2, int y2), e_indexes::DRAW_OUTLINED_RECT,
            void(__thiscall*)(void*, int, int, int, int), x, y, x2, y2);
    __VFUNC(draw_line(int x, int y, int x2, int y2), e_indexes::DRAW_LINE, void(__thiscall*)(void*, int, int, int, int),
            x, y, x2, y2);
    __VFUNC(create_font(), e_indexes::CREATE_FONT, unsigned long(__thiscall*)(void*));
    __VFUNC(set_font_glyph_set(unsigned long font, const char* font_name, int tall, int weight, int flags),
            e_indexes::SET_FONT_GLYPH_SET,
            bool(__thiscall*)(void*, unsigned long, const char*, int, int, int, int, int, int, int), font, font_name,
            tall, weight, 0, 0, flags, 0, 0);
    __VFUNC(draw_set_text_font(unsigned long font), e_indexes::DRAW_SET_TEXT_FONT,
            void(__thiscall*)(void*, unsigned long), font);
    __VFUNC(draw_set_text_color(color col), e_indexes::DRAW_SET_TEXT_COLOR, void(__thiscall*)(void*, color), col);
    __VFUNC(draw_set_text_pos(int x, int y), e_indexes::DRAW_SET_TEXT_POS, void(__thiscall*)(void*, int, int), x, y);
    __VFUNC(draw_print_text(const wchar_t* text, int text_length), e_indexes::DRAW_PRINT_TEXT,
            void(__thiscall*)(void*, const wchar_t*, int, int), text, text_length, 0);
    __VFUNC(get_text_size(unsigned long font, const wchar_t* text, int& wide, int& tall), e_indexes::GET_TEXT_SIZE,
            void(__thiscall*)(void*, unsigned long, const wchar_t*, int&, int&), font, text, wide, tall);
    __VFUNC(unlock_cursor(), e_indexes::UNLOCK_CURSOR, void(__thiscall*)(void*));

    void start_drawing();
    void finish_drawing();
};
