#pragma once

#define __VFUNC(function_name, index, type, ...)                                                                       \
    auto function_name                                                                                                 \
    {                                                                                                                  \
        return c_utils::get_virtual_function<type>(this, index)(this, __VA_ARGS__);                                    \
    };

enum e_console_colors
{
    console_color_white = 15,
    console_color_red = 12,
    console_color_green = 10,
    console_color_cyan = 11,
    console_color_beige = 14
};

class c_utils
{
public:
    template <typename T> static T get_virtual_function(void* base, const std::uint16_t index)
    {
        return (*static_cast<T**>(base))[index];
    }

    struct debug_t
    {
        void open_console();
        static void close_console();
        static void set_console_color(e_console_colors color);
        static void print(const char* text, e_console_colors color);
    } m_debug;

    static unsigned fnv_hash(const std::string& str);
};
inline c_utils g_utils;
