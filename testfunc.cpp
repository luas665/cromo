#include <iostream>
#include <string>

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


int main(int argc, char **argv) {
    std::cout << doubleToString(10.0) << std::endl;
}