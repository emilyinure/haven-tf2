#pragma once

class bf_write {
public:
	unsigned long* __restrict m_data;
	int	m_data_bytes;
	int m_data_bits;
	int m_cur_bit;
private:
	bool m_overflow;
	bool m_assert_on_overflow;
	const char* m_debug_name;
};

class bf_read {
	const unsigned char* __restrict m_data;
	int	m_data_bytes;
	int	m_data_bits;
	int	m_cur_bit;
private:
	bool m_overflow;
	bool m_assert_on_overflow;
	const char* m_debug_name;
};