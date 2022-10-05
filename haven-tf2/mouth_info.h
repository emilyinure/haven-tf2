#pragma once

class c_audio_source;

class c_voice_data {
public:
	float m_elapsed;
	c_audio_source* m_audio_source;
	bool m_ignore_phonems;
};

class c_mouth_info {
public:
	byte m_mouth_open;
	byte m_snd_count;
	int	m_snd_avg;
private:
	enum {
		MAX_VOICE_DATA = 4
	};

	short m_num_voice_sources;
	short m_needs_envelope;
	c_voice_data m_voice_sources[ MAX_VOICE_DATA ];
};