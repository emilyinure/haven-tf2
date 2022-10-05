#pragma once

class recv_table;
class recv_prop;
class c_recv_decoder;
class c_recv_proxy_data;

typedef void ( *recv_var_proxy_fn )( const c_recv_proxy_data* data, void* strct, void* out );
typedef void ( *array_length_proxy_fn )( void* strct, int object_id, int current_array_length );
typedef void ( *data_table_recv_var_proxy_fn )( const recv_prop* prop, void** out, void* data, int object_id );

typedef enum {
	dpt_int = 0,
	dpt_float,
	dpt_vector,
	dpt_vector_x_y,
	dpt_string,
	dpt_array,
	dpt_data_table,
	dpt_num_send_prop_types
} e_send_prop_types;

class d_variant {
public:
	union {
		float m_float;
		int m_int;
		const char* m_string;
		void* m_data;
		float m_vector[ 3 ];
	};

	e_send_prop_types m_type;
};

class c_recv_proxy_data {
public:
	const recv_prop* m_recv_prop;
	d_variant m_value;
	int	m_element;
	int m_ObjectID;
};

class c_standard_recv_proxies {
public:
	recv_var_proxy_fn m_int_32_to_int_8;
	recv_var_proxy_fn m_int_32_to_int_16;
	recv_var_proxy_fn m_int_32_to_int_32;
	recv_var_proxy_fn m_float_to_float;
	recv_var_proxy_fn m_vector_to_vector;
};

class recv_prop {
public:
	const char* m_var_name;
	e_send_prop_types m_recv_type;
	int m_flags;
	int	m_string_buffer_size;
	bool m_inside_array;
	const void* m_extra_data;
	recv_prop* m_array_prop;
	array_length_proxy_fn m_array_length_proxy;
	recv_var_proxy_fn m_proxy_fn;
	data_table_recv_var_proxy_fn m_data_table_proxy_fn;
	recv_table* m_data_table;
	int	m_offset;
	int	m_element_stride;
	int	m_num_of_elements;
	const char* m_parent_array_prop_name;
};

class recv_table {
public:
	recv_prop* m_props;
	int	m_num_of_props;
	c_recv_decoder* m_decoder;
	const char* m_net_table_name;
private:
	bool m_initialized;
	bool m_in_main_list;
};
