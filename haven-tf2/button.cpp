#include "sdk.h"

c_button::c_button(const char* name, callback_fn callback, const char* tooltip)
{
    this->m_type = e_control_type::CONTROL_TYPE_BUTTON;
    this->m_name = name;
    this->m_callback = callback;
    this->m_tooltip = tooltip;
}

void c_button::paint()
{
    // background.
    g_ui.m_draw.filled_rect(this->m_area, this->m_pressed ? g_ui.m_theme : color(35, 35, 35, 40 * g_ui.m_fade));
    g_ui.m_draw.outlined_rect(this->m_area, color(35, 35, 35, 40 * g_ui.m_fade));

    // title.
    g_ui.m_draw.text(g_render.m_fonts.menu.main,
                     {this->m_area.m_x + 2 + ((this->m_area.m_w - 4) * 0.5f), this->m_area.m_y + 3}, this->m_name,
                     color(255, 255, 255, 100 * g_ui.m_fade), e_text_alignment::text_align_center);
}

void c_button::update()
{
    if (g_input.hovering(this->m_area) && g_input.key_pressed(VK_LBUTTON))
    {
        this->m_pressed = true;

        if (this->m_callback)
            this->m_callback();
    }

    if (this->m_pressed && !g_input.key_down(VK_LBUTTON))
        this->m_pressed = false;
}
