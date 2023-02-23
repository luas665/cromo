#pragma once
#include <string>
#include <algorithm>

class Token
{
public:
    std::string tpe;
    std::string text;
    int startPos;
    Token(std::string type, std::string txt);
    Token(){}
    std::string to_string();
};






