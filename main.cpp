#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <windows.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <limits>
#include <filesystem>

#include "lib/vdf_parser.hpp"
#include "lib/json.hpp"

#define run_cmd(cmd) CreateProcess( \
    NULL, cmd, NULL, NULL, FALSE, \
    NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, \
    NULL, NULL, &si, &pi \
  ); \
  WaitForSingleObject(pi.hProcess, INFINITE);

#define start_game() CreateProcess( \
    "TSPUD_Bootstrap", \
    NULL, NULL, NULL, \
    NULL, NULL, NULL, NULL, \
    &si, &pi \
  );

using json = nlohmann::json;

int main() {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  std::string user_name;

  char old_name[48] = {};
  int64_t old_size = -1;

  ZeroMemory(&si, sizeof(si));
  ZeroMemory(&pi, sizeof(pi));

  // Rename game data folder
  if (!std::filesystem::exists("DATA")) {
    try {
      std::filesystem::rename("The Stanley Parable Ultra Deluxe_Data", "DATA");
    } catch (...) {
      MessageBox(NULL, "Unable to access game data folder", "TSPBR Error", MB_ICONERROR | MB_OK);
      return 1;
    }
  }

  // Load mdat file
  std::ifstream mdat_is("TSPBR_data\\mdat");
  if (!mdat_is.fail()) {
    mdat_is.read(reinterpret_cast<char*>(&old_size), 8);

    size_t i = 0;
    while (!mdat_is.eof())
      mdat_is.read(old_name + i++, 1);

    mdat_is.close();
  }

  try {
    // Load config file
    std::ifstream conf_is("config.json");
    if (conf_is.fail()) throw std::exception("Unable to load config.json");

    std::string steam_install;
    try {
      auto& conf = json::parse(conf_is, nullptr, true, true);
      steam_install = conf["steam_install"].get<std::string>();
      user_name = conf["force_name"].get<std::string>();
      conf_is.close();
    } catch (...) {
      throw std::exception("Unable to parse config.json");
    }

    // Get Steam profile name
    if (user_name.length() < 1) {
      try {
        std::ifstream lu_if;
        if (steam_install.length() < 1) {
          lu_if.open("..\\..\\..\\config\\loginusers.vdf");
        } else {
          std::filesystem::path steam_base(steam_install);
          lu_if.open(steam_base / "config\\loginusers.vdf");
        }
        if (lu_if.fail()) throw 0;

        auto root = tyti::vdf::read(lu_if);
        if (root.name != "users") throw 0;

        auto it = root.childs.begin();
        if (it == root.childs.end()) throw 0;

        auto& user_data = *it->second;
        user_name = user_data.attribs["PersonaName"];
        if (user_name.length() < 1) throw 0;
        lu_if.close();
      } catch (...) {
        throw std::exception("Unable to read Steam profile name");
      }
    }

    // Early exit
    if (old_size > -1 && user_name == old_name) {
      start_game();
      return 0;
    }

    try {
      // Create temp file with name
      std::ofstream name_of("TSPBR_data\\TSPR_name.tmp");
      name_of << user_name;
      name_of.close();

      // Synthesize voice line
      run_cmd("TSPBR_data\\voice.exe -v 75 -k TSPBR_data\\TSPR_name.tmp -o TSPBR_data\\0000.wav");
      std::remove("TSPBR_data\\TSPR_name.tmp");

      // Convert to FSB5
      if (!std::filesystem::exists("TSPBR_data\\0000.wav")) throw 0;
      run_cmd("TSPBR_data\\fsbankcl.exe -format vorbis -o TSPBR_data\\TSPBR_bank.fsb TSPBR_data\\0000.wav");

      if (!std::filesystem::exists("TSPBR_data\\TSPBR_bank.fsb")) throw 0;
      std::remove("TSPBR_data\\0000.wav");
    } catch (...) {
      throw std::exception("Unable to synthesize voice");
    }

    // Append FSB bank to the resource file
    size_t bank_size = 0, bank_off;
    try {
      std::ifstream bank_if("TSPBR_data\\TSPBR_bank.fsb", std::ifstream::binary);
      if (bank_if.fail()) throw 0;

      std::ofstream rsrc_of(
        "DATA\\sharedassets21.resource",
        std::ios::binary | std::ios::in | std::ios::out | std::ios::ate
      );

      if (old_size > -1) rsrc_of.seekp(-old_size, std::ios_base::end);
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
      throw std::exception("Unable to modify .resource file");
    }

    try {
      // Find Jim button entry
      std::ifstream assets_if("DATA\\sharedassets21.assets", std::ifstream::binary);
      if (assets_if.fail()) throw 0;

      std::string trg_str("ud_ncp2_jim_button");
      std::ignore = std::search(
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
    } catch (...) {
      throw std::exception("Unable to modify .assets file");
    }

    try {
      std::ofstream mdat_os("TSPBR_data\\mdat");
      mdat_os.write(reinterpret_cast<char*>(&bank_size), 8);
      mdat_os << user_name;
      mdat_os.close();
    } catch (...) {
      throw std::exception("Unable to write into mdat");
    }
  } catch (std::exception& e) {
    MessageBox(NULL, e.what(), "TSPBR Error", MB_ICONERROR | MB_OK);
    return 1;
  }

  start_game();
}
