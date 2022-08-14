#pragma once

#include "Mod.hpp"
#include "I18n.hpp"

class Graphics : public Mod {
public:
    std::string_view get_name() const override { return _("Graphics"); };

    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;

    void on_draw_ui() override;

    void on_pre_application_entry(void* entry, const char* name, size_t hash) override;
    void on_application_entry(void* entry, const char* name, size_t hash) override;

    bool on_pre_gui_draw_element(REComponent* gui_element, void* primitive_context) override;

private:
    void do_ultrawide_fix();
    void do_ultrawide_fov_restore(bool force = false);
    void set_vertical_fov(bool enable);

    float m_old_fov{90.0f};

    const ModToggle::Ptr m_ultrawide_fix{ ModToggle::create(generate_name("UltrawideFix"), false) };
    const ModToggle::Ptr m_ultrawide_vertical_fov{ ModToggle::create(generate_name("UltrawideFixVerticalFOV"), true) };
    const ModToggle::Ptr m_ultrawide_fov{ ModToggle::create(generate_name("UltrawideFixFOV"), true) };
    const ModSlider::Ptr m_ultrawide_fov_multiplier{ ModSlider::create(generate_name("UltrawideFOVMultiplier"), 0.01f, 3.0f, 0.5f) };
    const ModToggle::Ptr m_disable_gui{ ModToggle::create(generate_name("DisableGUI"), false) };

    ValueList m_options{
        *m_ultrawide_fix,
        *m_ultrawide_vertical_fov,
        *m_ultrawide_fov,
        *m_ultrawide_fov_multiplier,
        *m_disable_gui
    };
};
