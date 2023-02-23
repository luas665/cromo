#pragma once
#include <string>
#include <vector>
#include "node.hh"

struct CmObj {
    std::string type;
    void* data;

    CmObj(String* var): type("str"), data(new std::string(var->value)) {}
    CmObj(Float* var): type("float"), data(new double(var->value)) {}
    CmObj(Integer* var): type("int"), data(new int(var->value)) {}
    CmObj(Boolean* var): type("bool"), data(new bool(var->value)) {}
    CmObj(std::vector<CmObj*> var): type("list"), data(new std::vector<CmObj*>(var)) {}

    CmObj(String var): type("str"), data(new std::string(var.value)) {}
    CmObj(Float var): type("float"), data(new double(var.value)) {}
    CmObj(Integer var): type("int"), data(new int(var.value)) {}
    CmObj(Boolean var): type("bool"), data(new bool(var.value)) {}
    CmObj(Function var): type("func"), data(new Function(var.params, var.block)) {}

    CmObj(std::string type, void* data): type(type), data(data) {}

    CmObj(): type("None"), data(nullptr) {}
    
    CmObj(const char* err): type("err"), data(new std::string(err)) {}
    CmObj(const std::string err): type("err"), data(new std::string(err)) {}

};

typedef CmObj* cfunc(std::vector<CmObj*>);


typedef std::vector<CmObj*> Args;

