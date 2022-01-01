#include <utils/CodeConverter.h>
#include <utils/OsInfo.h>

#include <sstream>
#include <codecvt>
#include <locale>

#ifdef OS_WINDOWS
#include <Windows.h>
std::string UTF8toANSI(const std::string &src)
{
    auto len = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, NULL, 0);
    if (len <= 0)
        return std::string();
    WCHAR *wBuffer = new WCHAR[len + 1];
    MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, wBuffer, len);
    wBuffer[len] = 0;

    auto acp = GetACP();
    len = WideCharToMultiByte(acp, 0, wBuffer, -1, NULL, 0, NULL, NULL);
    if (len <= 0)
    {
        delete[] wBuffer;
        return std::string();
    }

    CHAR *buffer = new CHAR[len + 1];
    WideCharToMultiByte(acp, 0, wBuffer, -1, buffer, len, NULL, NULL);
    buffer[len] = 0;
    delete[] wBuffer;
    std::string result(buffer);
    delete[] buffer;
    return result;
}
#endif

std::string convertToNativeEncoding(const std::string &src, EncodingType ty)
{
    if (ty == EncodingType::UTF8)
    {
#ifdef OS_WINDOWS
        return UTF8toANSI(src);
#elif defined(OS_LINUX)
        return src;
#endif
    }
    else if (ty == EncodingType::UTF16_LE)
    {
        std::wstring wsrc;
        for (size_t i = 0; i < src.size(); i += 2)
        {
            char b[4]{src[i], src[i + 1], '\0', '\0'};
            wsrc.push_back(*(wchar_t *)(b));
        }

        std::wistringstream wiss(wsrc);
        wiss.imbue(std::locale(
            wiss.getloc(),
            new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

        std::string src_u8 =
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}
                .to_bytes(wiss.str());
        return convertToNativeEncoding(src_u8, EncodingType::UTF8);
    }
    return "";
}