#include "sdk.h"

void c_offsets::init() {
  const auto vgui_mat_surface = g_modules.get("vguimatsurface.dll");
  {
    // x-ref "-pixel_offset_x".
    this->m_sigs.start_drawing = vgui_mat_surface.get_sig("40 53 56 57 48 83 EC 50").m_ptr;

    // x-ref "Too many popups! Rendering will be bad!".
    this->m_sigs.finish_drawing = vgui_mat_surface.get_sig("40 53 48 83 EC 20 33 C9").m_ptr;
  }

  const auto client = g_modules.get("client.dll");
  {
    // x-ref "VEngineClient014".
    this->m_sigs.global_vars_base = client.get_sig("A3 ? ? ? ? 8D 45 08 6A 01").add(0x1);

    // goto CCollisionProperty count to WorldSpaceSurroundingBounds
    // (https://i-dont.go-outsi.de/5qA27hsx2) calls ComputeSurroundingBox which calls
    // ComputeHitboxSurroundingBox.
    this->m_sigs.compute_hitbox_surrounding_box =
        client
            .get_sig("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 48 89 7C 24 20 41 56 48 81 "
                     "EC 50 04")
            .m_ptr;

    // x-ref "Usage: \"load_itempreset <preset index>\"
    this->m_sigs.in_cond =
        client.get_sig("48 89 5C 24 08 57 48 83 EC 20 8B DA 48 8B F9 83 FA 20").m_ptr;

    this->m_sigs.invalidate_bone_cache = client.get_sig("8B 05 ?? ?? ?? ?? FF C8 C7 81").m_ptr;
  }
}
