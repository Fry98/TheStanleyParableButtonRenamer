#include <iostream>
#include <fstream>
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
    std::ifstream lu_f("C:\\Program Files (x86)\\Steam\\config\\loginusers.vdf");
    auto root = tyti::vdf::read(lu_f);
    if (root.name != "users") throw 0;

    auto it = root.childs.begin();
    if (it == root.childs.end()) throw 0;

    auto& user_data = *it->second;
    user_name = user_data.attribs["PersonaName"];
    if (user_name.length() < 1) throw 0;
    lu_f.close();
  } catch (...) {
    std::cerr << "Unable to read Steam profile name\n";
    return 1;
  }

  try {
    // Create temp file with name
    std::ofstream name_f("TSPR_name.tmp");
    name_f << user_name;
    name_f.close();

    // Synthesize voice line
    system("voice.exe -m -k TSPR_name.tmp -o 0000.wav > NUL");
    std::remove("TSPR_name.tmp");

    // Convert to FSB5
    if (!std::filesystem::exists("0000.wav")) throw 0;
    system("fsbankcl.exe -format vorbis -o TSPR_bank.fsb 0000.wav > NUL");

    if (!std::filesystem::exists("TSPR_bank.fsb")) throw 0;
    std::remove("0000.wav");
  } catch (...) {
    std::cerr << "Unable to synthesize voice\n";
    return 1;
  }
}
