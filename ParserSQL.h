#pragma once

#include "Options.h"
#include "ParserSQL.h"

#include<variant>
#include<iostream>

bool Parse(std::string request, MyCoolBD& bd);

std::string ReadWordFromBegin(std::string& s, bool only_one_word);

std::variant<int, bool, float, double, std::string> GetValue(const std::string& str, const std::string& type);