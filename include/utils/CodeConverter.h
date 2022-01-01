#ifndef CODE_CONVERTER_H_
#define CODE_CONVERTER_H_

#include <string>

enum class EncodingType
{
    UTF8,
    UTF16_LE,
};

extern std::string convertToNativeEncoding(const std::string &src, EncodingType ty);
extern std::string UTF16_LE_to_UTF8(const std::string &src);

#endif
