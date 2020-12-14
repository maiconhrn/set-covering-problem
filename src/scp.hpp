//
// Created by maicon on 06/12/2020.
//

#ifndef SET_COVERING_PROBLEM_SCP_HPP
#define SET_COVERING_PROBLEM_SCP_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <istream>
#include <random>
#include <set>
#include <algorithm>

#define MT std::mt19937 mt(std::random_device)

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

class scp {
private:
    const static short N_POP = 100;
    const static short N_MAX = 1000;
    const static short N_LS = 100;

    struct line;

    struct column {
        int index;
        double cost;
        std::map<int, struct line *> covered_lines;

        column();

        bool operator==(const column &rhs) const;

        bool operator!=(const column &rhs) const;
    };

    struct line {
        int index;
        std::map<int, struct column *> covered_by_columns;

        line();

        column *min_column(const std::map<int, struct line *> &uncovered_lines);

        bool operator==(const line &rhs) const;

        bool operator!=(const line &rhs) const;
    };

    struct solution {
        std::map<int, struct column *> columns;

        double cost = 0;

        std::map<int, int> columns_cover_line;

        bool operator==(const solution &rhs) const;

        bool operator!=(const solution &rhs) const;;

        void insert_column(scp::column *column);

        void erase_column(column column);

        column worst_fitness_column();
    };

    struct population_cmp {
        bool operator()(const solution &lhs, const solution &rhs) const {
            return lhs.cost > rhs.cost;
        }
    };

    int n_lines;
    int n_columns;
    std::map<int, struct column *> columns;
    std::map<int, struct line *> lines;

    static int extract_first_integer(const std::string &str);

    solution generate_individuous();

    static void erase_redundancy(solution &individuous);

    std::set<struct solution, population_cmp> generate_initial_population();

    static double fitness(solution &population);

    solution cross(std::set<struct solution, population_cmp> population);

    void mutate(solution &individuous);

    void local_search(solution &individuous);

    solution neighbor(solution &individuous);

    static solution best_fitness(std::set<struct solution, population_cmp> &population);

    static solution worst_fitness(std::set<struct solution, population_cmp> &population);

    static std::map<int, struct line *> intersect_by_key(
            const std::map<int, struct line *> &a,
            const std::map<int, struct line *> &b);

    static std::map<int, struct column *> intersect_by_key(
            const std::map<int, struct column *> &a,
            const std::map<int, struct column *> &b);

    static column *min_column(const std::map<int, struct line *> &uncovered_lines,
                              const std::map<int, struct column *> &columns);

public:
    solution result;

    clock_t begin = clock();

    scp();

    void read(std::istream &in);

    void run_genetics();

    void run_genetics_local_search();
};

#endif //SET_COVERING_PROBLEM_SCP_HPP
