#include "builtincmfuncs.hh"

namespace std {
    inline string to_string(bool __val) {
        if (__val) {
            return "true";
        }
        return "false";
    }
};

std::string removeChars(std::string __str, char chr) {
    std::string result = "";
    for (char c : __str) {
        if (c != chr) {
            result += c;
        }
    }
    return result;
}

bool isDigit(std::string __str) {
    for (char c : __str) {
        if (!std::isdigit(c)) {
            return false;
        } 
    }
    return true;
}

inline bool is_cmnum(CmObj* obj) {
    return obj->type == "int" || obj->type == "float";
}

inline bool is_cmnum(CmObj* obj, CmObj* other) {
    return is_cmnum(obj) && is_cmnum(other);
}

double cCmNum(CmObj* obj) {
    if (obj->type == "int") {
        return *(int*)obj->data;
    }
    return *(double*)obj->data;
}

void cprint(Args args) {
    if (args.size() > 0) {
        std::cout << *(std::string *)to_string(args[0])->data;
        for (int i = 1; i < args.size(); i++) {
            std::cout << " " << *(std::string *)to_string(args[i])->data;
        }
    }
}

void err(std::string _message) {
    std::cerr << _message << std::endl;
}

CmObj* abs(Args args) {
    if (args.size() != 1 && !is_cmnum(args[0])) {
        err("Error: Invalid arguments.");
        return new CmObj();
    }
    
    double num = cCmNum(args[0]);
    if (num < 0) {
        return new CmObj(Float(num*-1));
    }
    return new CmObj(Float(num));
}

CmObj* getPwd(Args args) {
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        return new CmObj(String(buffer));
    }
    return new CmObj();
}

CmObj* is_number(Args args) {
    if(args[0]->type == "str") {
        std::string str = *(std::string*)args[0]->data;
        int points = std::count(str.begin(), str.end(), '.');
        std::string strWOPoints = removeChars(str, '.');

        if (strWOPoints[0] == '-') {
            strWOPoints = strWOPoints.substr(1);
        }

        if(points <= 1 && isDigit(strWOPoints)) {
            return new CmObj(Boolean(true));
        }
        return new CmObj(Boolean(false));
    }
    
    return new CmObj(Boolean(is_cmnum(args[0])));
}

CmObj* cmExit(Args args) {
    int exitCode;

    if (args.size() > 0) {
        CmObj* c = args[0];

        if (c->type == "int" || c->type == "float") {
            exitCode = *(int*)c->data;
        } else {
            exitCode = 1;
        }
    } else {
        exitCode = 0;
    }


    exit(exitCode);
}

CmObj* print(Args args) {
    cprint(args);
    std::cout << std::endl;
    return new CmObj();
}

CmObj* input(Args args) {
    cprint(args);
    std::string inputString;
    getline(std::cin, inputString);

    return new CmObj(String(inputString));
}

CmObj* inputNum(Args args) {
    if (args.size() < 2) {
        err("Error: InputNum must be at least 2 arguments.");
        return new CmObj();
    }
    CmObj* result = input(Args(args.begin(), args.begin()+args.size()-1));

    while ((*(std::string*)result->data) == "" || !(*(bool*)is_number({result})->data) && *(bool*)gt(Args{len(Args{result}), new CmObj(Integer(0))})->data) {
        print(Args{args[args.size()-1]});
        result = input(Args(args.begin(), args.begin()+args.size()-1));
    }
    return to_float(result);
}

CmObj* type(Args args) {
    return new CmObj(String(args[0]->type));
}

std::string doubleToString(double n) {
    std::string r = std::to_string(n);
    bool stop = false;

    std::string result = "";

    for (int i = r.length() - 1; i >= 0; i--) {
        if (!stop && r[i] == '.'){
            i--;
            stop = true;
        } else if (!(r[i] == '0' && !stop)) {
            stop = true;
        }

        if (stop) {
            result.insert(result.begin(), 1, r[i]);
        }
    }

    return result;
}

