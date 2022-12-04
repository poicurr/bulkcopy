#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "StringUtils.hpp"

std::string src;
std::string dest;

void createConfigFile() {
  std::stringstream ss;
  ss << "src=./"
     << "\r\n";
  ss << "dest=~/Desktop"
     << "\r\n";
  std::ofstream ofs("./bulkcopy.conf");
  ofs << ss.str();
}

std::string unixpath(const std::string& s) { return replace(s, "\\", "/"); }

std::vector<std::string> loadfile(std::ifstream& ifs) {
  // read conf
  auto s = std::string{};
  char buffer[512];
  while (!ifs.eof()) {
    ifs.read(buffer, 512);
    s.append(buffer, ifs.gcount());
  }
  // parse conf
  auto lines = split(s, "\r\n");
  if (lines.size() == 1) {
    lines = split(s, "\n");
  }
  return lines;
}

int main(int argc, char* argv[]) {
  // target file data
  if (argc != 2) {
    std::cerr << "[usage] ./bulkcopy targets.txt" << std::endl;
    return 1;
  }

  // --------------------------------
  // load bulkcopy.conf
  // --------------------------------
  std::ifstream ifs("./bulkcopy.conf");
  if (!ifs.is_open()) {
    createConfigFile();
    ifs.open("./bulkcopy.conf");
  }
  auto conf = loadfile(ifs);
  auto map = std::unordered_map<std::string, std::string>{};
  for (auto line : conf) {
    if (line.empty()) continue;
    auto kv = split(line, "=");
    if (kv.size() == 2) map[kv[0]] = kv[1];
  }
  auto it = map.find("src");
  if (it != map.end()) src = it->second;
  it = map.find("dest");
  if (it != map.end()) dest = it->second;

  // --------------------------------
  // load targets.txt
  // --------------------------------
  std::cout << "--- loading target file ---" << std::endl;
  std::cout << src << "targets.txt" << std::endl;
  auto ifs2 = std::ifstream{src + "/targets.txt"};
  auto targets = loadfile(ifs2);

  char datetime[256];
  std::time_t t = std::time(NULL);
  std::strftime(datetime, sizeof(datetime), "%Y%m%d_%H%M", std::localtime(&t));
  auto outpath = dest + "/" + datetime;

  // --------------------------------
  // copy file
  // --------------------------------
  for (auto target : targets) {
    if (target.empty()) continue;
    auto from = std::filesystem::path{unixpath(src + target)};
    auto to = std::filesystem::path{unixpath(outpath + target)};
    std::cout << "---------------------------" << std::endl;
    std::cout << "from       : " << from << std::endl;
    std::cout << "to         : " << to << std::endl;
    std::cout << "---------------------------" << std::endl;
    if (!std::filesystem::exists(to.parent_path())) {
      std::filesystem::create_directories(to.parent_path());
    }
    std::filesystem::copy(from, to,
                          std::filesystem::copy_options::overwrite_existing);
  }
}
