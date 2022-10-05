#pragma once
enum stereo_eye_t {
	STEREO_EYE_MONO = 0,
	STEREO_EYE_LEFT = 1,
	STEREO_EYE_RIGHT = 2,
	STEREO_EYE_MAX = 3,
};

class c_view_setup {
public:
	int	m_x;
	int	m_unscaled_x;
	int	m_y;
	int	m_unscaled_y;
	int	m_width;
	int	m_unscaled_width;
	int	m_height;
	stereo_eye_t m_stereo_eye;
	int	m_unscaled_height;
	bool m_ortho;
	float m_ortho_left;
	float m_ortho_top;
	float m_ortho_right;
	float m_ortho_bottom;
	float m_fov;
	float m_fov_viewmodel;
	vector m_origin;
	vector m_angles;
	float m_near;
	float m_far;
	float m_near_view_model;
	float m_far_view_model;
	bool m_render_to_subrect_of_largerScreen;
	float m_aspect_ratio;
	bool m_off_center;
	float m_off_center_top;
	float m_off_center_bottom;
	float m_off_center_left;
	float m_off_center_right;
	bool m_do_bloom_and_tone_mapping;
	bool m_cache_full_screen_state;
	bool m_view_to_projection_override;
	view_matrix m_view_to_projection;
};