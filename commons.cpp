#include"commons.h"
#include<iomanip>
#include <windows.h>
#include<thread>

#pragma warning(disable:4996)


void Field::setCursorPosition(int x, int y) {
    std::cout << "\x1b[" << x + 1 << ";" << y * 2 + 1 << "H"; // ANSI код для перемещения курсора
}


void Field::drawCell(int x, int y, int type, int color) {
    setCursorPosition(x, y);
    if (type == 0) {
        cout << "\x1b[48;5;" << color << "m  \x1b[0m"; // green for alive
    }
    else if (type == 1) {
        cout << "\x1b[48;5;244m  \x1b[48;5;244m"; // for cursor
    }
    else {
        cout << "\x1b[47m  \x1b[0m"; // for dead
    }
}


Game::Game(const char* filename) {
    FILE* fp = fopen(filename, "r");

    char readBuffer[65536];
    rapidjson::FileReadStream stream(fp, readBuffer,
        sizeof(readBuffer));
    // Parse the JSON data  
    rapidjson::Document d;
    d.ParseStream(stream);

    // Close the file 
    fclose(fp);

    width = d["width"].GetInt();
    height = d["height"].GetInt();
    quantity_to_birth = d["quantity_to_birth"].GetInt();
    lcount_neighbour = d["lcount_neighbour"].GetInt();
    ucount_neighbour = d["ucount_neighbour"].GetInt();
    fps = d["fps"].GetInt();
    color = d["color"].GetInt();
}

Field::Field(Game& rules): rules(rules){
    if (rules.width == 0 || rules.height == 0) {
        throw "Empty field";
    }
    current = new vector<vector<Cell>>(rules.height, std::vector<Cell>(rules.width, Cell(0)));
    next = new vector<vector<Cell>>(rules.height, std::vector<Cell>(rules.width, Cell(0)));
    status = 0;
    for (int i = 0; i < rules.height; i++) {
        for (int j = 0; j < rules.width; j++) {
            (*current)[i][j].i = i;
            (*current)[i][j].j = j;
            (*next)[i][j].i = i;
            (*next)[i][j].j = j;
        }
    }
    cursor = { 0, 0 };
}

void Field::StartCatch() {
    thread t(&Field::ButtonsCatch, this);
    t.detach();
}

void Field::Display() {
    for (int i = 0; i < (*current).size(); i++) {
        for (int j = 0; j < (*current)[0].size(); j++) {
                if ((*current)[i][j].alive()) {
                    drawCell(i, j, 0, rules.color);
                }
                else {
                    drawCell(i, j, 2, rules.color);
                }
                if (cursor.first == i && cursor.second == j && !status) {
                    drawCell(i, j, 1, rules.color);
                }
            }
    }
}

void Field::GenerateNext() {
    if (!status)
        return;
    for (int i = 0; i < rules.height; i++) {
        for (int j = 0; j < rules.width; j++) {
            (*next)[i][j].set_life((*current)[i][j].Live(*this, rules));
        }
    }
    swap(next, current);
    Display();
}

bool Field::Get(int i, int j) {
    i = (i + (*current).size()) % (*current).size();
    j = (j + (*current)[0].size()) % (*current)[0].size();
    return (*current)[i][j].alive();
}

bool Cell::Live(Field& field, Game& rules) {
    int count = field.Get(i - 1, j - 1) + field.Get(i, j - 1) + field.Get(i + 1, j - 1) + field.Get(i - 1, j) + field.Get(i + 1, j) + field.Get(i - 1, j + 1) + field.Get(i, j + 1) + field.Get(i + 1, j + 1);
    if (!is_alive && count == rules.quantity_to_birth) {
        return 1;
    }
    else if (is_alive && rules.lcount_neighbour <= count && count <= rules.ucount_neighbour) {
        return 1;
    }
    return 0;

}


void Field::ButtonsCatch() {
    while (true) {
        for (int keyCode = 0; keyCode < 256; ++keyCode) {
            if (GetAsyncKeyState(keyCode) & 0x8000) {
                if (keyCode == 32) {
                    status = 1 - status;
                    drawCell(cursor.first, cursor.second, 1 + status, rules.color);
                }

                if (status)
                    continue;
                prev_cursor = cursor;

                if (keyCode == 38) {
                    cursor.first = (cursor.first - 1 + rules.height)% rules.height;
                }
                else if (keyCode == 40) {
                    cursor.first = (cursor.first + 1) % rules.height;
                }
                else if (keyCode == 39) {
                    cursor.second = (cursor.second + 1) % rules.width;
                }
                else if (keyCode == 37) {
                    cursor.second = (cursor.second - 1 + rules.width) % rules.width;
                }
                else if (keyCode == 13) {
                    (*current)[cursor.first][cursor.second].change_life();
                    drawCell(cursor.first, cursor.second, (*current)[cursor.first][cursor.second].alive(), rules.color);
                }
                if (prev_cursor != cursor) {
                    drawCell(prev_cursor.first, prev_cursor.second, ((*current)[prev_cursor.first][prev_cursor.second].alive()) ? 0: 2, rules.color);
                    drawCell(cursor.first, cursor.second, 1, rules.color);
                }
                
            }
        }

        // Add a small delay to avoid high CPU usage
        this_thread::sleep_for(chrono::milliseconds(CATCH_DELAY));
    }

}

Field::~Field() {
    delete[] next;
    delete[] current;
}

