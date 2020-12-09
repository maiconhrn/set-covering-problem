//
// Created by maicon on 06/12/2020.
//

#ifndef SET_COVERING_PROBLEM_SCP_HPP
#define SET_COVERING_PROBLEM_SCP_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <istream>
#include <random>
#include <algorithm>
#include <bits/unordered_map.h>

#define MT std::mt19937 mt(std::random_device)

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

class scp {
private:
    const static short N_POP = 500;
    const static short N_MAX = 500;

    struct line;

    struct column {
        int index;
        float cost;
        std::unordered_map<int, struct line *> covered_lines;

        column();

        bool operator==(const column &rhs) const;

        bool operator!=(const column &rhs) const;
    };

    struct line {
        int index;
        std::unordered_map<int, struct column *> covered_by_columns;

        line();

        column *min_column(const std::unordered_map<int, struct line *> &uncovered_lines);

        bool operator==(const line &rhs) const;

        bool operator!=(const line &rhs) const;
    };

    struct solution {
    private:
        float _cost = -1;

    public:
        std::unordered_map<int, struct column *> columns;

        bool operator==(const solution &rhs) const;

        bool operator!=(const solution &rhs) const;

        float cost();
    };

    int n_lines;
    int n_columns;
    std::unordered_map<int, struct column *> columns;
    std::unordered_map<int, struct line *> lines;

    static int extract_first_integer(const std::string &str);

    solution generate_individuous();

    std::vector<struct solution> generate_initial_population();

    float fitness(solution population);

    solution cross(std::vector<struct solution> population);

    solution mutate(solution individuous);

    solution best_fitness(std::vector<struct solution> population);

    solution worst_fitness(std::vector<struct solution> population);

    static std::unordered_map<int, struct line *> intersect_by_key(
            const std::unordered_map<int, struct line *> &a,
            const std::unordered_map<int, struct line *> &b);

    static std::unordered_map<int, struct column *> intersect_by_key(
            const std::unordered_map<int, struct column *> &a,
            const std::unordered_map<int, struct column *> &b);

    column *min_column(const std::unordered_map<int, struct line *> &uncovered_lines,
                       const std::unordered_map<int, struct column *> &columns);

public:
    scp();

    void read(std::istream &in);

    void run_genetics();
};

#endif //SET_COVERING_PROBLEM_SCP_HPP
