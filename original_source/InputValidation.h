#ifndef FLOATAPP_INPUT_VALIDATION_H
#define FLOATAPP_INPUT_VALIDATION_H
#include "Records.h"
#include <cstddef>
#include <iosfwd>
#include <string>
namespace floatapp {
bool parseMoney(const std::string& text, long long& out);
bool parseInteger(const std::string& text, int& out, int minimum, int maximum);
bool isValidDate(const std::string& text);
bool isValidText(const std::string& text, std::size_t maxLength=200);
bool validateData(const AppData& data, std::string& error);
bool nextRecordId(const AppData& data, int& out, std::string& error);
bool readMenuChoice(std::istream& in, std::ostream& out, int minimum, int maximum, int& value);
bool readMoney(std::istream& in, std::ostream& out, long long& value, bool allowZero=false);
bool readDate(std::istream& in, std::ostream& out, std::string& value);
bool readRequiredText(std::istream& in, std::ostream& out, std::string& value);
}
#endif
