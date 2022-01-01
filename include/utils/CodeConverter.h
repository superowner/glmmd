#ifndef CODE_CONVERTER_H_
#define CODE_CONVERTER_H_

#include <string>

enum class EncodingType
{
    UTF8,
    UTF16_LE,
};

extern std::string convertToNativeEncoding(const std::string &src, EncodingType ty);

#endif
