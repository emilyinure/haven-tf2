#pragma once

enum should_transmit_state_t {
	SHOULDTRANSMIT_START = 0,
	SHOULDTRANSMIT_END
};

enum data_update_type_t {
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

class i_client_networkable {
public:
	virtual i_client_unknown* get_i_client_unknown( ) = 0;
	virtual void release( ) = 0;
	virtual client_class* get_client_class( ) = 0;
	virtual void notify_should_transmit( should_transmit_state_t state ) = 0;
	virtual void on_pre_data_changed( data_update_type_t update_type ) = 0;
	virtual void on_data_changed( data_update_type_t update_type ) = 0;
	virtual void pre_data_update( data_update_type_t update_type ) = 0;
	virtual void post_data_update( data_update_type_t update_type ) = 0;
	virtual bool is_dormant( void ) = 0;
	virtual int entindex( void ) const = 0;
	virtual void receive_message( int class_id, bf_read& msg ) = 0;
	virtual void* get_data_table_base_ptr( ) = 0;
	virtual void set_destroyed_on_recreate_entities( void ) = 0;
	virtual void on_data_unchanged_in_pvs( ) = 0;
};