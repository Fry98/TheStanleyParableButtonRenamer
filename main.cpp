#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <windows.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <string>
#include <limits>
#include <cstdio>
#include <assert.h>
#include <filesystem>
#include "lib/vdf_parser.h"

#define run_command(cmd) CreateProcess( \
    NULL, cmd, NULL, NULL, FALSE, \
    NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, \
    NULL, NULL, &si, &pi \
  ); \
  WaitForSingleObject(pi.hProcess, INFINITE);

int main() {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  std::string user_name;

  ZeroMemory(&si, sizeof(si));
  ZeroMemory(&pi, sizeof(pi));

  // Get Steam profile name
  try {
    std::ifstream lu_if("C:\\Program Files (x86)\\Steam\\config\\loginusers.vdf");
    auto root = tyti::vdf::read(lu_if);
    if (root.name != "users") throw 0;

    auto it = root.childs.begin();
    if (it == root.childs.end()) throw 0;

    auto& user_data = *it->second;
    user_name = user_data.attribs["PersonaName"];
    if (user_name.length() < 1) throw 0;
    lu_if.close();
  } catch (...) {
    return 1;
  }

  try {
    // Create temp file with name
    std::ofstream name_of("TSPBR_data\\TSPR_name.tmp");
    name_of << user_name;
    name_of.close();

    // Synthesize voice line
    run_command("TSPBR_data\\voice.exe -v 75 -k TSPBR_data\\TSPR_name.tmp -o TSPBR_data\\0000.wav");
    std::remove("TSPBR_data\\TSPR_name.tmp");

    // Convert to FSB5
    if (!std::filesystem::exists("TSPBR_data\\0000.wav")) throw 0;
    run_command("TSPBR_data\\fsbankcl.exe -format vorbis -o TSPBR_data\\TSPBR_bank.fsb TSPBR_data\\0000.wav");

    if (!std::filesystem::exists("TSPBR_data\\TSPBR_bank.fsb")) throw 0;
    std::remove("TSPBR_data\\0000.wav");
  } catch (...) {
    return 1;
  }

  // Append FSB bank to the resource file
  size_t bank_size = 0, bank_off;
  try {
    std::ifstream bank_if("TSPBR_data\\TSPBR_bank.fsb", std::ifstream::binary);
    std::ofstream rsrc_of(
      "DATA\\sharedassets21.resource",
      std::ios::binary | std::ios::in | std::ios::out | std::ios::ate
    );

    bank_off = rsrc_of.tellp();
    while (!bank_if.eof()) {
      char ch = bank_if.get();
      rsrc_of.write(&ch, 1);
      bank_size++;
    }

    bank_if.close();
    rsrc_of.close();
    std::remove("TSPBR_data\\TSPBR_bank.fsb");
  } catch (...) {
    return 1;
  }

  try {
    // Find Jim button entry
    std::ifstream assets_if(
      "DATA\\sharedassets21.assets",
      std::ifstream::binary
    );

    std::string trg_str("ud_ncp2_jim_button");
    std::search(
      std::istreambuf_iterator<char>(assets_if),
      std::istreambuf_iterator<char>(),
      trg_str.begin(), trg_str.end()
    );

    size_t jb_off = assets_if.tellg();
    assets_if.close();

    // Update size and offset
    std::ofstream assets_of(
      "DATA\\sharedassets21.assets",
      std::ios::binary | std::ios::in | std::ios::out
    );

    assets_of.seekp(jb_off + 62, std::ios_base::cur);
    assets_of.write(reinterpret_cast<char*>(&bank_off), 8);
    assets_of.write(reinterpret_cast<char*>(&bank_size), 8);
    assets_of.close();

    CreateProcess(
      "TSPUD.exe",
      NULL, NULL, NULL,
      NULL, NULL, NULL, NULL,
      &si, &pi
    );
  } catch (...) {
    return 1;
  }
}
