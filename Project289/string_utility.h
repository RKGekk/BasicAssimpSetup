#pragma once

#include <string>
#include <locale>
#include <codecvt>

std::string w2s(const std::wstring& var);
std::wstring s2w(const std::string& var);