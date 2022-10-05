#pragma once

class i_file_list {
public:
	virtual bool is_file_in_list( const char* file_name ) = 0;
	virtual void release( ) = 0;
};