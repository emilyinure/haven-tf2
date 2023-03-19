#pragma once

class c_button : public c_base_control
{
    bool held_;

public:
    typedef void (*callback_fn)(void);
    callback_fn m_callback;
    bool m_pressed;

    c_button(const char* name, callback_fn callback, const char* tooltip = "");

    void paint() override;
    void update() override;
};
