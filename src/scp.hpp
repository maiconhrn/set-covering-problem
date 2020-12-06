//
// Created by maicon on 06/12/2020.
//

#ifndef SET_COVERING_PROBLEM_SCP_HPP
#define SET_COVERING_PROBLEM_SCP_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_map>
#include <istream>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

class scp {
private:
    struct scp_line;

    struct scp_column {
        int index;
        float cost;
        std::set<struct scp_line *> covered_lines;

        scp_column();
    };

    struct scp_line {
        int index;
        std::set<struct scp_column *> covered_by_columns;

        scp_line();
    };

    int n_lines;
    int n_columns;
    std::vector<struct scp_column *> columns;
    std::vector<struct scp_line *> lines;

    static int extract_first_integer(const std::string &str);

public:
    scp();

    void read(std::istream &in);
};

#endif //SET_COVERING_PROBLEM_SCP_HPP
