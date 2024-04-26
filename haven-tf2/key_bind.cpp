#include "sdk.h"
#include "key_bind.h"

c_key_bind::c_key_bind(const char* name, const char* tooltip, int default_type) {
  this->m_name = name;
  this->value_ = std::make_shared<key_bind_t>(name, default_type);
  this->m_type = e_control_type::CONTROL_TYPE_KEYBIND;
  g_ui.append_bind(this->value_);
  this->m_tooltip = tooltip;
}

auto c_key_bind::paint() -> void {
  if (this->capturing_) {
    for (auto i = 0; i < 255; ++i) {
      if (GetAsyncKeyState(i) && i != 3) {
        if (i == VK_LBUTTON && g_input.hovering(this->item_area_))
          continue;

        if (i == VK_ESCAPE) {
          this->value_->key      = 0;
          g_ui.m_focused_control = nullptr;
          this->capturing_       = false;
          break;
        } else if (strcmp("No bind", g_input.key(i)) != 0) {
          this->value_->key      = i;
          g_ui.m_focused_control = nullptr;
          this->capturing_       = false;
          break;
        }
      }
    }
  }

  // set our dropdown area.
  this->dropdown_area_ = {this->m_area.m_x + (this->m_area.m_w - (this->m_area.m_w * 0.5f)),
                          this->m_area.m_y + 2, this->m_area.m_w * 0.5f, 14};

  // set our item area.
  this->item_area_ = {this->m_area.m_x + (this->m_area.m_w - (this->m_area.m_w * 0.5f)),
                      this->m_area.m_y + 2, this->m_area.m_w * 0.5f, 4 * 13.f};

  // background.
  if (!this->open_) {
    g_ui.m_draw.filled_rect(this->dropdown_area_, color(35, 35, 35, 40 * g_ui.m_fade));
    g_ui.m_draw.outlined_rect(this->dropdown_area_, color(35, 35, 35, 40 * g_ui.m_fade));
  }

  // current value.
  if (!this->open_)
    g_ui.m_draw.text(g_render.m_fonts.menu.main,
                     {this->dropdown_area_.m_x + 2, this->dropdown_area_.m_y},
                     g_input.key(this->value_->key), color(255, 255, 255, 100 * g_ui.m_fade),
                     e_text_alignment::text_align_left);

  g_ui.m_theme.m_a = 100 * g_ui.m_fade;
  // title.
  g_ui.m_draw.text(g_render.m_fonts.menu.main, {this->m_area.m_x, this->m_area.m_y + 2},
                   this->m_name, g_ui.m_theme, e_text_alignment::text_align_left);

  g_ui.m_theme.m_a = 255;
  // draw our items.
  if (!this->open_)
    return;

  // dropdown background.
  g_ui.m_draw.filled_rect(this->item_area_, color(35, 35, 35, 40 * g_ui.m_fade));
  g_ui.m_draw.outlined_rect(this->item_area_, color(35, 35, 35, 40 * g_ui.m_fade));

  g_ui.m_theme.m_a = 100 * g_ui.m_fade;
  // items.
  for (auto i = 0; i < 4; i++) {
    const auto& item = this->key_bind_types_[i];
    const bool  hovering =
        g_input.hovering({this->item_area_.m_x + 2, this->item_area_.m_y + 2 + (i * 13),
                          this->item_area_.m_w, 13});
    g_ui.m_draw.text(g_ui.m_draw.fonts.main,
                     {this->item_area_.m_x + 2, this->item_area_.m_y + (i * 13)}, item,
                     this->value_->type == i
                         ? g_ui.m_theme
                         : color(255, 255, 255, (hovering ? 150 : 100) * g_ui.m_fade),
                     e_text_alignment::text_align_left);
  }
  g_ui.m_theme.m_a = 255;
}

auto c_key_bind::update() -> void {
  // update our hover state.
  this->hovering_dropdown_ = g_input.hovering(this->dropdown_area_);
  this->hovering_items_    = g_input.hovering(this->item_area_);

  if ((hovering_dropdown_ && !this->open_) && g_input.key_pressed(VK_LBUTTON)) {
    g_input.set_key(VK_LBUTTON, false);
    this->capturing_       = true;
    g_ui.m_focused_control = this;
    return;
  }

  // we clicked our dropdown, open it.
  if ((hovering_dropdown_ && !this->open_) && g_input.key_pressed(VK_RBUTTON)) {
    this->open_            = true;
    g_ui.m_focused_control = this;

    // we want to skip it from selecting this frame, will keep
    // it from falsely selecting the first option.
    return;
  }

  // we're not hovering dropdown or items but we clicked, close.
  if (this->open_ && (!this->hovering_dropdown_ && !this->hovering_items_) &&
      g_input.key_pressed(VK_LBUTTON)) {
    g_input.set_key(VK_LBUTTON, false);
    g_ui.m_focused_control = nullptr;
    this->open_            = false;
    return;
  }

  if (!this->capturing_ && !this->open_)
    return;

  g_ui.m_focused_control = this;

  for (auto i = 0; i < 4; i++) {
    // we clicked the item, select it.
    if (g_input.hovering({this->item_area_.m_x + 2, this->item_area_.m_y + 2 + (i * 13),
                          this->item_area_.m_w, 13}) &&
        g_input.key_pressed(VK_LBUTTON)) {
      g_input.set_key(VK_LBUTTON, false);
      this->value_->type     = i;
      g_ui.m_focused_control = nullptr;
      this->open_            = false;
    }
  }
}
