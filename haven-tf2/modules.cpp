#include "sdk.h"

// this is probably overengineered to shit, but it works and i like it.
void c_modules::init()
{
    c_utils::debug_t::set_console_color(console_color_cyan);
    printf_s("[>] waiting for serverbrowser.dll\n");

    // wait for serverbrowser.dll.
    while (!GetModuleHandleA("serverbrowser.dll"))
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    this->add("client.dll");
    this->add("server.dll");
    this->add("engine.dll");
    this->add("tier0.dll", false);
    this->add("vguimatsurface.dll");
    this->add("vstdlib.dll");
    this->add("vgui2.dll");
}

module_t c_modules::get(const char* name)
{
    const auto hashed_name = c_utils::fnv_hash(name);

    for (auto& module : this->modules_)
    {
        // todo: add wildcard searching which would return an array of modules.
        // couldn't see using it, just would be nice to have.
        if (module.m_hash != hashed_name)
            continue;

        // found.
        return module;
    }

    c_utils::debug_t::set_console_color(console_color_red);
    printf_s("[!] failed to get module %s\n", name);

    return {};
}

void c_modules::add(const char* name, bool populate_ifaces)
{
    auto module_addr = GetModuleHandleA(name);
    while (!module_addr)
    {
        module_addr = GetModuleHandleA(name);
    }

    if (!module_addr)
    {
        c_utils::debug_t::set_console_color(console_color_red);
        printf_s("[!] failed to add module %s\n", name);
        return;
    }

#ifdef _DEBUG
    // c_utils::debug_t::set_console_color(console_color_beige);
    // printf_s( "[>] %s added\n", name );
#endif

    module_t mod = {name, module_addr, c_utils::fnv_hash(name)};

    if (populate_ifaces)
    {
        // export CreateInterface calls CreateInterfaceInternal https://i-dont.go-outsi.de/5ptWtWhKH
        // this sig *should* be global.
        mod.populate_interfaces(mod.get_sig("8B 35 ?? ?? ?? ?? 57 85 F6 74 38").add(0x2));
    }

    this->modules_.emplace_back(mod);
}

void module_t::populate_interfaces(uint8_t* reg)
{
    if (!reg)
    {
        c_utils::debug_t::set_console_color(console_color_red);
        printf_s("[!] invalid interface reg (%s)", this->m_name);
        return;
    }

    const auto interface_list = **reinterpret_cast<interface_reg***>(reg);

    if (!interface_list)
    {
        c_utils::debug_t::set_console_color(console_color_red);
        printf_s("[!] failed to populate interfaces for (%s)", this->m_name);
        return;
    }

    for (interface_reg* cur_iface = interface_list; cur_iface; cur_iface = cur_iface->m_next)
    {
        if (!cur_iface->m_create_fn)
            continue;

        this->m_interfaces.emplace_back(cur_iface->m_name, c_utils::fnv_hash(cur_iface->m_name),
                                        static_cast<uintptr_t*>(cur_iface->m_create_fn()));
    }

#ifdef _DEBUG
    c_utils::debug_t::set_console_color(console_color_beige);
    printf_s("[>] populated %i interfaces (%s)\n", this->m_interfaces.size(), this->m_name);
#endif
}

interface_t module_t::get_interface(const char* name, const bool exact) const
{
    if (exact)
        return this->get_interface_exact(name);

    for (const auto& iface : this->m_interfaces)
    {
        // todo: find faster alternative.
        if (!strstr(iface.m_name, name))
            continue;

#ifdef _DEBUG
        c_utils::debug_t::set_console_color(console_color_beige);
        printf_s("[>] found %s (0x%p)\n", iface.m_name, iface.m_ptr);
#endif

        // found.
        return iface;
    }

    c_utils::debug_t::set_console_color(console_color_red);
    printf_s("[!] failed to get iface %s\n", name);
    return {};
}

interface_t module_t::get_interface_exact(const char* name) const
{
    const auto hashed_name = c_utils::fnv_hash(name);

    for (const auto& iface : this->m_interfaces)
    {
        if (iface.m_hash != hashed_name)
            continue;

#ifdef _DEBUG
        c_utils::debug_t::set_console_color(console_color_beige);
        printf_s("[>] found %s (0x%p)\n", iface.m_name, iface.m_ptr);
#endif

        // found.
        return iface;
    }

    c_utils::debug_t::set_console_color(console_color_red);
    printf_s("[!] failed to get iface %s\n", name);
    return {};
}

signature_t module_t::get_sig(const char* sig) const
{
    if (!this->m_module)
        return {};

    static auto pattern_to_byte = [](const char* pattern)
    {
        auto bytes = std::vector<int>{};
        const auto start = const_cast<char*>(pattern);
        const auto end = const_cast<char*>(pattern) + std::strlen(pattern);

        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;

                if (*current == '?')
                    ++current;

                bytes.push_back(-1);
            }
            else
            {
                bytes.push_back(std::strtoul(current, &current, 16));
            }
        }
        return bytes;
    };

    const auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(this->m_module);
    const auto nt_headers =
        reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(this->m_module) + dos_header->e_lfanew);

    const auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
    const auto pattern_bytes = pattern_to_byte(sig);
    const auto scan_bytes = reinterpret_cast<std::uint8_t*>(this->m_module);

    const auto s = pattern_bytes.size();
    const auto d = pattern_bytes.data();

    for (auto i = 0ul; i < size_of_image - s; ++i)
    {
        auto found = true;

        for (auto j = 0ul; j < s; ++j)
        {
            if (scan_bytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }

        if (found)
            return {&scan_bytes[i]};
    }

    return {};
}
