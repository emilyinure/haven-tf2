#pragma once

class c_client_state
{
public:
    void* vtables[3];
    int socket;
    i_net_channel_info* network_channel;
    char pad1[320];
    struct c_clock_drift
    {
    public:
        float clock_offsets[16];
        int current_offset = 0;
        int server_tick = 0;
        int client_tick = 0;
    } clock_drift;
    int delta_tick;
    char pad2[272];
    int maxClients;
    char pad3[18540];
    int lastOutgoingCommand;
    int chokedCommands;
    int lastCommandAck;
};
