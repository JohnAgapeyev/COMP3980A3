#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <array>


std::string selectTagMessageFactory();
std::string readTagMessageFactory();

void parseSelectResponse(const std::string& resp);
void parseReadResponse(const std::string& resp);

std::string selectTagMessageFactory() {
    std::string message;
    message.push_back('\r');
    message.append("0001");  //Loop_f flag
    message.append("0101"); //Command
    message.append("0000"); //Auto tag detect
    message.push_back('\r');
    return message;
}

std::string readTagMessageFactory() {
    std::string message;
    message.push_back('\r');
    message.append("0004"); //RF_f flag
    message.append("0102"); //Command
    message.append("0000"); //Auto tag detect
    message.append("0000"); //Start reading at address 0x0000
    message.append("0001"); // Read one block
    message.push_back('\r');
    return message;
}

void parseSelectResponse(const std::string& resp) {
    //All responses start with LF and end with CRLF
}

void parseReadResponse(const std::string& resp) {
    //All responses start with LF and end with CRLF
}