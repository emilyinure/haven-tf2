#include "sdk.h"

void save_callback()
{
    g_config.save(std::format("config_{}.intern", g_ui.m_controls.config.config_slot->m_selected_index + 1).c_str());
}

void load_callback()
{
    g_config.load(std::format("config_{}.intern", g_ui.m_controls.config.config_slot->m_selected_index + 1).c_str());
}

void key_bind_t::update()
{
    type = std::clamp(type, 0, static_cast<int>(this->key_bind_types_.size()));

    switch (type)
    {
        case key_bind_type_always:
            enabled = true;
            break;
        case key_bind_type_hold:
            enabled = g_input.key_down(key);
            break;
        case key_bind_type_toggle:
        {
            if (g_input.key_pressed_prestine(key))
                enabled = !enabled;
        }
        break;
        case key_bind_type_off_key:
            enabled = !GetAsyncKeyState(key);
            break;
        default:
            enabled = true;
            break;
    }
}

void c_ui::init()
{
    // initialize our draw functions.
    this->m_draw.init();

    const auto main_window = this->add_window("haven", {290, 50, 300, 350});
    {
        const auto aim = main_window->add_tab("Aim");
        {
            const auto players = aim->add_groupbox("Players", {0, 5, 215, 250});
            {
                this->m_controls.aim.players.enabled = players->add_checkbox("Enabled");
                this->m_controls.aim.players.key = players->add_keybind("Key", "Aimbot key", 0);
                this->m_controls.aim.players.interp =
                    players->add_slider("Interp", 1, 10, "Interpolation On Player Prediction");
                this->m_controls.aim.players.hitchance =
                    players->add_slider("Hitchance", 0, 100, "Hitchance For Player Prediction");
                this->m_controls.aim.players.fire_mode = players->add_combobox("Fire Mode");
                {
                    this->m_controls.aim.players.fire_mode->add_item("Default");
                    this->m_controls.aim.players.fire_mode->add_item("Auto Release");
                    this->m_controls.aim.players.fire_mode->add_item("Automatic");
                }
            }
        }
        const auto visuals = main_window->add_tab("Visuals");
        {
            const auto players = visuals->add_groupbox("Players", {0, 5, 210, 250});
            {
                this->m_controls.visuals.players.enabled = players->add_checkbox("Enabled");
                this->m_controls.visuals.players.name = players->add_checkbox("Name");
                this->m_controls.visuals.players.box = players->add_checkbox("Box");
                this->m_controls.visuals.players.health = players->add_combobox("Health");
                this->m_controls.visuals.players.health->add_item("Off");
                this->m_controls.visuals.players.health->add_item("Bar");
                this->m_controls.visuals.players.health->add_item("Text");
                this->m_controls.visuals.players.flags = players->add_checkbox("Flags");
                this->m_controls.visuals.players.weapon = players->add_checkbox("Weapon");
            }
        }
    }

    const auto config_window = this->add_window("Config", {15, 50, 270, 128});
    {
        const auto tab = config_window->add_tab("Main");
        {
            const auto groupbox = tab->add_groupbox("Config", {0, 5, 191, 91});
            {
                this->m_controls.config.config_slot = groupbox->add_combobox("Slot");
                {
                    this->m_controls.config.config_slot->add_item("Default");
                    this->m_controls.config.config_slot->add_item("Slot 2");
                    this->m_controls.config.config_slot->add_item("Slot 3");
                    this->m_controls.config.config_slot->add_item("Slot 4");
                    this->m_controls.config.config_slot->add_item("Slot 5");
                }

                this->m_controls.config.save =
                    groupbox->add_button("Save", save_callback, "Save your config (located in base tf2 folder)");
                this->m_controls.config.load =
                    groupbox->add_button("Load", load_callback, "Load your config (located in base tf2 folder)");
            }
        }
    }
    g_config.init();
}

void c_ui::on_paint()
{
    this->m_current_time = g_interfaces.m_global_vars->m_real_time;

    // no windows.
    if (this->m_windows.empty())
        return;

    if (g_input.key_pressed(VK_INSERT))
        this->m_open = !this->m_open;

    for (auto& it : m_binds)
        it->update();

    if (this->m_open)
    {
        this->m_fade = fminf(this->m_fade + 0.01f, 1.f);
    }
    else
    {
        this->m_fade = fmaxf(this->m_fade - 0.01f, 0.f);
    }

    if (this->m_open && g_ui.m_fade == 1.f)
        for (const auto& window : this->m_windows)
            window->update();

    if (g_ui.m_fade != 0.f)
        for (const auto& window : this->m_windows)
            window->paint();

    if (!this->m_open)
        return;

    // sort based on the last time we clicked the window.
    std::ranges::sort(this->m_windows,
                      [&](const std::shared_ptr<c_window>& first, const std::shared_ptr<c_window>& second)
                      { return first->m_last_click_time < second->m_last_click_time; });

    draw_cursor();
}

std::shared_ptr<c_window> c_ui::add_window(const char* name, box_t area)
{
    auto window = std::make_shared<c_window>(name, area);

    // place our window into the list.
    this->m_windows.emplace_back(window);

    return window;
}

void c_ui::draw_cursor()
{
    g_ui.m_draw.filled_rect({g_input.m_mouse_pos.m_x - 4, g_input.m_mouse_pos.m_y - 4, 8, 8}, {70, 70, 70});
    g_ui.m_draw.outlined_rect({g_input.m_mouse_pos.m_x - 4, g_input.m_mouse_pos.m_y - 4, 8, 8}, {85, 85, 85});
}

void c_ui::draw_t::init()
{
    // init draw functions.
    this->filled_rect = c_render::filled_rect;
    this->outlined_rect = c_render::outlined_rect;
    this->text = c_render::text;
    this->get_text_size = c_render::get_text_size;

    // init fonts.
    this->fonts.main = g_render.m_fonts.menu.main;
    this->fonts.title = g_render.m_fonts.menu.title;
    this->fonts.tabs = g_render.m_fonts.menu.tabs;
    this->fonts.tooltip = g_render.m_fonts.menu.tooltip;
}
