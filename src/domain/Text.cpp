#include "domain/Text.hpp"

#include <algorithm>
#include <unordered_map>

namespace text {
namespace {

std::u32string decode(const std::string& s) {
    std::u32string out;
    size_t i = 0;
    while (i < s.size()) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        char32_t cp;
        int n;
        if (c < 0x80)            { cp = c;        n = 1; }
        else if ((c >> 5) == 6)  { cp = c & 0x1F; n = 2; }
        else if ((c >> 4) == 14) { cp = c & 0x0F; n = 3; }
        else if ((c >> 3) == 30) { cp = c & 0x07; n = 4; }
        else { ++i; continue; }
        for (int k = 1; k < n && i + k < s.size(); ++k)
            cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3F);
        out.push_back(cp);
        i += n;
    }
    return out;
}

std::string encode(const std::u32string& u) {
    std::string out;
    for (char32_t cp : u) {
        if (cp < 0x80) out.push_back(static_cast<char>(cp));
        else if (cp < 0x800) {
            out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp < 0x10000) {
            out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else {
            out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }
    return out;
}

char32_t lower(char32_t c) {
    if (c >= U'A' && c <= U'Z') return c + 32;
    if (c >= 0x410 && c <= 0x42F) return c + 32;  // А-Я -> а-я
    if (c == 0x401) return 0x451;                 // Ё -> ё
    return c;
}

bool isCyrVowelOrSign(char32_t c) {
    static const std::u32string v = U"аеёиоуыэюяьй";
    return v.find(c) != std::u32string::npos;
}

const std::unordered_map<std::string, std::string>& synonyms() {
    static const std::unordered_map<std::string, std::string> m = {
        {"картошка", "картофель"}, {"картошку", "картофель"}, {"картошки", "картофель"},
        {"картофеля", "картофель"}, {"томаты", "помидоры"}, {"томат", "помидор"},
        {"курятина", "курица"}, {"куриное мясо", "курица"}, {"куриная грудка", "курица"},
    };
    return m;
}

}  // namespace

std::string normalize(const std::string& raw) {
    // 1. регистр, ё -> е, схлопываем пробелы
    std::u32string src = decode(raw), cleaned;
    bool pendingSpace = false;
    for (char32_t c : src) {
        c = lower(c);
        if (c == 0x451) c = 0x435;
        if (c <= 0x20) { pendingSpace = !cleaned.empty(); continue; }
        if (pendingSpace) cleaned.push_back(U' ');
        pendingSpace = false;
        cleaned.push_back(c);
    }
    // 2. синонимы
    std::string s = encode(cleaned);
    auto it = synonyms().find(s);
    if (it != synonyms().end()) cleaned = decode(it->second);
    // 3. отбрасываем одно окончание у длинных слов
    if (cleaned.size() >= 4 && isCyrVowelOrSign(cleaned.back())) cleaned.pop_back();
    return encode(cleaned);
}

}  // namespace text
