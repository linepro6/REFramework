#pragma once

#include <deque>
#include <unordered_set>

#include <utility/FunctionHook.hpp>

#include "../Mod.hpp"
#include "I18n.hpp"

class LooseFileLoader : public Mod {
public:
    static std::shared_ptr<LooseFileLoader>& get();

public:
    std::string_view get_name() const override { return _("LooseFileLoader"); }

    std::optional<std::string> on_initialize() override;
    void on_config_load(const utility::Config& cfg) override;
    void on_config_save(utility::Config& cfg) override;
    
    void on_frame() override;
    void on_draw_ui() override;

private:
    void hook();
    bool handle_path(const wchar_t* path);
    static uint64_t path_to_hash_hook(const wchar_t* path);

    bool m_hook_success{false};
    bool m_attempted_hook{false};
    uint32_t m_files_encountered{};
    uint32_t m_loose_files_loaded{};

    std::shared_mutex m_mutex{};
    std::deque<std::wstring> m_recent_accessed_files{}; // max 100
    std::deque<std::wstring> m_recent_loose_files{}; // max 100
    std::unordered_set<std::wstring> m_all_accessed_files{};
    std::unordered_set<std::wstring> m_all_loose_files{};

    std::unique_ptr<FunctionHook> m_path_to_hash_hook{nullptr};

    ModToggle::Ptr m_enabled{ ModToggle::create(generate_name("Enabled")) };
    bool m_show_recent_files{false}; // Not persistent because its for dev purposes

    ValueList m_options{
        *m_enabled
    };
};