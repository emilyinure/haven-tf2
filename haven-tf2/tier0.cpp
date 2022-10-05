#include "sdk.h"

void c_tier0::init( ) {
	this->tier0_module_ = g_modules.get( "tier0.dll" );

	this->msg_ = this->tier0_module_.get_proc_address< generic_msg_fn >( "Msg" );
	this->con_warning_ = this->tier0_module_.get_proc_address< generic_msg_fn >( "?ConWarning@@YAXPBDZZ" );
	this->dev_msg_ = this->tier0_module_.get_proc_address< generic_msg_fn >( "?DevMsg@@YAXPBDZZ" );
}

// dunno why we have these.
void c_tier0::msg( const char* text ) const {
	if ( !this->msg_ )
		return;

	this->msg_( text );
}

void c_tier0::warning( const char* text ) const {
	if ( !this->con_warning_ )
		return;

	this->con_warning_( text );
}

void c_tier0::dev_msg( const char* text ) const {
	if ( !this->dev_msg_ )
		return;

	this->dev_msg_( text );
}