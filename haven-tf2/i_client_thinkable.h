#pragma once

class c_client_think_handle_ptr;
typedef c_client_think_handle_ptr* client_think_handle_t;

class i_client_thinkable {
public:
	virtual i_client_unknown* get_i_client_unknown( ) = 0;
	virtual void client_think( ) = 0;
	virtual client_think_handle_t get_think_handle( ) = 0;
	virtual void set_think_handle( client_think_handle_t h_think ) = 0;
	virtual void release( ) = 0;
};