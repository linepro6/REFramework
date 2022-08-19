#include "I18n.hpp"
#include "FnvHash.hpp"
#include <unordered_map>
#include "I18n/i18n_ymo_zh_CN.hpp"

namespace yi18n {
std::unordered_map<uint32_t, const char*> hashToStrMap;
static char* g_buffer = nullptr;

#pragma pack(push, 1)
struct YMOData {
    uint16_t len;
    struct {
        uint32_t hash;
        uint16_t offset;
    } table[1];
};
#pragma pack(pop)

void LoadTranslateData(const YMOData* ymo) {
    hashToStrMap.reserve(ymo->len);
    for (uint16_t i = 0; i < ymo->len; ++i) {
        const auto hash = ymo->table[i].hash;
        const auto offset = ymo->table[i].offset;
        const auto str = reinterpret_cast<const char*>(reinterpret_cast<const uint8_t*>(ymo) + offset);
        hashToStrMap.emplace(hash, str);
    }
}

const char* TranslateWithHash(const char* const str, const uint32_t hash) {
    auto it = hashToStrMap.find(hash);
    if (it != hashToStrMap.end())
        return it->second;
    return str;
}

uint32_t fnv1a_32(const char* str, uint32_t hval = FNV1_32_INIT) {
    auto len = strlen(str);
    for (size_t i = 0; i < len; ++i) {
        for (size_t j = 0; j < sizeof(char); ++j) {
            hval ^= ((str[i] >> (j * CHAR_BIT)) & 0xff); // little endian
            hval *= FNV_32_PRIME;
        }
    }
    return hval;
}

const char* Translate(const char* const str) {
    const auto hash = fnv1a_32(str);
    auto result = TranslateWithHash(str, hash);
    if (strcmp(str, result) == 0) {
        std::string source{str};
        auto index = source.find_first_of(':');
        if (index != std::string::npos && index > 0) {
            auto title = source.substr(0, index);
            std::string target = Translate(title.c_str());
            if (title != target) {
                auto trans = (target + ": " + source.substr(index + 1));
                if (g_buffer) {
                    delete[] g_buffer;
                }
                g_buffer = new char[trans.size() + 1];
                strcpy(g_buffer, trans.c_str());
                return g_buffer;
            }
        }
    }
    return result;
}

void Init() {
    LoadTranslateData(reinterpret_cast<const YMOData*>(i18n_ymo_zh_CN));
}

} // namespace yi18n
