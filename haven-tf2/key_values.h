#pragma once

// todo: maybe complete this?
class key_values {
public:
	int m_key_name;
	char* m_string_val;
	wchar_t* m_wide_string_val;

	union {
		int m_int_value;
		float m_float_value;
		void* m_ptr_value;
		unsigned char m_color[ 4 ];
	};

	char m_data_type;
	char m_has_escape_sequences;
	char m_evaluate_conditionala;
	char m_unused[ 1 ];
	key_values* m_peer;
	key_values* m_sub;
	key_values* m_chain;
};