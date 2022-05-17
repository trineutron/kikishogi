#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

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

constexpr int8_t mochigoma_startpos[2][8]{};

std::mt19937_64 rng;

std::string bestmove(const int8_t board[9][9], const int8_t mochigoma[2][8],
                     int turn);

void usi_init() {
    std::cout << "id name kikishogi" << std::endl;
    std::cout << "id author trineutron" << std::endl;
    std::cout << "usiok" << std::endl;
}

void ready() { std::cout << "readyok" << std::endl; }

constexpr char text_type[]{' ', 'P', 'L', 'N', 'S', 'G', 'B', 'R', 'K'};

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

const std::vector<std::pair<int, int>> type_dir[9]{
    {},
    {{-1, 0}},
    {{-1, 0}},
    {{-2, -1}, {-2, 1}},
    {{-1, -1}, {-1, 0}, {-1, 1}, {1, -1}, {1, 1}},
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 0}},
    {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}},
    {{-1, 0}, {0, -1}, {0, 1}, {1, 0}},
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

constexpr bool type_run[9]{false, false, true, false, false,
                           false, true,  true, false};

void copy_board(const int8_t board_in[9][9], int8_t board_out[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            board_out[i][j] = board_in[i][j];
        }
    }
}

void copy_mochigoma(const int8_t mochigoma_in[2][8],
                    int8_t mochigoma_out[2][8]) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            mochigoma_out[i][j] = mochigoma_in[i][j];
        }
    }
}

bool in_board(const int x, const int y) {
    return 0 <= x and x < 9 and 0 <= y and y < 9;
}

void move(int8_t board_current[9][9], int8_t mochigoma[2][8], int turn,
          const std::string &word) {
    int y_prev = '9' - word[0];
    int x_prev = word[1] - 'a';
    bool is_new = word[1] == '*';
    int y_next = '9' - word[2];
    int x_next = word[3] - 'a';
    bool promote = word.back() == '+';
    int type;
    if (is_new) {
        type = get_type(word[0]);
        mochigoma[(1 - turn) >> 1][type]--;
        type *= turn;
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
    int type_capture = abs(board_current[x_next][y_next]);
    if (not is_new and type_capture) {
        mochigoma[(1 - turn) >> 1][type_capture & 7]++;
    }
    board_current[x_next][y_next] = type;
}

void get_board(const std::string &s, int8_t board_out[9][9],
               int8_t mochigoma_out[2][8], int &turn) {
    std::stringstream ss(s);
    bool is_move_mode = false;
    while (true) {
        std::string word;
        ss >> word;
        if (not ss) break;
        if (word == "position") {
        } else if (word == "startpos") {
            copy_board(board_startpos, board_out);
            copy_mochigoma(mochigoma_startpos, mochigoma_out);
        } else if (word == "sfen") {
            std::cerr << "Warning: sfen" << std::endl;
        } else if (word == "moves") {
            is_move_mode = true;
            turn = 1;
        } else if (is_move_mode) {
            move(board_out, mochigoma_out, turn, word);
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

bool can_move(const int x, const int type, const int turn) {
    if (turn == 1) {
        return not((x < 2 and type == 3) or (x == 0 and type <= 2));
    } else {
        return not((x >= 7 and type == 3) or (x == 8 and type <= 2));
    }
}

std::vector<std::string> moves(const int8_t board[9][9],
                               const int8_t mochigoma[2][8], int turn) {
    std::vector<std::string> res;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            const int type = turn * board[i][j];
            if (type <= 0) continue;
            int type_move[2]{type, 0};
            if (9 <= type and type <= 13) {
                type_move[0] = 5;
            } else if (type >= 14) {
                type_move[0] = type - 8;
                type_move[1] = 8;
            }
            for (int k = 0; k < 2; k++) {
                int run_limit = type_run[type_move[k]] ? 8 : 1;
                for (auto &&[dx, dy] : type_dir[type_move[k]]) {
                    for (int run_idx = 1; run_idx <= run_limit; run_idx++) {
                        const int i_next = i + run_idx * turn * dx;
                        const int j_next = j + run_idx * dy;
                        if (not in_board(i_next, j_next)) break;
                        if (turn * board[i_next][j_next] > 0) break;
                        std::string s;
                        s += '9' - j;
                        s += i + 'a';
                        s += '9' - j_next;
                        s += i_next + 'a';
                        bool can_promote;
                        if (turn == 1) {
                            can_promote = i < 3 or i_next < 3;
                        } else {
                            can_promote = i >= 6 or i_next >= 6;
                        }
                        if (type == 5 or type >= 8) can_promote = false;
                        if (turn * board[i_next][j_next] == -8) {
                            res.push_back("capture");
                        }
                        if (can_move(i_next, type, turn)) res.push_back(s);
                        if (can_promote) {
                            s += '+';
                            res.push_back(s);
                        }
                        if (board[i_next][j_next]) break;
                    }
                }
            }
        }
    }
    bool fu_already[9]{};
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == turn) {
                fu_already[j] = true;
            }
        }
    }

    for (int type = 1; type < 8; type++) {
        if (mochigoma[(1 - turn) >> 1][type] == 0) continue;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (board[i][j]) continue;
                if (not can_move(i, type, turn)) continue;
                if (type == 1 and fu_already[j]) continue;
                std::string s;
                s += text_type[type];
                s += '*';
                s += '9' - j;
                s += i + 'a';
                res.push_back(s);
            }
        }
    }
    return res;
}

bool is_legal(const int8_t board[9][9], const int8_t mochigoma[2][8], int turn,
              const std::string &move_to_check) {
    int8_t board_new[9][9];
    int8_t mochigoma_new[2][8];
    copy_board(board, board_new);
    copy_mochigoma(mochigoma, mochigoma_new);
    move(board_new, mochigoma_new, turn, move_to_check);
    auto res = moves(board_new, mochigoma_new, -turn);
    for (auto &&s : res) {
        if (s == "capture") return false;
    }
    if (move_to_check[0] == 'P') {
        return bestmove(board_new, mochigoma_new, -turn) != "resign";
    }
    return true;
}

std::string bestmove(const int8_t board[9][9], const int8_t mochigoma[2][8],
                     int turn) {
    auto res = moves(board, mochigoma, turn);
    std::shuffle(res.begin(), res.end(), rng);
    for (auto &&s : res) {
        if (is_legal(board, mochigoma, turn, s)) return s;
    }
    return "resign";
}

int main() {
    int8_t board[9][9];
    int8_t mochigoma[2][8];
    int turn = 1;
    // print_board(board_startpos);
    while (true) {
        std::string s;
        std::getline(std::cin, s);
        std::string type = s.substr(0, s.find(' '));
        if (type == "usi") {
            usi_init();
        } else if (type == "isready") {
            ready();
        } else if (type == "position") {
            get_board(s, board, mochigoma, turn);
        } else if (type == "go") {
            std::cout << "bestmove " << bestmove(board, mochigoma, turn)
                      << std::endl;
        } else if (type == "quit") {
            return 0;
        }
    }
}
