#include <iostream>
#include <string>
#include <cmath>
#include <Windows.h>
#include <vector>
#include"commons.h"
#include<thread>

using namespace std;


int main() {
    cout << "\x1b?25l";
    Game g("settings.json");
    Field f(g);
    f.Display();
    f.StartCatch();

    while (true) {
        f.GenerateNext();
        
        this_thread::sleep_for(chrono::milliseconds(1000 / g.fps));
    }

    return 0;
}