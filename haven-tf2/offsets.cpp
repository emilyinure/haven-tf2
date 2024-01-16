#include "sdk.h"

void c_offsets::init()
{
    const auto vgui_mat_surface = g_modules.get("vguimatsurface.dll");
    {
        // x-ref "-pixel_offset_x".
        this->m_sigs.start_drawing =
            vgui_mat_surface
                .get_sig("55 8B EC 64 A1 ? ? ? ? 6A ? 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC ? 80 3D ? ? ? ? ?")
                .m_ptr;

        // x-ref "Too many popups! Rendering will be bad!".
        this->m_sigs.finish_drawing =
            vgui_mat_surface.get_sig("55 8B EC 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A ?").m_ptr;
    }

    const auto client = g_modules.get("client.dll");
    {
        // x-ref "VEngineClient014".
        this->m_sigs.global_vars_base = client.get_sig("A3 ? ? ? ? 8D 45 08 6A 01").add(0x1);

        // goto CCollisionProperty count to WorldSpaceSurroundingBounds (https://i-dont.go-outsi.de/5qA27hsx2)
        // calls ComputeSurroundingBox which calls ComputeHitboxSurroundingBox.
        this->m_sigs.compute_hitbox_surrounding_box =
            client
                .get_sig("55 8B EC 81 EC ? ? ? ? 56 8B F1 57 80 BE 89 08 00 00 ? 0F 85 ? ? ? ? 83 BE 90 08 00 00 ? 75 "
                         "? E8 ? ? ? ? 8B BE ? ? ? ? 85 FF 0F 84 ? ? ? ? 8B 86 ? ? ? ? 8B 17 53")
                .m_ptr;

        // x-ref "Usage: \"load_itempreset <preset index>\"
        this->m_sigs.in_cond = client.get_sig("55 8B EC 83 EC 08 56 57 8B 7D 08 8B F1 83").m_ptr;

        this->m_sigs.invalidate_bone_cache = client.get_sig("A1 ? ? ? ? 48 C7 81 60 08 00 00 FF FF 7F FF").m_ptr;
    }
}
