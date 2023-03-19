#pragma once

#include <iostream>
#include <fstream>

class key_bind_t;

class c_key_bind : public c_base_control
{
    const char* key_bind_types_[4] = {"Always", "Hold", "Toggle", "Off Key"};

public:
    std::shared_ptr<key_bind_t> value_;
    box_t dropdown_area_ = {0, 0, 0, 0}, item_area_ = {0, 0, 0, 0};
    bool open_ = false, hovering_dropdown_ = false, hovering_items_ = false, capturing_{false};

    c_key_bind(const char* name, const char* tooltip, int default_type);

    auto paint() -> void override;

    auto update() -> void override;
};
