#include <windows.h>
#include <strsafe.h>
#include <filesystem>
#include <iostream>

#define SELF_REMOVE_STRING TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"")

#define IGNORE_ALL(expr) \
  try { expr; } catch (...) {}

#define restore_backup(file) \
  try { \
    if (std::filesystem::exists("DATA\\" file ".bak")) { \
      std::remove("DATA\\" file); \
      std::filesystem::rename("DATA\\" file ".bak", "DATA\\" file); \
    } \
  } catch (...) {}

void delete_self() {
  TCHAR szModuleName[MAX_PATH];
  TCHAR szCmd[2 * MAX_PATH];
  STARTUPINFO si = {0};
  PROCESS_INFORMATION pi = {0};

  GetModuleFileName(NULL, szModuleName, MAX_PATH);
  StringCbPrintf(szCmd, 2 * MAX_PATH, SELF_REMOVE_STRING, szModuleName);
  CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
}

int main() {
  IGNORE_ALL(std::remove("TSPBR_config.json"));
  IGNORE_ALL(std::remove("The Stanley Parable Ultra Deluxe.exe"));

  restore_backup("sharedassets21.assets");
  restore_backup("sharedassets21.resource");

  IGNORE_ALL(std::filesystem::remove_all("TSPBR_data"));
  IGNORE_ALL(std::filesystem::rename("TSPUD_Bootstrap", "The Stanley Parable Ultra Deluxe.exe"));
  IGNORE_ALL(std::filesystem::rename("DATA", "The Stanley Parable Ultra Deluxe_Data"));

  delete_self();
}
