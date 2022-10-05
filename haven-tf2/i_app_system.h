#pragma once

enum init_return_val_t {
	INIT_FAILED = 0,
	INIT_OK,
	INIT_LAST_VAL,
};

class i_app_system {
public:
	virtual bool connect( create_interface_fn factory ) = 0;
	virtual void disconnect( ) = 0;
	virtual void* query_interface( const char* p_interface_name ) = 0;
	virtual init_return_val_t init( ) = 0;
	virtual void shutdown( ) = 0;
};