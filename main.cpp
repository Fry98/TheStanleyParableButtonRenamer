#define UNICODE

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

int main() {
  std::string user_name;

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
    std::cerr << "Unable to read Steam profile name\n";
    return 1;
  }

  try {
    // Create temp file with name
    std::ofstream name_of("TSPBR_data\\TSPR_name.tmp");
    name_of << user_name;
    name_of.close();

    // Synthesize voice line
    system("TSPBR_data\\voice.exe -v 60 -k TSPBR_data\\TSPR_name.tmp -o TSPBR_data\\0000.wav > NUL");
    std::remove("TSPBR_data\\TSPR_name.tmp");

    // Convert to FSB5
    if (!std::filesystem::exists("TSPBR_data\\0000.wav")) throw 0;
    system("TSPBR_data\\fsbankcl.exe -format vorbis -o TSPBR_data\\TSPBR_bank.fsb TSPBR_data\\0000.wav > NUL");

    if (!std::filesystem::exists("TSPBR_data\\TSPBR_bank.fsb")) throw 0;
    std::remove("TSPBR_data\\0000.wav");
  } catch (...) {
    std::cerr << "Unable to synthesize voice\n";
    return 1;
  }

  // Append FSB bank to the resource file
  size_t bank_size = 0, bank_off;
  try {
    std::ifstream bank_if("TSPBR_data\\TSPBR_bank.fsb", std::ifstream::binary);
    std::ofstream rsrc_of(
      "The Stanley Parable Ultra Deluxe_Data\\sharedassets21.resource",
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
    std::cerr << "Unable to modify .resource file\n";
    return 1;
  }

  try {
    // Find Jim button entry
    std::ifstream assets_if(
      "The Stanley Parable Ultra Deluxe_Data\\sharedassets21.assets",
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
      "The Stanley Parable Ultra Deluxe_Data\\sharedassets21.assets",
      std::ios::binary | std::ios::in | std::ios::out
    );

    assets_of.seekp(jb_off + 62, std::ios_base::cur);
    assets_of.write(reinterpret_cast<char*>(&bank_off), 8);
    assets_of.write(reinterpret_cast<char*>(&bank_size), 8);
    assets_of.close();

    return 0;
  } catch (...) {
    throw;
    std::cerr << "Unable to modify .assets file\n";
    return 1;
  }
}
