#include "file_dialog.h"
#if defined(__WINOS__)
#define NOMINMAX
#include <Windows.h>
#include <commdlg.h>
#include <vector>
#pragma comment(lib, "Comdlg32.lib")

namespace gb {
namespace {
file_dialog_result select_level(bool save) {
  file_dialog_result result;
  std::vector<wchar_t> path(32768, L'\0');
  OPENFILENAMEW dialog = {};
  dialog.lStructSize = sizeof(dialog);
  dialog.hwndOwner = GetActiveWindow();
  dialog.lpstrFilter = L"Game Box level (*.level)\0*.level\0\0";
  dialog.nFilterIndex = 1;
  dialog.lpstrFile = path.data();
  dialog.nMaxFile = static_cast<DWORD>(path.size());
  dialog.lpstrDefExt = L"level";
  dialog.lpstrTitle = save ? L"Save terrain level" : L"Load terrain level";
  dialog.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST |
                 (save ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST);
  BOOL accepted = save ? GetSaveFileNameW(&dialog) : GetOpenFileNameW(&dialog);
  if (!accepted) {
    DWORD code = CommDlgExtendedError();
    if (code != 0) {
      result.cancelled = false;
      result.error =
          "Native file dialog failed (" + std::to_string(code) + ").";
    }
    return result;
  }
  result.cancelled = false;
  int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, path.data(),
                                  -1, nullptr, 0, nullptr, nullptr);
  if (bytes <= 0) {
    result.error = "Cannot convert selected level path to UTF-8.";
    return result;
  }
  std::vector<char> utf8(bytes);
  if (!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, path.data(), -1,
                           utf8.data(), bytes, nullptr, nullptr)) {
    result.error = "Cannot convert selected level path to UTF-8.";
    return result;
  }
  result.path = utf8.data();
  return result;
}
} // namespace
file_dialog_result file_dialog::save_level() { return select_level(true); }
file_dialog_result file_dialog::open_level() { return select_level(false); }
bool file_dialog::confirm_discard_level(){return MessageBoxW(GetActiveWindow(),L"Discard unsaved level changes and load another level?",L"Unsaved level",MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES;}
} // namespace gb
#else
namespace gb {
namespace {
file_dialog_result unsupported() {
  file_dialog_result result;
  result.cancelled = false;
  result.error =
      "Native terrain level dialogs are unavailable on this platform.";
  return result;
}
} // namespace
file_dialog_result file_dialog::save_level() { return unsupported(); }
file_dialog_result file_dialog::open_level() { return unsupported(); }
bool file_dialog::confirm_discard_level(){return false;}
} // namespace gb
#endif
