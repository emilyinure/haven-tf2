#pragma once

class c_groupbox : public c_base_control
{
    vector_2d control_offset_ = {0, 0}, padding_ = {0, 0};

    void paint_children();
    void update_children();

public:
    box_t m_render_area = {0, 0, 0, 0};
    std::deque<std::shared_ptr<c_base_control>> m_controls = {};

    c_groupbox(const char* name, box_t area);

    void paint() override;
    void update() override;

    std::shared_ptr<c_checkbox> add_checkbox(const char* name, const char* tooltip = "");
    std::shared_ptr<c_key_bind> add_keybind(const char* name, const char* tooltip, int default_type = 0);
    std::shared_ptr<c_button> add_button(const char* name, c_button::callback_fn callback, const char* tooltip = "");
    std::shared_ptr<c_slider> add_slider(const char* name, float min = 0.f, float max = 100.f,
                                         const char* tooltip = "");
    std::shared_ptr<c_combobox> add_combobox(const char* name, const char* tooltip = "");
};
