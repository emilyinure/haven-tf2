#include "sdk.h"

void c_config::init() {
  const auto& windows = g_ui.m_windows;

  // no valid windows.
  if (windows.empty())
    return;

  for (const auto& window : windows) {
    this->windows_.emplace_back(window);

    auto& tabs = window->m_tabs;

    // no tabs in this window.
    if (tabs.empty())
      continue;

    for (const auto& tab : tabs) {
      auto& groupboxes = tab->m_groupboxes;

      // no groupboxes in this tab.
      if (groupboxes.empty())
        continue;

      for (const auto& groupbox : groupboxes) {
        auto& controls = groupbox->m_controls;

        // no controls in this groupbox.
        if (controls.empty())
          continue;

        for (auto& control : controls) {
          // place our controls into the list.
          this->config_controls_.emplace_back(control, window->m_name, tab->m_name,
                                              groupbox->m_name);
        }
      }
    }
  }
}

void c_config::save(const char* file_name) const {
  try {
    nlohmann::json json;
    // save our window position(s).
    for (auto& window : this->windows_) {
      json[window->m_name]["x"] = window->m_area.m_x;
      json[window->m_name]["y"] = window->m_area.m_y;
    }

    // save our controls.
    for (auto& config_control : this->config_controls_) {
      auto& control = config_control.m_control;

      // no reason to save buttons.
      if (control->m_type == e_control_type::CONTROL_TYPE_BUTTON)
        continue;

      auto& var = json[config_control.m_window_name][config_control.m_tab_name]
                      [config_control.m_groupbox_name][control->m_name];

      switch (control->m_type) {
        case e_control_type::CONTROL_TYPE_CHECKBOX: {
          const auto checkbox = dynamic_cast<c_checkbox*>(control.get());

          var = checkbox->m_value;
        } break;
        case e_control_type::CONTROL_TYPE_SLIDER: {
          const auto slider = dynamic_cast<c_slider*>(control.get());

          var = slider->m_value;
        } break;
        case e_control_type::CONTROL_TYPE_COMBOBOX: {
          const auto combobox = dynamic_cast<c_combobox*>(control.get());

          var = combobox->m_selected_index;
        } break;
        case e_control_type::CONTROL_TYPE_KEYBIND: {
          const auto keybind = dynamic_cast<c_key_bind*>(control.get());

          var["key"]  = keybind->value_->key;
          var["type"] = keybind->value_->type;
        } break;
        default:
          break;
      }
    }

    std::ofstream stream(file_name);
    stream << std::setw(4) << json << std::endl;
    stream.close();
  } catch (std::exception& e) {
    printf_s(__FUNCTION__ " %s\n", e.what());
  }
}

void c_config::load(const char* file_name) const {
  try {
    std::ifstream  stream(file_name);
    nlohmann::json json;
    stream >> json;
    stream.close();

    // load our window position(s).
    for (auto& window : this->windows_) {
      window->m_area.m_x = json[window->m_name]["x"].get<float>();
      window->m_area.m_y = json[window->m_name]["y"].get<float>();
    }

    // load our controls.
    for (auto& config_control : this->config_controls_) {
      auto& control = config_control.m_control;

      // no reason to load buttons.
      if (control->m_type == e_control_type::CONTROL_TYPE_BUTTON)
        continue;

      auto& var = json[config_control.m_window_name][config_control.m_tab_name]
                      [config_control.m_groupbox_name][control->m_name];

      // fixes an issue that breaks config if you add a new var.
      if (var.is_null())
        continue;

      switch (control->m_type) {
        case e_control_type::CONTROL_TYPE_CHECKBOX: {
          const auto checkbox = dynamic_cast<c_checkbox*>(control.get());

          checkbox->m_value = var.get<bool>();
        } break;
        case e_control_type::CONTROL_TYPE_SLIDER: {
          const auto slider = dynamic_cast<c_slider*>(control.get());

          slider->m_value = var.get<float>();
        } break;
        case e_control_type::CONTROL_TYPE_COMBOBOX: {
          const auto combobox = dynamic_cast<c_combobox*>(control.get());

          combobox->m_selected_index = var.get<int>();
        } break;
        case e_control_type::CONTROL_TYPE_KEYBIND: {
          const auto keybind = dynamic_cast<c_key_bind*>(control.get());

          keybind->value_->key  = var["key"].get<int>();
          keybind->value_->type = var["type"].get<int>();
        } break;
        default:
          break;
      }
    }
  } catch (std::exception& e) {
    printf_s(__FUNCTION__ " %s\n", e.what());
  }
}
