#ifndef CODE_CONVERTER_H_
#define CODE_CONVERTER_H_

#include <string>

enum class EncodingType
{
    UTF8,
    UTF16_LE,
};

extern std::string convertToNativeEncoding(const std::string &, EncodingType);
extern std::string UTF16_LE_to_UTF8(const std::string &);
extern std::string shiftJIS_to_UTF8(const std::string &);

#endif
