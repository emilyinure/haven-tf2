#pragma once

class c_tab : public c_base_control
{

    void paint_children();
    void update_children();

public:
    std::deque<std::shared_ptr<c_groupbox>> m_groupboxes = {};
    box_t m_item_area = {0, 0, 0, 0};
    bool m_enabled = false;

    c_tab(const char* name);

    void paint() override;
    void update() override;
    std::shared_ptr<c_groupbox> add_groupbox(const char* name, box_t area);
};
