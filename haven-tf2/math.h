#pragma once

namespace math {
	float get_fraction( float val, float max, float min = 0 );

    bool world_to_screen(const vector& origin, vector& screen);
};