#pragma once

class i_client_entity_list
{
public:
    virtual i_client_networkable* get_client_networkable(int ent_num) = 0;
    virtual i_client_networkable* get_client_networkable_from_handle(c_base_handle& ent) = 0;
    virtual i_client_unknown* get_client_unknown_from_handle(c_base_handle &ent) = 0;

private:
    virtual i_client_entity* get_client_entity(int ent_num) = 0;
    virtual i_client_entity* get_client_entity_from_handle(c_base_handle& ent) = 0;

public:
    virtual int number_of_entities(bool include_non_networkable) = 0;
    virtual int get_highest_entity_index(void) = 0;
    virtual void set_max_entities(int max_ents) = 0;
    virtual int get_max_entities() = 0;

    template <typename T> T* get_entity(int ent_num)
    {
        return reinterpret_cast<T*>(this->get_client_entity(ent_num));
    }

    template <typename T> T* get_entity(c_base_handle ent)
    {
        return reinterpret_cast<T*>(this->get_client_entity_from_handle(ent));
    }
};