CmObj* to_string(CmObj* obj) {
    if (obj->type == "str") {
        return new CmObj(String(*(std::string*)obj->data));
    } else if (obj->type == "float") {
        return new CmObj(String(doubleToString(*(double*)obj->data)));
    } else if (obj->type == "int") {
        return new CmObj(String(std::to_string(*(int*)obj->data)));
    } else if (obj->type == "bool") {
        return new CmObj(String(std::to_string(*(bool*)obj->data)));
    } else if (obj->type == "bifunc") {
        return new CmObj(String("<built-in function " + *(std::string*)obj->data + ">"));
    } else if (obj->type == "func") {
        return new CmObj(String("<function>"));
    } else if (obj->type == "list") {
        Args elems = *(Args*)obj->data;
        std::string str = "[";
        if (elems.size() > 0) {
            str += *(std::string*)to_string(elems[0])->data;
            for (int i = 1; i < elems.size(); i++) {
                str += ", " + *(std::string*)to_string(elems[i])->data;
            }
        }
        str += "]";

        return new CmObj(String(str));
    } else if (obj->type == "err") {
        return new CmObj(String(*(std::string*)obj->data));
    } else {
        return new CmObj("No conversion type " + obj->type + " to string");
    }
}

CmObj* to_string(Args args) {
    return to_string(args[0]);
}

CmObj* to_bool(CmObj* obj) {
    if (obj->type == "str") {
        return new CmObj(Boolean((*(std::string*)obj->data).length() > 0));
    } else if (obj->type == "float") {
        return new CmObj(Boolean((*(double*)obj->data) != 0));
    } else if (obj->type == "int") {
        return new CmObj(Boolean((*(int*)obj->data) != 0));
    } else if (obj->type == "bool") {
        return new CmObj(Boolean(*(bool*)obj->data));
    } else if (obj->type == "list") {
        Args elems = *(Args*)obj->data;
        return new CmObj(Boolean(elems.size() > 0));
    } else if(obj->type == "None") {
        return new CmObj(Boolean(false));
    } else {
        return new CmObj("No conversion type " + obj->type + " to string");
    }
}

CmObj* to_bool(Args args) {
    return to_bool(args[0]);
}

CmObj* to_int(CmObj* obj) {
    if (obj->type == "str") {
        if (*(bool*)is_number(Args{obj})->data) {
            std::string value = *(std::string*)obj->data;
            double intValue = std::stod(value);
            return new CmObj(Integer(intValue));
        }        
    } else if (obj->type == "float") {
        return new CmObj(Integer(*(double*)obj->data));
    } else if (obj->type == "int") {
        return new CmObj(Integer(*(int*)obj->data));
    } else if (obj->type == "bool") {
        return new CmObj(Integer(*(bool*)obj->data));
    }
    return new CmObj("No conversion type " + obj->type + " to string");
}
CmObj* to_int(Args args) {
    return to_int(args[0]);
}

CmObj* to_float(CmObj* obj) {
    if (obj->type == "str") {
        if (*(bool*)is_number(Args{obj})->data) {
            std::string value = *(std::string*)obj->data;
            double intValue = std::stod(value);
            return new CmObj(Float(intValue));
        }        
    } else if (obj->type == "float") {
        return new CmObj(Float(*(double*)obj->data));
    } else if (obj->type == "int") {
        return new CmObj(Float(*(int*)obj->data));
    } else if (obj->type == "bool") {
        return new CmObj(Float(*(bool*)obj->data));
    }
    return new CmObj("No conversion type " + obj->type + " to string");
}
CmObj* to_float(Args args) {
    return to_float(args[0]);
}



CmObj* mod(Args args) {
    if (args.size() == 2) {
        CmObj* left = args[0];
        CmObj* right = args[1];

        if (is_cmnum(left) && is_cmnum(right)) {
            double result =  std::fmod(cCmNum(left), cCmNum(right));
            return new CmObj(Float(result));
        }

        err("Error: Invalid type of arguments expected int or float types.");

    } else {
        err("Error: Invalid number of arguments in \"mod\" function: expected 2 arguments.");
    } 
    return new CmObj();
}

CmObj* len(Args args) {
    if (args[0]->type == "str") {
        std::string str = *(std::string*)args[0]->data;
        return new CmObj(Integer(str.length()));
    } else if (args[0]->type == "list") {
        std::vector<CmObj*> list = *(std::vector<CmObj*>*)args[0]->data;
        return new CmObj(Integer(list.size()));
    }
    return new CmObj(Integer(0));

}

