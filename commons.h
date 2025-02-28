#pragma once

#include<vector>
#include<iostream>
#include"rapidjson/document.h"
#include"rapidjson/filereadstream.h"

using namespace std;

#define CATCH_DELAY 150


class Field;
class Game;

class Cell {
    bool is_alive;
public:
    int i; //coords
    int j;
    bool Live(Field& field, Game& game);
    bool alive() { return is_alive; }
    void change_life() { is_alive = 1 - is_alive; }
    void set_life(bool val) { is_alive = val; }
    Cell(bool status) : i(0), j(0), is_alive(status) {  }
};

class Field {
    vector<vector<Cell>>* current;
    vector<vector<Cell>>* next;
    Game& rules;
    bool status; //1 - working, 0 - stopped
    pair<int, int> cursor;
    pair<int, int> prev_cursor;

public:
    static void drawCell(int x, int y, int type, int color);
    static void setCursorPosition(int x, int y);
    void Display();
    void GenerateNext();
    void ButtonsCatch();
    void StartCatch();
    Field() = delete;
    Field(Game& rules);
    bool Get(int i, int j);
    ~Field();

};

class Game {
private:
public:
    int width;
    int height;
    int quantity_to_birth; //condition that says how many cells needed to create alive cell
    int lcount_neighbour; //how many neghbours is needed to continue life of the cell, lower bound
    int ucount_neighbour; // upper bound
    int fps;
    Field* field = nullptr;
    int color;

    Game() = delete; //useless, as compiler won't create a constructor
    Game(int width, int height, int quantity, int lcount, int ucount, int color) : width(width), height(height), quantity_to_birth(quantity), lcount_neighbour(lcount), ucount_neighbour(ucount), color(color) {};
    Game(const char* filename);
};