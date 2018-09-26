//
// Created by 吴先 on 2018/9/26.
//

#ifndef WORD_SEGMENTATION_UNICODE_H
#define WORD_SEGMENTATION_UNICODE_H
#include <iostream>
#include <string>
#include <codecvt>
#include "Limonp/Logger.hpp"

//#define WINDOWS

typedef std::u16string unicode_t;

#ifdef WINDOWS
#include <windows.h>
const char DICT_FILEPATH[] = "jieba.gbk";
BOOL MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize) {
    DWORD dwMinSize;
    dwMinSize = MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, NULL, 0);
    if (dwSize < dwMinSize) {
        return FALSE;
    }
    MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize);
    return TRUE;
}
BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize) {
    DWORD dwMinSize;
    dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
    if (dwSize < dwMinSize) {
        return FALSE;
    }
    WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwSize, NULL, FALSE);
    return TRUE;
}
class Converter {
public:
    unicode_t from_bytes(const std::string& s) {
        wchar_t *ws = new wchar_t[s.length() + 1];
        if (!MByteToWChar(s.c_str(), ws, s.length() + 1)) {
            LogFatal("converting %s to unicode failed.", s.c_str());
        }
        unicode_t ret((unicode_t::value_type*)ws);
        delete[] ws;
        return ret;
    }
    std::string to_bytes(const unicode_t& unicode) {
        char *s = new char[2*(unicode.length()) + 1];
        if (!WCharToMByte((wchar_t*)unicode.c_str(), s, 2 * (unicode.length()) + 1)) {
            LogFatal("converting to unicode error.");
        }
        std::string ret(s);
        delete[] s;
        return ret;
    }
};
Converter converter;
#else
template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};
const char DICT_FILEPATH[] = "jieba.utf8";
std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> converter;
#endif

unicode_t P_set = u"，。、；‘【】《》？：“”{}！@#￥%…&*（）-=—+,./;'[]\\<>?:\"{} | !@#$%^&*() -= _ +『』 ⊙";

namespace std {
    std::ostream &operator<<(std::ostream &os, const unicode_t &unicode) {
        os << "u\"" + converter.to_bytes(unicode) + "\"";
        return os;
    }

    std::ofstream &operator<<(std::ofstream &ofs, const unicode_t &unicode) {
        ofs << converter.to_bytes(unicode);
        return ofs;
    }
}

template<typename TDICT, typename TKEY, typename TVALUE>
TVALUE get(TDICT &dict, TKEY key, TVALUE value) {
    if (dict.find(key) != dict.end()) {
        value = dict[key];
    }
    return value;
}

bool is_english(char16_t c) {
    return (c >= u'a' && c <= u'z') || (c >= u'A' && c <= u'Z');
}

bool is_P(unicode_t unicode) {
    for (auto c : unicode) {
        if (P_set.find(c) == P_set.npos)
            return false;
    }
    return true;
}
bool is_P(std::string& s) {
    return is_P(converter.from_bytes(s));
}
#endif //WORD_SEGMENTATION_UNICODE_H
