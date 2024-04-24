#pragma once

#include <windows.h>
#include <windowsx.h>
#include <thread>
#include <chrono>
#include <deque>
#include <fstream>

#include "json.h"
#include "minhook/minhook.h"
#include "json.h"

#include "modules.h"

#include "matrix_3x4.h"
#include "vector.h"
#include "vector_2d.h"
#include "view_matrix.h"
#include "math.h"
#include "box.h"
#include "color.h"

#include "utils.h"
#include "input.h"

#include "tier0.h"

#include "c_utl_memory.h"
#include "c_utl_vector.h"
#include "c_utl_string.h"
#include "interface_reg.h"
#include "dt_recv.h"
#include "client_class.h"
#include "button_code.h"
#include "bit_buf.h"
#include "base_types.h"
#include "view_setup.h"
#include "data_map.h"
#include "i_file_list.h"
#include "renamed_recv_table_info.h"
#include "mouth_info.h"
#include "client_text_message.h"
#include "i_spatial_query.h"
#include "i_net_channel_info.h"
#include "modes.h"
#include "i_achievement_mgr.h"
#include "key_values.h"
#include "i_recipient_filter.h"
#include "snd_info.h"
#include "sound_info.h"
#include "c_base_handle.h"
#include "i_handle_entity.h"
#include "i_collideable.h"
#include "i_client_unknown.h"
#include "i_client_renderable.h"
#include "i_client_networkable.h"
#include "i_client_thinkable.h"
#include "i_client_entity.h"
#include "overlay_text_t.h"
#include "i_app_system.h"
#include "convar.h"
#include "usercmd.h"
#include "c_attribute_manager.h"

#include "c_global_vars_base.h"
#include "i_base_client_dll.h"
#include "iv_engine_client.h"
#include "i_prediction.h"
#include "i_engine_sound.h"
#include "i_client_entity_list.h"
#include "i_engine_vgui.h"
#include "i_surface.h"
#include "iv_debug_overlay.h"
#include "i_cvar.h"
#include "i_model_info.h"
#include "mem_alloc.h"
#include "isteamclient.h"
#include "isteamfriends.h"
#include "isteamutils.h"
#include "isteamapps.h"
#include "isteamuserstats.h"
#include "isteamuser.h"

#include "game_rules.h"
#include "i_engine_trace.h"
#include "i_localize.h"
#include "render_view.h"
#include "c_client_state.h"

#include "render.h"

#include "entity.h"
