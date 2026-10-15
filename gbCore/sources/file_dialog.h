#pragma once
#include <string>

namespace gb {
struct file_dialog_result {
  std::string path, error;
  bool cancelled = true;
};

// Native platform UI only; callers own level serialization and replacement.
class file_dialog {
public:
  static file_dialog_result save_level();
  static file_dialog_result open_level();
  static bool confirm_discard_level();
};
} // namespace gb
