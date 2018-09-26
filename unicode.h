//
// Created by 吴先 on 2018/9/26.
//

#ifndef WORD_SEGMENTATION_UNICODE_H
#define WORD_SEGMENTATION_UNICODE_H
#include <iostream>
#include <string>
#include <codecvt>

//#define GBK

typedef std::u16string unicode_t;

template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};
#ifdef GBK
const char DICT_FILEPATH[] = "jieba.gbk";
const char* GBK_LOCALE_NAME = ".936"; //GBK在windows下的locale name
typedef deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>> gbfacet_t;
std::wstring_convert<gbfacet_t, char16_t> converter(new gbfacet_t(GBK_LOCALE_NAME));
#else
const char DICT_FILEPATH[] = "jieba.utf8";
std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> converter;
#endif

unicode_t P_set = u"，。、；‘【】《》？：“”{}！@#￥%…&*（）-=—+,./;'[]\\<>?:\"{} | !@#$%^&*() -= _ +『』 ⊙";

namespace std {
    std::ostream &operator<<(std::ostream &os, const unicode_t &unicode) {
        os << "u\"" + converter.to_bytes(unicode.c_str()) + "\"";
        return os;
    }

    std::ofstream &operator<<(std::ofstream &ofs, const unicode_t &unicode) {
        ofs << converter.to_bytes(unicode.c_str());
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
