#pragma once
#include "cmobj.hh"
#include "node.hh"
#include <iostream>
#include <map>
#include <cmath>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <limits.h>
#include <filesystem>

typedef CmObj* (*bifuncs)(Args);

CmObj* print(Args);
CmObj* input(Args);
CmObj* inputNum(Args);
CmObj* type(Args);
CmObj* len(Args);
CmObj* mod(Args);
CmObj* abs(Args);
CmObj* getPwd(Args);

CmObj* add(Args);
CmObj* min(Args);
CmObj* mul(Args);
CmObj* div(Args);
CmObj* pot(Args);

CmObj* eq(Args);
CmObj* gt(Args);
CmObj* lt(Args);
CmObj* neq(Args);
CmObj* gteq(Args);
CmObj* lteq(Args);

CmObj* andop(Args);
CmObj* orop(Args);


CmObj* cmExit(Args args);

CmObj* to_string(CmObj* obj);
CmObj* to_string(Args args);
CmObj* to_bool(CmObj* obj);
CmObj* to_bool(Args args);
CmObj* to_int(Args args);
CmObj* to_float(CmObj* obj);
CmObj* to_float(Args args);

CmObj* is_number(Args args);

const std::map<std::string, bifuncs> builtInFunctions = {
    {"print", print},
    {"input", input},
    {"inpnum", inputNum},

    {"is_number", is_number},

    {"str", to_string},
    {"bool", to_bool},
    {"int", to_int},
    {"float", to_float},

    {"add", add},
    {"min", min},
    {"mul", mul},
    {"div", div},
    {"pot", pot},

    {"type", type},
    {"len", len},
    {"mod", mod},
    {"abs", abs},
    {"getcwd", getPwd},

    {"exit", cmExit}
};
