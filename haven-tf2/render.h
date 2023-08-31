#pragma once

enum e_text_alignment
{
    text_align_left,
    text_align_center,
    text_align_right,
};
class Vertex_t;
class c_render
{
    static int create_font(const char* name, int size, int weight, int flags = e_font_flags::font_flag_none);

public:
    struct
    {
        unsigned long main;
        unsigned long secondary;

        struct
        {
            unsigned long name;
            unsigned long flags;
        } esp;

        struct
        {
            unsigned long main;
            unsigned long title;
            unsigned long tabs;
            unsigned long tooltip;
        } menu;
    } m_fonts;

    void init();
    static void filled_rect(box_t box, color col);
    static void filled_rect(vector_2d pos, vector_2d size, color col);
    static void outlined_rect(box_t box, color col);
    static void outlined_rect(vector_2d pos, vector_2d size, color col);
    static void line(vector_2d from, vector_2d to, color col);
    static void box(const vector from, const vector to, const color col);
    static void text(unsigned long font, vector_2d pos, const char* text, color col,
                     e_text_alignment alignment = e_text_alignment::text_align_left);
    static vector_2d get_text_size(const char* text, unsigned long font);

    static void render_verts(int count, Vertex_t* vertexes, color c);
};
inline c_render g_render;
