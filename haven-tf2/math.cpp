#include "math.h"
#include "interfaces.h"
#include "client.h"
namespace math
{
    float get_fraction(const float val, const float max, const float min)
    {
        return std::clamp((val - min) / (max - min), 0.f, 1.f);
    }

    bool world_to_screen(const vector& origin, vector& screen)
    {
        const auto& matrix = g_cl.m_view_matrix;

        const auto w = matrix.m_mat[3][0] * origin.m_x + matrix.m_mat[3][1] * origin.m_y +
                       matrix.m_mat[3][2] * origin.m_z + matrix.m_mat[3][3];
        if (w < 0.001f)
            return false;

        int x, y;
        g_interfaces.m_engine->get_screen_size(x, y);

        screen.m_x = static_cast<float>(x) / 2.0f;
        screen.m_y = static_cast<float>(y) / 2.0f;

        screen.m_x *= 1.0f + (matrix.m_mat[0][0] * origin.m_x + matrix.m_mat[0][1] * origin.m_y +
                              matrix.m_mat[0][2] * origin.m_z + matrix.m_mat[0][3]) /
                                 w;
        screen.m_y *= 1.0f - (matrix.m_mat[1][0] * origin.m_x + matrix.m_mat[1][1] * origin.m_y +
                              matrix.m_mat[1][2] * origin.m_z + matrix.m_mat[1][3]) /
                                 w;

        return true;
    }
} // namespace math