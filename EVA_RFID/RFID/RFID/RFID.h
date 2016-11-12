#pragma once

#include <string>

std::string selectTagMessageFactory();
std::string readTagMessageFactory();

void parseSelectResponse(const std::string& resp);
void parseReadResponse(const std::string& resp);