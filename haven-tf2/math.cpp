float c_math::get_fraction(const float val, const float max, const float min)
{
    return std::clamp((val - min) / (max - min), 0.f, 1.f);
}
