#pragma once

class c_input {
	bool key_state_[ 256 ] = { }, last_key_state_[ 256 ] = { }, prestine_key_state_[ 256 ] = { };
	const char* keys_[ 253 ] = { "Unassigned", "Left Mouse", "Right Mouse", "Control+Break", "Middle Mouse", "Mouse 4", "Mouse 5", "No bind", "Backspace", "TAB", "No bind", "No bind", "No bind", "ENTER", "No bind", "No bind", "SHIFT", "CTRL", "ALT", "PAUSE", "CAPS LOCK", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "ESC", "No bind", "No bind", "No bind", "No bind", "Spacebar", "Page up", "Page down", "End", "Home", "Left", "Up", "Right", "Down", "No bind", "Print", "No bind", "Print Screen", "Insert", "Delete", "No bind", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Left Windows", "Right Windows", "No bind", "No bind", "No bind", "NUM 0", "NUM 1", "NUM 2", "NUM 3", "NUM 4", "NUM 5", "NUM 6", "NUM 7", "NUM 8", "NUM 9", "*", "+", "_", "-", ".", "/", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "NUM LOCK", "SCROLL LOCK", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "LSHIFT", "RSHIFT", "LCONTROL", "RCONTROL", "LMENU", "RMENU", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "Next Track", "Previous Track", "Stop", "Play/Pause", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", ";", "+", ",", "-", ".", "/?", "~", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "[{", "\\|", "}]", "'\"", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind", "No bind" };

public:
	vector_2d m_mouse_pos = { };

	void on_wnd_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
	void poll( );
	bool key_pressed( int key ) const;
	void set_key( const int key, bool state );
	bool key_down( int key ) const;
	const char* key( int key ) const;
	bool hovering( box_t area ) const;

	auto key_pressed_prestine( const int key ) const -> bool;
}; inline c_input g_input;