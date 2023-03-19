#include "sdk.h"

void c_base_control::paint_tooltip()
{
    if (!g_input.hovering(this->m_area) || this->m_tooltip == "" ||
        (g_ui.m_focused_control && g_ui.m_focused_control != this))
    {
        this->last_non_hover_time_ = g_ui.m_current_time;
        return;
    }

    // only draw if we've been hovering for .5 seconds.
    if (g_ui.m_current_time - this->last_non_hover_time_ < .5f)
        return;

    // get our cur. mouse pos.
    auto mouse_pos = g_input.m_mouse_pos;

    // get the text size.
    const auto text_size = g_ui.m_draw.get_text_size(this->m_tooltip, g_render.m_fonts.menu.tooltip);

    // calculate our tooltip area.
    box_t tooltip_area = {
        g_input.m_mouse_pos.m_x + 3,
        g_input.m_mouse_pos.m_y - 20,
        text_size.m_x + 3,
        text_size.m_y + 1,
    };

    // background.
    g_ui.m_draw.filled_rect(tooltip_area, {30, 30, 30});
    g_ui.m_draw.outlined_rect(tooltip_area, {65, 65, 65});

    // text.
    g_ui.m_draw.text(g_render.m_fonts.menu.tooltip, {tooltip_area.m_x + 2, tooltip_area.m_y}, this->m_tooltip,
                     {255, 255, 255}, e_text_alignment::text_align_left);
}
