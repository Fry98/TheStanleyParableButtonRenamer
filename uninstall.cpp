#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <windows.h>
#include <strsafe.h>
#include <filesystem>
#include <iostream>

#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del /f /q \"%s\"")

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

#define try_remove(path) \
  try { \
    std::remove(path); \
  } catch (...) {}

#define restore_backup(file) \
  try { \
    if (std::filesystem::exists("DATA\\" file ".bak")) { \
      try_remove("DATA\\" file); \
      std::filesystem::rename("DATA\\" file ".bak", "DATA\\" file); \
    } \
  } catch (...) {}

int main() {
  try_remove("config_TSPBR.json");
  try_remove("The Stanley Parable Ultra Deluxe.exe");

  restore_backup("sharedassets21.assets");
  restore_backup("sharedassets21.resource");

  try {
    std::filesystem::remove_all("TSPBR_data");
  } catch (...) {}

  try {
    std::filesystem::rename("TSPUD_Bootstrap", "The Stanley Parable Ultra Deluxe.exe");
  } catch (...) {}

  delete_self();
}
