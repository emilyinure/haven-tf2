#include "input.h"
void c_input::on_wnd_proc(HWND hwnd, const UINT msg, WPARAM wparam, const LPARAM lparam)
{
    if (msg == WM_MOUSEMOVE)
        this->m_mouse_pos = {static_cast<float>(GET_X_LPARAM(lparam)), static_cast<float>(GET_Y_LPARAM(lparam))};
}

// todo: temp. implementation, this is the quickest way without manual delays.
void c_input::poll()
{
    for (auto i = 0; i < 256; i++)
    {
        last_key_state_[i] = prestine_key_state_[i];
        this->key_state_[i] = GetAsyncKeyState(i);
        prestine_key_state_[i] = key_state_[i];
    }
}

bool c_input::key_pressed(const int key) const
{
    return this->key_state_[key] && !this->last_key_state_[key];
}

void c_input::set_key(const int key, bool state)
{
    this->key_state_[key] = state;
}

bool c_input::key_down(const int key) const
{
    return this->key_state_[key];
}

auto c_input::key(const int index) const -> const char*
{
    return this->keys_[index];
}

bool c_input::hovering(const box_t area) const
{
    return this->m_mouse_pos.m_x > area.m_x && this->m_mouse_pos.m_y > area.m_y &&
           this->m_mouse_pos.m_x < area.m_x + area.m_w && this->m_mouse_pos.m_y < area.m_y + area.m_h;
}

auto c_input::key_pressed_prestine(const int key) const -> bool
{
    return prestine_key_state_[key] && !last_key_state_[key];
}
