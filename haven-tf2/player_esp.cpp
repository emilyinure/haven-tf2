#include "sdk.h"
#include "movement_simulate.h"
#include <string.h>
void c_visuals::player_esp()
{

    for (auto i = 1; i < g_interfaces.m_engine->get_max_clients(); i++)
    {
        auto& player_esp = g_ui.m_controls.visuals.players;

        if (!player_esp.enabled->m_value)
        {
            continue;
        }

        const auto player = g_interfaces.m_entity_list->get_entity<c_base_player>(i);

        if (!player->is_valid(g_cl.m_local))
            continue;

        auto shared = player->m_shared();
        if (!shared)
            continue;

        auto bounding_box = player->get_bounding_box();

        if (bounding_box.is_zero())
            continue;

        const auto health = player->m_i_health();
        const auto max_health = player->get_max_health();

        player_info_s player_info;

        if (player_esp.name->m_value)
            if (g_interfaces.m_engine->get_player_info(player->entindex(), &player_info))
                g_render.text(g_render.m_fonts.menu.main,
                              {bounding_box.m_x + (bounding_box.m_w * 0.5f), bounding_box.m_y - 15}, player_info.m_name,
                              g_cl.m_local->m_i_team_num() == player->m_i_team_num() ? g_ui.m_theme
                                                                                     : color(255, 255, 255),
                              text_align_center);

        if (player_esp.box->m_value)
        {
            g_render.outlined_rect(
                {bounding_box.m_x - 1, bounding_box.m_y - 1, bounding_box.m_w + 2, bounding_box.m_h + 2}, {0, 0, 0});
            g_render.outlined_rect(
                {bounding_box.m_x + 1, bounding_box.m_y + 1, bounding_box.m_w - 2, bounding_box.m_h - 2}, {0, 0, 0});
            g_render.outlined_rect(bounding_box, {255, 255, 255});
        }

        if (player_esp.health->m_selected_index == 1)
        {
            const box_t health_bar_area = {bounding_box.m_x - 6, bounding_box.m_y, 4, bounding_box.m_h};

            // get our healthbar height.
            const auto healthbar_height = health_bar_area.m_h * c_math::get_fraction(health, max_health, 0);

            g_render.filled_rect(health_bar_area, {35, 35, 35});
            g_render.filled_rect({health_bar_area.m_x, health_bar_area.m_y + (health_bar_area.m_h - healthbar_height),
                                  health_bar_area.m_w, healthbar_height},
                                 {0, 255, 0});
            g_render.outlined_rect(health_bar_area, {0, 0, 0});
        }
        else if (player_esp.health->m_selected_index == 2)
        {
            char buffer[128];
            _itoa(health, buffer, 10);

            g_render.text(g_render.m_fonts.menu.main, {bounding_box.m_x - 2, bounding_box.m_y}, buffer,
                          color(255, 255, 255), text_align_right);
        }

        // eventually do multi combo box.
        if (player_esp.flags->m_value)
        {
            float flag_offset = 0.f;
            auto draw_flag =
                [&flag_offset, &bounding_box, player](const char* flag_text, const color& flag_color) mutable
            {
                const color col = {255, 255, 255};

                const vector_2d flag_text_size = g_render.get_text_size(flag_text, g_render.m_fonts.menu.main);
                g_render.text(g_render.m_fonts.menu.main,
                              {(bounding_box.m_x + bounding_box.m_w) + 4, (bounding_box.m_y + flag_offset) - 2},
                              flag_text, col);

                flag_offset += flag_text_size.m_y + 1;
            };

            if (health > max_health)
                draw_flag("Overheal", color(255, 255, 255));

            if (shared->in_cond(e_tf_cond::TF_COND_DISGUISED))
                draw_flag("Disguised", color(255, 255, 255));

            if (shared->in_cond(e_tf_cond::TF_COND_STEALTHED))
                draw_flag("Cloaked", color(255, 255, 255));
        }

        if (player_esp.weapon->m_value)
        {
            const auto weapon = reinterpret_cast<c_base_weapon*>(player->get_active_weapon());
            if (!weapon)
            {
                return;
            }

            std::string weapon_text = weapon->get_localized_name();
            g_render.text(g_render.m_fonts.menu.main,
                          {bounding_box.m_x + (bounding_box.m_w * 0.5f), bounding_box.m_y - 30}, weapon_text.c_str(),
                          {255, 255, 255, 255}, text_align_center);
        }
    }

    // predicted path
    if (g_cl.m_local->is_valid(g_cl.m_local, false))
    {
        vector last_pos = g_cl.m_local->m_vec_origin();
        vector screen_1;
        vector screen_2;

        g_movement.setup_mv(g_cl.m_local->m_velocity(), g_cl.m_local, g_cl.m_local->entindex() - 1);

        for (auto i = 0; i < 100; i++)
        {
            auto pos = g_movement.run();
        }
    }
}
