#include <iostream>
#include <pthread.h>
#include <ncurses.h>
#include <string>
using namespace std;

int main() {
    bool exit = false;
    string key;
    while (!exit) {
        std::cout << "Welcome to musiC++!" << std::endl;
        std::cout << "press q to quit" << std::endl;
        std::cin >> key;
        if (key == "q") {
            exit = true;
        }
    }
    return 0;
}
