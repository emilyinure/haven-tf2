#pragma once

// forward decl.
enum sound_level_t;

struct sound_info_t {
	int	m_sequence_number;
	int	m_entity_index;
	int m_channel;
	const char* pszName;
	vector m_origin;
	vector m_direction;
	float m_volume;
	sound_level_t m_sound_level;
	bool m_looping;
	int m_pitch;
	int m_special_dsp;
	vector m_listener_origin;
	int m_flags;
	int m_sound_num;
	float m_delay;
	bool m_is_sentence;
	bool m_is_ambient;
	int	m_speaker_entity;
};

struct spatialization_info_t {
	typedef enum {
		SI_INCREATION = 0,
		SI_INSPATIALIZATION
	} SPATIALIZATIONTYPE;

	SPATIALIZATIONTYPE m_type;
	sound_info_t m_info;
	vector* m_origin;
	vector* m_angles;
	float* m_radius;
};