#pragma once

class c_attribute_manager {
public:
  static float attribute_hook_float(float value, const char* attribute_hook, void* entity,
                                    void* item_list              = NULL,
                                    bool  is_global_const_string = false) {
    typedef float(__cdecl * oAttributeHookValue)(float, const char*, void*, void*, bool);
    static auto func = g_modules.get("client.dll")
                           .get_sig("55 8B EC 83 EC 0C 8B 0D ? ? ? ? 53 56 57 33 F6 33 FF 89 "
                                    "75 F4 89 7D F8 8B 41 08 85 C0 74 38")
                           .as<oAttributeHookValue>();
    return func(value, attribute_hook, entity, item_list, is_global_const_string);
  }
};