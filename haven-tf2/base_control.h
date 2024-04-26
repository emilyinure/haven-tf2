#pragma once

enum e_control_type {
  CONTROL_TYPE_INVALID = -1,
  CONTROL_TYPE_WINDOW,
  CONTROL_TYPE_TAB,
  CONTROL_TYPE_GROUPBOX,
  CONTROL_TYPE_CHECKBOX,
  CONTROL_TYPE_BUTTON,
  CONTROL_TYPE_SLIDER,
  CONTROL_TYPE_COMBOBOX,
  CONTROL_TYPE_KEYBIND
};

class c_base_control {
  float last_non_hover_time_ = 0.f;

public:
  const char*    m_name    = "invalid control";
  e_control_type m_type    = e_control_type::CONTROL_TYPE_INVALID;
  box_t          m_area    = {0, 0, 0, 0};
  const char*    m_tooltip = "";

  virtual void paint()  = 0;
  virtual void update() = 0;
  void         paint_tooltip();
};
