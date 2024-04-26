#pragma once

// todo: remove when i add traceray.
struct ray_t {
  vector_aligned m_Start;       // starting point, centered within the extents
  vector_aligned m_Delta;       // direction + length of the ray
  vector_aligned m_StartOffset; // Add this to m_Start to get the actual ray start
  vector_aligned m_Extents;     // Describes an axis aligned box extruded along a ray
  bool           m_IsRay;       // are the extents zero?
  bool           m_IsSwept;     // is delta != 0?

  void initialize(const vector& start, const vector& end) {
    m_Delta = end - start;

    m_IsSwept = (m_Delta.length_sqr() != 0);

    m_Extents.init();
    m_IsRay = true;

    m_StartOffset.init();
    m_Start = start;
  }

  void initialize(vector const& start, vector const& end, vector const& mins,
                  vector const& maxs) {
    m_Delta = end - start;

    m_IsSwept = (m_Delta.length_sqr() != 0);

    m_Extents = (maxs - mins);
    m_Extents *= 0.5f;
    m_IsRay = (m_Extents.length_sqr() < 1e-6);

    // Offset m_Start to be in the center of the box...
    m_StartOffset = (mins + maxs);
    m_StartOffset *= 0.5f;
    m_Start = (start + m_StartOffset);
    m_StartOffset *= -1.0f;
  }
  ray_t() {}
  ray_t(vector vecStart, vector vecEnd) { initialize(vecStart, vecEnd); }
  ray_t(vector vecStart, vector vecEnd, vector min, vector max) {
    initialize(vecStart, vecEnd, min, max);
  }
};

class i_spatial_leaf_enumerator {
public:
  virtual bool enumerate_leaf(int leaf, int context) = 0;
};

class i_spatial_query {
public:
  virtual int  leaf_count() const                                                         = 0;
  virtual bool enumerate_leaves_at_point(vector const& pt, i_spatial_leaf_enumerator* p_enum,
                                         int context)                                     = 0;
  virtual bool enumerate_leaves_in_box(vector const& mins, vector const& maxs,
                                       i_spatial_leaf_enumerator* p_enum, int context)    = 0;
  virtual bool enumerate_leaves_in_sphere(vector const& center, float radius,
                                          i_spatial_leaf_enumerator* p_enum, int context) = 0;
  virtual bool enumerate_leaves_along_ray(ray_t const& ray, i_spatial_leaf_enumerator* p_enum,
                                          int context)                                    = 0;
};
