#pragma once

class color {
public:
  int m_r, m_g, m_b, m_a;

  color(const int r, const int g, const int b, const int a = 255)
      : m_r(r), m_g(g), m_b(b), m_a(a) {}

  color operator-(const int val) const {
    color return_val = {255, 255, 255};
    return_val.m_r   = std::clamp(this->m_r - val, 0, 255);
    return_val.m_g   = std::clamp(this->m_g - val, 0, 255);
    return_val.m_b   = std::clamp(this->m_b - val, 0, 255);

    return return_val;
  }
};
