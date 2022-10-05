#pragma once

class c_renamed_recv_table_info {
public:
	const char* m_old_name;
	const char* m_new_name;
	c_renamed_recv_table_info* m_next;
};