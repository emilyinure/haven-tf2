#pragma once
class player_record_t;
class c_backstab
{
public:
    bool last_valid = false;
    bool check_player(c_base_player* player);
    void run();
} inline g_backstab;
