#pragma once

class c_tier0 {
	typedef void( __cdecl* generic_msg_fn )( const char* message );

	module_t tier0_module_;
	generic_msg_fn msg_ = nullptr, con_warning_ = nullptr, dev_msg_ = nullptr;
public:
	void init( );
	void msg( const char* text ) const;
	void warning( const char* text ) const;
	void dev_msg( const char* text ) const;
}; inline c_tier0 g_tier0;