CmObj* add(Args args) {
    CmObj* left = args[0];
    CmObj* right = args[1];
    if (is_cmnum(left, right)) {
        return new CmObj(Float(cCmNum(left) + cCmNum(right)));
    } else if (left->type == "str" && right->type == "str") {
        return new CmObj(String(*(std::string*)left->data + *(std::string*)right->data ));
    } else if (left->type == "list" && right->type == "list") {
        Args l = *(Args*)left->data;
        Args r = *(Args*)right->data;
        l.insert(l.end(), r.begin(), r.end());
        return new CmObj(l);
    }
    err("Error: Unexpect type of arguments.");
    return new CmObj();
}

CmObj* min(Args args) {
    CmObj* left = args[0];
    CmObj* right = args[1];
    if (is_cmnum(left, right)) {
        return new CmObj(Float(cCmNum(left) - cCmNum(right)));
    }
    err("Error: Unexpect type of arguments.");
    return new CmObj();
}

CmObj* mul(Args args) {
    CmObj* left = args[0];
    CmObj* right = args[1];
    if (is_cmnum(left, right)) {
        return new CmObj(Float(cCmNum(left) * cCmNum(right)));
    }
    err("Error: Unexpect type of arguments.");
    return new CmObj();
}

CmObj* div(Args args) {
    CmObj* left = args[0];
    CmObj* right = args[1];
    if (is_cmnum(left, right)) {
        return new CmObj(Float(cCmNum(left) / cCmNum(right)));
    }
    err("Error: Unexpect type of arguments.");
    return new CmObj();
}

CmObj* pot(Args args) {
    CmObj* left = args[0];
    CmObj* right = args[1];
    if (is_cmnum(left, right)) {
        return new CmObj(Float(std::pow(cCmNum(left), cCmNum(right))));
    }
    err("Error: Unexpect type of arguments.");
    return new CmObj();
}


CmObj* eq(Args args) { 
    CmObj* left = args[0];
    CmObj* right = args[1];

    if (left->type == right->type) {
        if (left->type == "str") {
            return new CmObj(Boolean( (*(std::string*)left->data) == (*(std::string*)right->data) ));
        } else if (left->type == "bool") {
            return new CmObj(Boolean((*(bool*)left->data) == (*(bool*)right->data)));
        }
    } else if (is_cmnum(left, right)) {
        return new CmObj(Boolean(cCmNum(left) == cCmNum(right)));
    } 
    return new CmObj(Boolean(false));
}

CmObj* gt(Args args) {
    CmObj* left = args[0];
    CmObj* right = args[1];
    if (is_cmnum(left, right)) {
        return new CmObj(Boolean(cCmNum(left) > cCmNum(right)));
    }
    return new CmObj(Boolean(false));
}

CmObj* lt(Args args) {
    CmObj* left = args[0];
    CmObj* right = args[1];
    if (is_cmnum(left, right)) {
        return new CmObj(Boolean(cCmNum(left) < cCmNum(right)));
    }
    return new CmObj(Boolean(false));
}

CmObj* neq(Args args) {
    bool iseq = *(bool*)eq(args)->data;
    
    return new CmObj(Boolean(!iseq));
}

CmObj* gteq(Args args) {
    CmObj* left = args[0];
    CmObj* right = args[1];
    if (is_cmnum(left, right)) {
        return new CmObj(Boolean(cCmNum(left) >= cCmNum(right)));
    }
    return new CmObj(Boolean(false));
}

CmObj* lteq(Args args) {
    CmObj* left = args[0];
    CmObj* right = args[1];
    if (is_cmnum(left, right)) {
        return new CmObj(Boolean(cCmNum(left) <= cCmNum(right)));
    }
    return new CmObj(Boolean(false));
}


CmObj* andop(Args args) {
    bool left = *(bool*)args[0]->data;
    bool right = *(bool*)args[1]->data;

    return new CmObj(Boolean(left && right));
}

CmObj* orop(Args args) {
    bool left = *(bool*)args[0]->data;
    bool right = *(bool*)args[1]->data;

    return new CmObj(Boolean(left || right));
}

