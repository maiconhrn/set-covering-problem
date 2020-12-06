//
// Created by maicon on 06/12/2020.
//

#include "scp.hpp"

scp::scp_column::scp_column() {
    this->index = -1;
    this->cost = 0;
    this->covered_lines.clear();
}

scp::scp_line::scp_line() {
    this->index = -1;
    this->covered_by_columns.clear();
}

scp::scp() {
    this->n_lines = 0;
    this->n_columns = 0;
    this->columns.clear();
    this->lines.clear();
}

int scp::extract_first_integer(const std::string &str) {
    std::stringstream ss;

    ss << str;

    std::string temp;
    int found;
    while (!ss.eof()) {
        ss >> temp;

        if (std::stringstream(temp) >> found) {
            return found;
        }

        temp = "";
    }

    cerr << "None integer found" << endl;

    return -1;
}

void scp::read(std::istream &in) {
    std::string str_line;

    std::getline(cin, str_line);
    this->n_lines = extract_first_integer(str_line);

    str_line = "";

    std::getline(cin, str_line);
    this->n_columns = extract_first_integer(str_line);

    int line_index;

    std::getline(cin, str_line); // go pointer to next line

    std::unordered_map<int, struct scp_line *> lines_aux;

    for (int i = 0; i < n_columns; ++i) {
        cin.clear();

        auto *column = new scp_column();
        cin >> column->index;
        cin >> column->cost;

        std::stringstream ss;

        str_line = "";
        std::getline(cin, str_line);

        ss << str_line;
        while (!ss.eof()) {
            ss >> line_index;

            scp_line *line;
            if (lines_aux.find(line_index) == lines_aux.end()) {
                line = new scp_line();
            } else {
                line = lines_aux[line_index];
            }
            line->index = line_index;
            line->covered_by_columns.insert(column);

            column->covered_lines.insert(line);
            lines_aux.insert({line->index, line});
        }

        this->columns.push_back(column);
    }
}
