#include "sdk.h"

c_groupbox::c_groupbox(const char* name, box_t area)
{
    this->m_type = e_control_type::CONTROL_TYPE_GROUPBOX;
    this->m_name = name;
    this->m_render_area = area;
    this->padding_ = {5, 5};
}

void c_groupbox::paint()
{
    // background.
    g_ui.m_draw.filled_rect(this->m_area, color(35, 35, 35, 40 * g_ui.m_fade));

    // title background.
    g_ui.m_draw.filled_rect({this->m_area.m_x, this->m_area.m_y, this->m_area.m_w, 15},
                            color(35, 35, 35, 40 * g_ui.m_fade));

    // outline.
    // g_ui.m_draw.outlined_rect( this->m_area, { 65, 65, 65, 40 } );

    // title outline.
    // g_ui.m_draw.outlined_rect( { this->m_area.m_x, this->m_area.m_y, this->m_area.m_w, 15 }, { 65, 65, 65 } );

    // title text.
    g_render.text(g_render.m_fonts.menu.main, {this->m_area.m_x + (this->m_area.m_w * 0.5f), this->m_area.m_y},
                  this->m_name, color(255, 255, 255, 100 * g_ui.m_fade), e_text_alignment::text_align_center);

    this->paint_children();
}

void c_groupbox::update()
{
    this->update_children();
}

void c_groupbox::paint_children()
{
    if (this->m_controls.empty())
        return;

    for (auto& child : this->m_controls)
    {
        // if we have a focused control, skip it.
        if (g_ui.m_focused_control && g_ui.m_focused_control == child.get())
            continue;

        child->paint();
        child->paint_tooltip();
    }

    // we want to draw it after all other controls.
    if (g_ui.m_focused_control)
    {
        g_ui.m_focused_control->paint_tooltip();
        g_ui.m_focused_control->paint();
    }
}

void c_groupbox::update_children()
{
    if (this->m_controls.empty())
        return;

    // reset our control offset.
    this->control_offset_ = {6, 20};

    for (auto i = 0; i < this->m_controls.size(); i++)
    {
        auto& child = this->m_controls[i];

        // set our position.
        child->m_area.m_x = this->m_area.m_x + this->control_offset_.m_x;
        child->m_area.m_y = this->m_area.m_y + this->control_offset_.m_y;

        // deal with our size on a control by control basis.
        switch (child->m_type)
        {
            case e_control_type::CONTROL_TYPE_CHECKBOX:
            {
                child->m_area.m_w = child->m_area.m_h = 12;
            }
            break;
            case e_control_type::CONTROL_TYPE_BUTTON:
            {
                child->m_area.m_w = this->m_area.m_w - (this->control_offset_.m_x * 2);
                child->m_area.m_h = 20;
            }
            break;
            case e_control_type::CONTROL_TYPE_SLIDER:
            {
                child->m_area.m_w = this->m_area.m_w - (this->control_offset_.m_x * 2);
                child->m_area.m_h = 12;
            }
            break;
            case e_control_type::CONTROL_TYPE_KEYBIND:
            {
                child->m_area.m_w = this->m_area.m_w - (this->control_offset_.m_x * 2);
                child->m_area.m_h = 16;
                if (((c_key_bind*)child.get())->open_)
                    child->m_area.m_h = ((c_key_bind*)child.get())->item_area_.m_h;
            }
            break;
            case e_control_type::CONTROL_TYPE_COMBOBOX:
            {
                child->m_area.m_w = this->m_area.m_w - (this->control_offset_.m_x * 2);
                child->m_area.m_h = 16;
                if (((c_combobox*)child.get())->open_)
                    child->m_area.m_h = ((c_combobox*)child.get())->item_area_.m_h;
            }
            break;
            default:
                break;
        }

        // add to the control offset.
        this->control_offset_.m_y += child->m_area.m_h + this->padding_.m_y;

        // if we have a focused control, only update it.
        if (g_ui.m_focused_control && g_ui.m_focused_control != child.get())
            continue;

        child->update();
    }
}

std::shared_ptr<c_checkbox> c_groupbox::add_checkbox(const char* name, const char* tooltip)
{
    auto checkbox = std::make_shared<c_checkbox>(name, tooltip);

    this->m_controls.emplace_back(checkbox);

    return checkbox;
}

std::shared_ptr<c_key_bind> c_groupbox::add_keybind(const char* name, const char* tooltip, int default_type)
{
    auto checkbox = std::make_shared<c_key_bind>(name, tooltip, default_type);

    this->m_controls.emplace_back(checkbox);

    return checkbox;
}

std::shared_ptr<c_button> c_groupbox::add_button(const char* name, c_button::callback_fn callback, const char* tooltip)
{
    auto button = std::make_shared<c_button>(name, callback, tooltip);

    this->m_controls.emplace_back(button);

    return button;
}

std::shared_ptr<c_slider> c_groupbox::add_slider(const char* name, float min, float max, const char* tooltip)
{
    auto slider = std::make_shared<c_slider>(name, min, max, tooltip);

    this->m_controls.emplace_back(slider);

    return slider;
}

std::shared_ptr<c_combobox> c_groupbox::add_combobox(const char* name, const char* tooltip)
{
    auto combobox = std::make_shared<c_combobox>(name, tooltip);

    this->m_controls.emplace_back(combobox);

    return combobox;
}
