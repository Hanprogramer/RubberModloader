#pragma once
#include "pch.h"
#include <string>
bool replace(std::string& str, const std::string& from, const std::string& to);
void replaceAll(std::string& str, const std::string& from, const std::string& to);
bool hasEnding(std::string const& fullString, std::string const& ending);