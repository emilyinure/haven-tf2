#pragma once

class mem_alloc_t
{
public:
    // virtual ~IMemAlloc();
    void* alloc(size_t size)
    {
        return g_utils.get_virtual_function<void*(__thiscall*)(decltype(this), size_t)>(this, 1)(this, size);
    }

    void* re_alloc(size_t size, void* mem)
    {
        return g_utils.get_virtual_function<void*(__thiscall*)(decltype(this), size_t, void*)>(this, 3)(this, size,
                                                                                                        mem);
    }

    void free(void* mem)
    {
        g_utils.get_virtual_function<void(__thiscall*)(decltype(this), void*)>(this, 5)(this, mem);
    }
};
