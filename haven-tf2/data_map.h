#pragma once

class i_save_restore_ops;
struct optimized_data_map_t;
struct data_map_t;

struct input_func_t {

};

typedef enum _field_types {
	FIELD_VOID = 0,
	FIELD_FLOAT,
	FIELD_STRING,
	FIELD_VECTOR,
	FIELD_QUATERNION,
	FIELD_INTEGER,
	FIELD_BOOLEAN,
	FIELD_SHORT,
	FIELD_CHARACTER,
	FIELD_COLOR32,
	FIELD_EMBEDDED,
	FIELD_CUSTOM,
	FIELD_CLASSPTR,
	FIELD_EHANDLE,
	FIELD_EDICT,
	FIELD_POSITION_VECTOR,
	FIELD_TIME,
	FIELD_TICK,
	FIELD_MODELNAME,
	FIELD_SOUNDNAME,
	FIELD_INPUT,
	FIELD_FUNCTION,
	FIELD_VMATRIX,
	FIELD_VMATRIX_WORLDSPACE,
	FIELD_MATRIX3X4_WORLDSPACE,
	FIELD_INTERVAL,
	FIELD_MODELINDEX,
	FIELD_MATERIALINDEX,
	FIELD_VECTOR2D,
	FIELD_INTEGER64,
	FIELD_VECTOR4D,
	FIELD_TYPECOUNT,
} field_type_t;

enum {
	TD_OFFSET_NORMAL = 0,
	TD_OFFSET_PACKED = 1,
	TD_OFFSET_COUNT,
};

struct type_description_t {
	field_type_t m_field_ype;
	const char* m_field_name;
	int	m_field_offset;
	unsigned short m_field_size;
	short m_flags;
	const char* m_external_name;
	i_save_restore_ops* m_save_restore_ops;
	input_func_t	m_input_func;
	data_map_t* td;
	int	m_field_size_in_bytes;
	struct type_description_t* m_override_field;
	int	m_override_count;
	float m_field_tolerance;
	int	m_flat_offset[ TD_OFFSET_COUNT ];
	unsigned short m_flat_group;
};

struct data_map_t {
	type_description_t* m_data_desc;
	int	m_data_num_fields;
	char const* m_data_class_name;
	data_map_t* m_base_map;
	int	m_packed_size;
	optimized_data_map_t* m_optimized_data_map;
};