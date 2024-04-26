#pragma once

enum key_bind_type_t
{
    key_bind_type_always = 0,
    key_bind_type_hold,
    key_bind_type_toggle,
    key_bind_type_off_key
};

struct key_bind_t
{
    key_bind_t()
    {
    }
    key_bind_t(const char* name, int default_type)
    {
        this->name = name;
        type = default_type;
    }
    std::vector<const char*> key_bind_types_{"Always", "Hold", "Toggle", "Off Key"};
    void update();
    const char* name = "";
    int key = 0;
    int type = key_bind_type_always;
    bool enabled = false;
};

class c_ui
{
    std::shared_ptr<c_window> add_window(const char* name, box_t area);
    static void draw_cursor();

public:
    float m_current_time;
    std::deque<std::shared_ptr<c_window>> m_windows;
    std::deque<std::shared_ptr<key_bind_t>> m_binds;
    bool m_open = true;
    void append_bind(std::shared_ptr<key_bind_t> value)
    {
        m_binds.push_back(value);
    }
    float m_fade = 1.f;
    color m_theme = {0x61, 0xBA, 0x3B};
    
    c_base_control* m_focused_control;

    struct draw_t
    {
        void (*filled_rect)(box_t area, color col);
        void (*outlined_rect)(box_t area, color col);
        void (*text)(unsigned long font, vector_2d pos, const char* text, color col, e_text_alignment alignment);
        vector_2d (*get_text_size)(const char* text, unsigned long font);

        struct
        {
            unsigned long main;
            unsigned long title;
            unsigned long tabs;
            unsigned long tooltip;
        } fonts;

        void init();
    } m_draw;

    struct
    {
        struct
        {
            std::shared_ptr<c_combobox> config_slot;

            std::shared_ptr<c_button> save;
            std::shared_ptr<c_button> load;
        } config;

        struct
        {
            struct
            {
                std::shared_ptr<c_checkbox> enabled;
                std::shared_ptr<c_checkbox> name;
                std::shared_ptr<c_checkbox> box;
                std::shared_ptr<c_combobox> health;
                std::shared_ptr<c_checkbox> flags;
                std::shared_ptr<c_checkbox> weapon;
                std::shared_ptr<c_checkbox> offscreen;
            } players;
            struct
            {
                std::shared_ptr<c_slider> x_offset;
                std::shared_ptr<c_slider> y_offset;
                std::shared_ptr<c_slider> z_offset;
                std::shared_ptr<c_slider> roll;
            } view_model;
        } visuals;
        struct
        {
            struct
            {
                std::shared_ptr<c_checkbox> enabled;
                std::shared_ptr<c_key_bind> key;
                std::shared_ptr<c_slider> interp;
                std::shared_ptr<c_combobox> fire_mode;
                std::shared_ptr<c_slider> hitchance;
            } players;
            struct
            {
                std::shared_ptr<c_checkbox> enabled;
                std::shared_ptr<c_key_bind> key;
                std::shared_ptr<c_checkbox> auto_fire;
            } other;
        } aim;
        struct
        {
            std::shared_ptr<c_checkbox> auto_strafer;
            std::shared_ptr<c_key_bind> third_person;
            std::shared_ptr<c_slider> third_person_horizontal_offset;
            std::shared_ptr<c_slider> third_person_vertical_offset;
            std::shared_ptr<c_slider> third_person_distance_offset;
            std::shared_ptr<c_checkbox> fake_latency_toggle;
            std::shared_ptr<c_slider> fake_latency_amount;
        } misc;
    } m_controls;

    void init();
    void on_paint();
};
inline c_ui g_ui;
