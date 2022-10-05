#pragma once

class i_console_display_func {
public:
	virtual void color_print( const color& clr, const char* p_message ) = 0;
	virtual void print( const char* message ) = 0;
	virtual void d_print( const char* message ) = 0;
};

class i_cvar_query : public i_app_system {
public:
	virtual bool are_con_vars_linkable( const convar* child, const convar* parent ) = 0;
};

class i_cvar : public i_app_system {
public:
	virtual cvar_dll_identifier_t allocate_dll_identifier( ) = 0;
	virtual void register_con_command( con_command_base* command_base ) = 0;
	virtual void unregister_con_command( con_command_base* command_base ) = 0;
	virtual void unregister_con_commands( cvar_dll_identifier_t id ) = 0;
	virtual const char* get_command_line_value( const char* variable_name ) = 0;
	virtual con_command_base* find_command_base( const char* name ) = 0;
	virtual const con_command_base* find_command_base( const char* name ) const = 0;
	virtual convar* find_var( const char* var_name ) = 0;
	virtual const convar* find_var( const char* var_name ) const = 0;
	virtual con_command* find_command( const char* name ) = 0;
	virtual const con_command* find_command( const char* name ) const = 0;
	virtual con_command_base* get_commands( void ) = 0;
	virtual const con_command_base* get_commands( void ) const = 0;
	virtual void install_global_change_callback( fn_change_callback_t callback ) = 0;
	virtual void remove_global_change_callback( fn_change_callback_t callback ) = 0;
	virtual void call_global_change_callbacks( convar* var, const char* old_string, float fold_value ) = 0;
	virtual void install_console_display_func( i_console_display_func* display_func ) = 0;
	virtual void remove_console_display_func( i_console_display_func* display_func ) = 0;
	virtual void console_color_printf( const color& clr, const char* format, ... ) const = 0;
	virtual void console_printf( const char* format, ... ) const = 0;
	virtual void console_d_printf( const char* format, ... ) const = 0;
	virtual void revert_flagged_convars( int flag ) = 0;
	virtual void install_cvar_query( i_cvar_query* p_query ) = 0;
	virtual bool is_material_thread_set_allowed( ) const = 0;
	virtual void queue_material_thread_set_value( convar* cvar, const char* value ) = 0;
	virtual void queue_material_thread_set_value( convar* cvar, int value ) = 0;
	virtual void queue_material_thread_set_value( convar* cvar, float value ) = 0;
	virtual bool has_queued_material_thread_convar_sets( ) const = 0;
	virtual int process_queued_material_thread_con_var_sets( ) = 0;
protected: class icvar_iterator_internal;
public:
	class iterator {
	public:
		inline iterator( i_cvar* icvar );
		inline ~iterator( void );
		inline void set_first( void );
		inline void next( void );
		inline bool is_valid( void );
		inline con_command_base* get( void );
	private:
		icvar_iterator_internal* m_iter;
	};
protected:
	class icvar_iterator_internal {
	public:
		virtual ~icvar_iterator_internal( ) {}
		virtual void set_first( void ) = 0;
		virtual void next( void ) = 0;
		virtual bool is_valid( void ) = 0;
		virtual con_command_base* get( void ) = 0;
	};
	virtual icvar_iterator_internal* factory_internal_iterator( void ) = 0;
	friend class iterator;
};