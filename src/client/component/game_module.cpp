#include <std_include.hpp>
#include "loader/component_loader.hpp"

#ifdef INJECT_HOST_AS_LIB
#include <utils/hook.hpp>
#endif

#include <utils/nt.hpp>

#include "game_module.hpp"

namespace game_module {
#ifdef INJECT_HOST_AS_LIB
namespace {
utils::hook::detour handle_a_hook;
utils::hook::detour handle_w_hook;
utils::hook::detour handle_ex_a_hook;
utils::hook::detour handle_ex_w_hook;
utils::hook::detour file_name_a_hook;
utils::hook::detour file_name_w_hook;

HMODULE WINAPI get_module_handle_a(const LPCSTR module_name) {
  if (!module_name) {
    return get_game_module();
  }

  return handle_a_hook.invoke<HMODULE>(module_name);
}

HMODULE WINAPI get_module_handle_w(const LPWSTR module_name) {
  if (!module_name) {
    return get_game_module();
  }

  return handle_w_hook.invoke<HMODULE>(module_name);
}

BOOL WINAPI get_module_handle_ex_a(const DWORD flags, const LPCSTR module_name,
                                   HMODULE* hmodule) {
  if (!module_name) {
    *hmodule = get_game_module();
    return TRUE;
  }

  return handle_ex_a_hook.invoke<BOOL>(flags, module_name, hmodule);
}

BOOL WINAPI get_module_handle_ex_w(const DWORD flags, const LPCWSTR module_name,
                                   HMODULE* hmodule) {
  if (!module_name) {
    *hmodule = get_game_module();
    return TRUE;
  }

  return handle_ex_w_hook.invoke<BOOL>(flags, module_name, hmodule);
}

DWORD WINAPI get_module_file_name_a(HMODULE hmodule, const LPSTR filename,
                                    const DWORD size) {
  if (!hmodule) {
    hmodule = get_game_module();
  }

  return file_name_a_hook.invoke<DWORD>(hmodule, filename, size);
}

DWORD WINAPI get_module_file_name_w(HMODULE hmodule, const LPWSTR filename,
                                    const DWORD size) {
  if (!hmodule || utils::nt::library(hmodule) == get_game_module()) {
    hmodule = get_host_module();
  }

  return file_name_w_hook.invoke<DWORD>(hmodule, filename, size);
}

void hook_module_resolving() {
  handle_a_hook.create(&GetModuleHandleA, &get_module_handle_a);
  handle_w_hook.create(&GetModuleHandleW, &get_module_handle_w);
  handle_ex_w_hook.create(&GetModuleHandleExA, &get_module_handle_ex_a);
  handle_ex_w_hook.create(&GetModuleHandleExW, &get_module_handle_ex_w);
  file_name_a_hook.create(&GetModuleFileNameA, &get_module_file_name_a);
  file_name_w_hook.create(&GetModuleFileNameW, &get_module_file_name_w);
}
} // namespace
#endif

utils::nt::library get_game_module() {
  static utils::nt::library game{reinterpret_cast<HMODULE>(0x400000)};
  return game;
}

utils::nt::library get_host_module() {
  static utils::nt::library host{};
  return host;
}
class component final : public component_interface {
public:
  void post_start() override { get_host_module(); }

  void post_load() override {
#ifdef INJECT_HOST_AS_LIB
    hook_module_resolving();
#else
    assert(get_host_module() == get_game_module());
#endif
  }
};
} // namespace game_module

REGISTER_COMPONENT(game_module::component)