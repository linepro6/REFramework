#include "../REFramework.hpp"

#include "REFrameworkConfig.hpp"

std::shared_ptr<REFrameworkConfig>& REFrameworkConfig::get() {
     static std::shared_ptr<REFrameworkConfig> instance{std::make_shared<REFrameworkConfig>()};
     return instance;
}

std::optional<std::string> REFrameworkConfig::on_initialize() {
    return Mod::on_initialize();
}

void REFrameworkConfig::on_draw_ui() {
    if (!ImGui::CollapsingHeader(_("Configuration"))) {
        return;
    }

    ImGui::TreePush(_("Configuration"));

    m_menu_key->draw(_("Menu Key"));
    m_remember_menu_state->draw(_("Remember Menu Open/Closed State"));

    if (m_font_size->draw(_("Font Size"))) {
        auto font_size = m_font_size->value();
        if (font_size > 5) {
            g_framework->set_font_size(font_size);
        } else {
            g_framework->set_font_size(5);
        }
    }

    ImGui::TreePop();
}

void REFrameworkConfig::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }

    if (m_remember_menu_state->value()) {
        g_framework->set_draw_ui(m_menu_open->value(), false);
    }
    
    auto font_size = m_font_size->value();
    if (font_size > 5) {
        g_framework->set_font_size(font_size);
    } else {
        g_framework->set_font_size(5);
    }
}

void REFrameworkConfig::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}
