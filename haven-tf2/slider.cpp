#include "sdk.h"

c_slider::c_slider(const char* name, float min, float max, const char* tooltip)
{
    this->m_type = e_control_type::CONTROL_TYPE_SLIDER;
    this->m_name = name;
    this->m_min = min;
    this->m_max = max;
    this->m_tooltip = tooltip;
    this->m_value = false;
}

void c_slider::paint()
{
    // set our bar area.
    this->bar_area_ = {this->m_area.m_x + (this->m_area.m_w - (this->m_area.m_w * 0.5f)), this->m_area.m_y + 2,
                       this->m_area.m_w * 0.5f, 10};

    // update our filling.
    this->filling_ = this->bar_area_.m_w * c_math::get_fraction(this->m_value, this->m_max, this->m_min);

    // background.
    g_ui.m_draw.filled_rect(this->bar_area_, color(35, 35, 35, 40 * g_ui.m_fade));
    g_ui.m_theme.m_a = 100 * g_ui.m_fade;
    // fill.
    g_ui.m_draw.filled_rect({this->bar_area_.m_x, this->bar_area_.m_y, this->filling_, this->bar_area_.m_h},
                            g_ui.m_theme);

    // outline.
    g_ui.m_draw.outlined_rect(this->bar_area_, color(35, 35, 35, 40 * g_ui.m_fade));

    // title.
    g_ui.m_theme.m_a = 100 * g_ui.m_fade;
    g_ui.m_draw.text(g_render.m_fonts.menu.main, {this->m_area.m_x, this->m_area.m_y}, this->m_name, g_ui.m_theme,
                     e_text_alignment::text_align_left);
    g_ui.m_theme.m_a = 255.f;
    // value text.
    char buffer[16] = {};
    sprintf_s(buffer, "%.0f", this->m_value);

    // get our text size.
    const auto text_size = g_ui.m_draw.get_text_size(buffer, g_render.m_fonts.menu.main);

    g_ui.m_draw.text(g_render.m_fonts.menu.main,
                     {std::clamp(this->bar_area_.m_x + this->filling_, this->bar_area_.m_x,
                                 this->bar_area_.m_x + this->bar_area_.m_w - text_size.m_x),
                      this->bar_area_.m_y + 1},
                     buffer, color(255, 255, 255, 100 * g_ui.m_fade), e_text_alignment::text_align_left);
}

void c_slider::update()
{
    if (g_input.hovering(this->bar_area_) && g_input.key_pressed(VK_LBUTTON))
    {
        this->m_dragging = true;
        g_ui.m_focused_control = this;
    }

    // we were dragging, but we let go.
    if (this->m_dragging && !g_input.key_down(VK_LBUTTON))
    {
        g_ui.m_focused_control = nullptr;
        this->m_dragging = false;
    }

    if (!this->m_dragging)
        return;

    // min + ( max - min ) * ( x_drag - x_pos ) / total_width
    // for now we'll cast this to an int.
    this->m_value = static_cast<int>(std::clamp(
        this->m_min +
            ((this->m_max - this->m_min) * ((g_input.m_mouse_pos.m_x - this->bar_area_.m_x) / this->bar_area_.m_w)),
        this->m_min, this->m_max));
}
