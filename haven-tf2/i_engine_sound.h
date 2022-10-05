#pragma once

// no reason to create a sep. file for this.
#define	PITCH_NORM		100	
#define PITCH_LOW		95
#define PITCH_HIGH		120

enum sound_level_t {
	SNDLVL_NONE = 0,
	SNDLVL_20dB = 20,
	SNDLVL_25dB = 25,
	SNDLVL_30dB = 30,
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45,
	SNDLVL_50dB = 50,
	SNDLVL_55dB = 55,
	SNDLVL_IDLE = 60,
	SNDLVL_60dB = 60,
	SNDLVL_65dB = 65,
	SNDLVL_STATIC = 66,
	SNDLVL_70dB = 70,
	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75,
	SNDLVL_80dB = 80,
	SNDLVL_TALKING = 80,
	SNDLVL_85dB = 85,
	SNDLVL_90dB = 90,
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100,
	SNDLVL_105dB = 105,
	SNDLVL_110dB = 110,
	SNDLVL_120dB = 120,	
	SNDLVL_130dB = 130,
	SNDLVL_GUNFIRE = 140,
	SNDLVL_140dB = 140,
	SNDLVL_150dB = 150,
	SNDLVL_180dB = 180,
};

class i_engine_sound {
public:
	virtual bool precache_sound( const char* sample, bool preload = false, bool is_ui_sound = false ) = 0;
	virtual bool is_sound_precached( const char* sample ) = 0;
	virtual void prefetch_sound( const char* sample ) = 0;
	virtual float get_sound_duration( const char* sample ) = 0;
	virtual void emit_sound( i_recipient_filter& filter, int ent_index, int channel, const char* sample, float fl_volume, float fl_attenuation, int flags = 0, int pitch = PITCH_NORM, int special_dsp = 0, const vector* origin = NULL, const vector* direction = NULL, c_utl_vector< vector >* utl_vec_origins = NULL, bool update_positions = true, float soundtime = 0.0f, int speakerentity = -1 ) = 0;
	virtual void emit_sound( i_recipient_filter& filter, int ent_index, int channel, const char* sample, float fl_volume, sound_level_t sound_level, int flags = 0, int pitch = PITCH_NORM, int special_dsp = 0, const vector* origin = NULL, const vector* direction = NULL, c_utl_vector< vector >* utl_vec_origins = NULL, bool update_positions = true, float soundtime = 0.0f, int speakerentity = -1 ) = 0;
	virtual void emit_sentence_by_index( i_recipient_filter& filter, int ent_index, int channel, int sentence_index, float fl_volume, sound_level_t sound_level, int flags = 0, int pitch = PITCH_NORM, int special_dsp = 0, const vector* origin = NULL, const vector* direction = NULL, c_utl_vector< vector >* utl_vec_origins = NULL, bool update_positions = true, float soundtime = 0.0f, int speakerentity = -1 ) = 0;
	virtual void stop_sound( int ent_index, int channel, const char* sample ) = 0;
	virtual void stop_all_sounds( bool clear_buffers ) = 0;
	virtual void set_room_type( i_recipient_filter& filter, int room_type ) = 0;
	virtual void set_player_dsp( i_recipient_filter& filter, int dsp_type, bool fast_reset ) = 0;
	virtual void emit_ambient_sound( const char* sample, float fl_volume, int pitch = PITCH_NORM, int flags = 0, float soundtime = 0.0f ) = 0;
	virtual float get_dist_gain_from_sound_level( sound_level_t sound_level, float dist ) = 0;
	virtual int get_guid_for_last_sound_emitted( ) = 0;
	virtual bool is_sound_still_playing( int guid ) = 0;
	virtual void stop_sound_by_guid( int guid ) = 0;
	virtual void set_volume_by_guid( int guid, float fvol ) = 0;
	virtual void get_active_sounds( c_utl_vector< snd_info_t >& sndlist ) = 0;
	virtual void precache_sentence_group( const char* group_name ) = 0;
	virtual void notify_begin_movie_playback( ) = 0;
	virtual void notify_end_movie_playback( ) = 0;
};