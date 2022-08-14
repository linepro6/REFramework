#pragma once
#include <unordered_map>
#include "FnvHash.hpp"

namespace yi18n {

struct YMOData;

void LoadTranslateData(const YMOData* ymo);

#ifdef _INC_WINDOWS
void LoadTranslateDataFromResource(const HINSTANCE hInst);
#endif

const char* TranslateWithHash(const char* const str, const uint32_t hash);
} // namespace yi18n

#define _(str) yi18n::TranslateWithHash(str, std::integral_constant<uint32_t, fnv1a_32(str)>::value)
#define C_(ctxt, str) yi18n::TranslateWithHash(str, std::integral_constant<uint32_t, fnv1a_32(ctxt L"\004" str)>::value)
