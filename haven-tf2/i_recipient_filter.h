#pragma once

class i_recipient_filter {
public:
	virtual	~i_recipient_filter( ) {}
	virtual bool is_reliable( void ) const = 0;
	virtual bool is_init_message( void ) const = 0;
	virtual int get_recipient_count( void ) const = 0;
	virtual int get_recipient_index( int slot ) const = 0;
};