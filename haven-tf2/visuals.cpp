#include "sdk.h"
#include "movement_simulate.h"

void c_visuals::on_paint() {
  this->watermark();

  if (!g_cl.m_local)
    return;

  if (!g_interfaces.m_engine->is_in_game())
    return;

  this->player_esp();
  g_movement.draw();
  // g_movement.setup_mv( local->m_velocity( ), local, local->entindex() );
  // for( auto i = 0; i <300; i++ )
  //	g_movement.run( );
  // g_movement.draw( );
}

void c_visuals::watermark() {
  const auto text_size = g_render.get_text_size("haven", g_render.m_fonts.main);

  g_render.filled_rect({3, 3}, {text_size.m_x + 4, text_size.m_y + 4}, {0, 0, 0, 150});
  g_render.text(g_render.m_fonts.main, {5, 5}, "haven", {255, 255, 255});
}
