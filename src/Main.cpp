#include <chrono>
#include <mutex>
#include <thread>
#include <windows.h>
#include <winternl.h>

#include <utility/Module.hpp>
#include <utility/Thread.hpp>

#include "ExceptionHandler.hpp"
#include "REFramework.hpp"
#include "I18n.hpp"
#include <fstream>
#include <filesystem>
#include <memory>
namespace fs = std::filesystem;

HMODULE g_dinput = 0;
std::mutex g_load_mutex{};
std::unique_ptr<char[]> g_translateData;

void failed() {
    MessageBox(0, _("REFramework: Unable to load the original dinput8.dll. Please report this to the developer."), "REFramework", 0);
    ExitProcess(0);
}

bool load_dinput8() {
    std::scoped_lock _{g_load_mutex};

    if (g_dinput) {
        return true;
    }

    wchar_t buffer[MAX_PATH]{0};
    if (GetSystemDirectoryW(buffer, MAX_PATH) != 0) {
        // Load the original dinput8.dll
        if ((g_dinput = LoadLibraryW((std::wstring{buffer} + L"\\dinput8.dll").c_str())) == NULL) {
            failed();
            return false;
        }

        return true;
    }

    failed();
    return false;
}

extern "C" {
// DirectInput8Create wrapper for dinput8.dll
__declspec(dllexport) HRESULT WINAPI
    direct_input8_create(HINSTANCE hinst, DWORD dw_version, const IID& riidltf, LPVOID* ppv_out, LPUNKNOWN punk_outer) {
// This needs to be done because when we include dinput.h in DInputHook,
// It is a redefinition, so we assign an export by not using the original name
#pragma comment(linker, "/EXPORT:DirectInput8Create=direct_input8_create")

    load_dinput8();
    return ((decltype(DirectInput8Create)*)GetProcAddress(g_dinput, "DirectInput8Create"))(hinst, dw_version, riidltf, ppv_out, punk_outer);
}
}

void i18n_init() {
    std::string modpath{};

    modpath.resize(1024, 0);
    modpath.resize(GetModuleFileName(nullptr, modpath.data(), modpath.size()));

    //  auto i18n_config_path = fs::path{modpath}.parent_path() / "reframework" / "i18n" / "config.txt";
    //  
    //  std::ifstream config_file(i18n_config_path.string());
    //  std::string translate;
    //  if (config_file) {
    //      config_file >> translate;
    //      config_file.close();
    //  }
    // auto i18n_path = fs::path{modpath}.parent_path() / "reframework" / "i18n" /  "zh_CN.ymo";
    auto i18n_path = fs::path{modpath}.parent_path() / "zh_CN.ymo";
    std::ifstream file(i18n_path.string(), std::ios::binary | std::ios::ate);
    if (file) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        g_translateData = std::make_unique_for_overwrite<char[]>(static_cast<std::size_t>(size));
        if (file.read(g_translateData.get(), size)) {
            yi18n::LoadTranslateData(reinterpret_cast<yi18n::YMOData*>(g_translateData.get()));
        }
    }
}

void startup_thread(HMODULE reframework_module) {
    // We will set it once here, then do it continuously
    // every now and then because it gets replaced
    reframework::setup_exception_handler();
    i18n_init();

#ifndef NDEBUG
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    if (load_dinput8()) {
        g_framework = std::make_unique<REFramework>(reframework_module);

        const auto our_dll = utility::get_module_within(&load_dinput8);

#ifdef MHRISE
        if (our_dll) {
            utility::spoof_module_paths_in_exe_dir();
            utility::unlink(*our_dll);
        }
#endif
    }
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startup_thread, handle, 0, nullptr);
    }

    return TRUE;
}
