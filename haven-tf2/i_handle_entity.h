#pragma once

class i_handle_entity {
public:
	virtual ~i_handle_entity( ) {}
	virtual void set_ref_e_handle( const c_base_handle& handle ) = 0;
	virtual const c_base_handle& get_ref_e_handle( ) const = 0;
};