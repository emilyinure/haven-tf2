#include "sdk.h"
#include "math.h"

void c_render::init()
{
    this->m_fonts.main = create_font("Tahoma", 12, 300, e_font_flags::font_flag_dropshadow);
    this->m_fonts.secondary = create_font("Tahoma", 11, 300, e_font_flags::font_flag_dropshadow);
    
    // menu.
    this->m_fonts.menu.main = create_font("Tahoma", 12, 200, e_font_flags::font_flag_antialias);
    this->m_fonts.menu.title = create_font("Segoe UI", 13, 200, e_font_flags::font_flag_antialias);
    this->m_fonts.menu.tabs =
        create_font("Segoe UI", 18, 200, e_font_flags::font_flag_antialias | e_font_flags::font_flag_dropshadow);
    this->m_fonts.menu.tooltip = create_font("Tahoma", 13, 300, e_font_flags::font_flag_dropshadow);
}

void c_render::filled_rect(const box_t box, const color col)
{
    g_interfaces.m_surface->draw_set_color(col);
    g_interfaces.m_surface->draw_filled_rect(box.m_x, box.m_y, box.m_x + box.m_w, box.m_y + box.m_h);
}

void c_render::filled_rect(const vector_2d pos, const vector_2d size, const color col)
{
    c_render::filled_rect({pos.m_x, pos.m_y, size.m_x, size.m_y}, col);
}

void c_render::outlined_rect(const box_t box, const color col)
{
    g_interfaces.m_surface->draw_set_color(col);
    g_interfaces.m_surface->draw_outlined_rect(box.m_x, box.m_y, static_cast<int>(box.m_x + box.m_w),
                                               static_cast<int>(box.m_y + box.m_h));
}

void c_render::outlined_rect(const vector_2d pos, const vector_2d size, const color col)
{
    outlined_rect({pos.m_x, pos.m_y, size.m_x, size.m_y}, col);
}

void c_render::line(const vector_2d from, const vector_2d to, const color col)
{
    g_interfaces.m_surface->draw_set_color(col);
    g_interfaces.m_surface->draw_line(from.m_x, from.m_y, to.m_x, to.m_y);
}

void c_render::box(const vector from, const vector to, const color col)
{
    vector from_right = from, from_forward = from, from_up = from;
    vector to_forward = to, to_right = to, to_down = to;
    to_down.m_z = from.m_z;
    from_up.m_z = to.m_z;

    from_forward.m_x = to.m_x;
    to_forward.m_y = from.m_y;

    from_right.m_y = to.m_y;
    to_right.m_x = from.m_x;

    vector from_screen, to_screen, from_right_screen, from_forward_screen, from_up_screen, to_forward_screen, to_right_screen, to_down_screen;

    if (!math::world_to_screen(from, from_screen) ||
        !math::world_to_screen(to, to_screen) ||
        !math::world_to_screen(from_right, from_right_screen) ||
        !math::world_to_screen(from_forward, from_forward_screen) ||
        !math::world_to_screen(from_up, from_up_screen) ||
        !math::world_to_screen(to_right, to_right_screen) ||
        !math::world_to_screen(to_forward, to_forward_screen) ||
        !math::world_to_screen(to_down, to_down_screen))
        return;

    g_interfaces.m_surface->draw_set_color(col);
    g_interfaces.m_surface->draw_line(from_screen.m_x, from_screen.m_y, from_right_screen.m_x, from_right_screen.m_y);
    g_interfaces.m_surface->draw_line(from_screen.m_x, from_screen.m_y, from_forward_screen.m_x, from_forward_screen.m_y);
    g_interfaces.m_surface->draw_line(from_screen.m_x, from_screen.m_y, from_up_screen.m_x, from_up_screen.m_y);

    g_interfaces.m_surface->draw_line(to_screen.m_x, to_screen.m_y, to_right_screen.m_x, to_right_screen.m_y);
    g_interfaces.m_surface->draw_line(to_screen.m_x, to_screen.m_y, to_forward_screen.m_x, to_forward_screen.m_y);
    g_interfaces.m_surface->draw_line(to_screen.m_x, to_screen.m_y, to_down_screen.m_x, to_down_screen.m_y);

    g_interfaces.m_surface->draw_line(to_down_screen.m_x, to_down_screen.m_y, from_right_screen.m_x,
                                      from_right_screen.m_y);
    g_interfaces.m_surface->draw_line(to_down_screen.m_x, to_down_screen.m_y, from_forward_screen.m_x,
                                      from_forward_screen.m_y);

    g_interfaces.m_surface->draw_line(from_up_screen.m_x, from_up_screen.m_y, to_right_screen.m_x, to_right_screen.m_y);
    g_interfaces.m_surface->draw_line(from_up_screen.m_x, from_up_screen.m_y, to_forward_screen.m_x,
                                      to_forward_screen.m_y);

    g_interfaces.m_surface->draw_line(from_forward_screen.m_x, from_forward_screen.m_y, to_forward_screen.m_x,
                                      to_forward_screen.m_y);
    g_interfaces.m_surface->draw_line(from_right_screen.m_x, from_right_screen.m_y, to_right_screen.m_x,
                                      to_right_screen.m_y);

}

void c_render::text(const unsigned long font, vector_2d pos, const char* text, const color col,
                    const e_text_alignment alignment)
{
    if (!font)
        return;

    wchar_t buf[2056] = {};

    const auto text_size = get_text_size(text, font);

    // todo, maybe vertical alignment?
    switch (alignment)
    {
        case e_text_alignment::text_align_center:
            pos.m_x -= text_size.m_x * 0.5;
            break;
        case e_text_alignment::text_align_right:
            pos.m_x -= text_size.m_x;
            break;
        default:
            break;
    }

    if (MultiByteToWideChar(CP_UTF8, 0, text, -1, buf, 128) > 0)
    {
        g_interfaces.m_surface->draw_set_text_font(font);
        g_interfaces.m_surface->draw_set_text_color(col);
        g_interfaces.m_surface->draw_set_text_pos(pos.m_x, pos.m_y);
        g_interfaces.m_surface->draw_print_text(buf, wcslen(buf));
    }
}

vector_2d c_render::get_text_size(const char* text, const unsigned long font)
{
    if (!font)
        return {};

    wchar_t buf[2056] = {};

    if (MultiByteToWideChar(CP_UTF8, 0, text, -1, buf, 128) > 0)
    {
        auto width = 0, height = 0;

        g_interfaces.m_surface->get_text_size(font, buf, width, height);

        return {static_cast<float>(width), static_cast<float>(height)};
    }

    return {};
}

void c_render::render_verts(int count, Vertex_t* vertexes, color c)
{
    static int Texture = g_interfaces.m_surface->create_texture_id(true); // need to make a texture with procedural true
    unsigned char buffer[4] = {255, 255, 255, 255};                       // r,g,b,a

    g_interfaces.m_surface->draw_set_texture_rgba(Texture, buffer, 1, 1, false,
                                                  false); // Texture, char array of texture, width, height
    g_interfaces.m_surface->draw_set_color(c); // keep this full color and opacity use the RGBA @top to set values.
    g_interfaces.m_surface->draw_set_texture(Texture); // bind texture

    g_interfaces.m_surface->draw_textured_polygon(count, vertexes);
}

int c_render::create_font(const char* name, const int size, const int weight, const int flags)
{
    const auto font = g_interfaces.m_surface->create_font();
    g_interfaces.m_surface->set_font_glyph_set(font, name, size, weight, flags);

    return font;
}
