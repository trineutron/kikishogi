#include <cassert>
#include <iostream>
#include <sstream>

constexpr int8_t board_startpos[9][9]{
    {-2, -3, -4, -5, -8, -5, -4, -3, -2},
    {0, -7, 0, 0, 0, 0, 0, -6, 0},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1},
    {},
    {},
    {},
    {1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 6, 0, 0, 0, 0, 0, 7, 0},
    {2, 3, 4, 5, 8, 5, 4, 3, 2},
};

void usi_init() {
    std::cout << "id name test" << std::endl;
    std::cout << "id author trineutron" << std::endl;
    std::cout << "usiok" << std::endl;
}

void ready() { std::cout << "readyok" << std::endl; }

int get_type(char c) {
    switch (c) {
        case 'P':
            return 1;
        case 'L':
            return 2;
        case 'N':
            return 3;
        case 'S':
            return 4;
        case 'G':
            return 5;
        case 'B':
            return 6;
        case 'R':
            return 7;
        case 'K':
            return 8;
        default:
            return 0;
    }
}

void move(int8_t board_current[9][9], int turn, const std::string &word) {
    int y_prev = '9' - word[0];
    int x_prev = word[1] - 'a';
    bool is_new = word[1] == '*';
    int y_next = '9' - word[2];
    int x_next = word[3] - 'a';
    bool promote = word.back() == '+';
    int type;
    if (is_new) {
        type = turn * get_type(word[0]);
    } else {
        type = board_current[x_prev][y_prev];
        board_current[x_prev][y_prev] = 0;
    }
    assert(turn * type > 0);
    if (promote) {
        if (type > 0) {
            type += 8;
        } else {
            type -= 8;
        }
    }
    board_current[x_next][y_next] = type;
}

void get_board(const std::string &s, int8_t board_out[9][9], int &turn) {
    std::stringstream ss(s);
    bool is_move_mode = false;
    while (true) {
        std::string word;
        ss >> word;
        if (not ss) break;
        if (word == "position") {
            continue;
        } else if (word == "startpos") {
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    board_out[i][j] = board_startpos[i][j];
                }
            }
        } else if (word == "sfen") {
            std::cerr << "Warning: sfen" << std::endl;
            continue;
        } else if (word == "moves") {
            is_move_mode = true;
            turn = 1;
        } else if (is_move_mode) {
            move(board_out, turn, word);
            turn = -turn;
        } else {
            std::cerr << "Warning: sfenstring" << std::endl;
        }
    }
}

void print_board(const int8_t board_print[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            std::cerr << ' ' << int(board_print[i][j]);
        }
        std::cerr << std::endl;
    }
}

std::string bestmove(int8_t board[9][9], int turn) {
    std::string res;
    for (int i = 0; i < 9; i++) {
        if (not res.empty()) break;
        for (int j = 0; j < 9; j++) {
            const int type = turn * board[i][j];
            if (type <= 0) continue;
            if (type == 3 or type == 6) continue;
            const int i_next = i - turn;
            if (i_next < 0 or 9 <= i_next) continue;
            if (turn * board[i_next][j] > 0) continue;
            res += '9' - j;
            res += i + 'a';
            res += '9' - j;
            res += i_next + 'a';
            if (i_next == 0 or i_next == 8) res += '+';
            break;
        }
    }
    return res;
}

int main() {
    int8_t board[9][9];
    int turn = 1;
    print_board(board_startpos);
    while (true) {
        std::string s;
        std::getline(std::cin, s);
        std::string type = s.substr(0, s.find(' '));
        if (type == "usi") {
            usi_init();
        } else if (type == "isready") {
            ready();
        } else if (type == "position") {
            get_board(s, board, turn);
            print_board(board);
            std::cerr << turn << std::endl;
        } else if (type == "go") {
            std::cout << "bestmove " << bestmove(board, turn) << std::endl;
        } else if (type == "quit") {
            return 0;
        }
    }
}