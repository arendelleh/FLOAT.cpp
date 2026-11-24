#ifndef FLOATAPP_FILE_STORAGE_H
#define FLOATAPP_FILE_STORAGE_H
#include "Records.h"
#include <string>
namespace floatapp {
enum class LoadStatus { Loaded, NotFound, Error };
LoadStatus loadData(const std::string& path, AppData& data, std::string& error);
bool saveData(const std::string& path, const AppData& data, std::string& error);
}
#endif
