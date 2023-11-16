#include "sdk.h"

c_tab::c_tab(const char* name)
{
    this->m_type = e_control_type::CONTROL_TYPE_TAB;
    this->m_name = name;
}

void c_tab::paint()
{
    // background.
    g_ui.m_draw.filled_rect(this->m_area, color(35, 35, 35, 40));

    // title.
    g_ui.m_draw.text(g_render.m_fonts.menu.tabs, {this->m_area.m_x + (this->m_area.m_w * 0.5f), this->m_area.m_y + 2},
                     this->m_name, color(255, 255, 255, 100 * g_ui.m_fade), e_text_alignment::text_align_center);

    // bars.
    // g_ui.m_draw.outlined_rect( { this->m_area.m_x, this->m_area.m_y + this->m_area.m_h - 3, this->m_area.m_w, 1 },
    // this->m_enabled ? g_ui.m_theme : color( 35, 35, 35, 30 ) );
    g_ui.m_theme.m_a = 100 * g_ui.m_fade;
    if (this->m_enabled)
        g_ui.m_draw.filled_rect({this->m_area.m_x, this->m_area.m_y + this->m_area.m_h - 2, this->m_area.m_w, 1},
                                g_ui.m_theme - 20);
    g_ui.m_theme.m_a = 255;

    // outline.
    // g_ui.m_draw.outlined_rect( this->m_area, { 65, 65, 65 } );

    if (!this->m_enabled)
        return;

    this->paint_children();
}

void c_tab::update()
{
    if (!this->m_enabled)
        return;

    this->update_children();
}

void c_tab::paint_children()
{
    if (this->m_groupboxes.empty())
        return;

    for (const auto& child : this->m_groupboxes)
        child->paint();
}

void c_tab::update_children()
{
    if (this->m_groupboxes.empty())
        return;

    for (auto i = 0; i < this->m_groupboxes.size(); i++)
    {
        const auto& child = this->m_groupboxes[i];
        // handle position.
        child->m_area = {child->m_render_area.m_x + this->m_item_area.m_x,
                         child->m_render_area.m_y + this->m_item_area.m_y, child->m_render_area.m_w,
                         child->m_render_area.m_h};

        child->update();
    }
}

std::shared_ptr<c_groupbox> c_tab::add_groupbox(const char* name, box_t area)
{
    auto groupboxes = std::make_shared<c_groupbox>(name, area);

    this->m_groupboxes.emplace_back(groupboxes);

    return groupboxes;
}


