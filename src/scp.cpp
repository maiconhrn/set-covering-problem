//
// Created by maicon on 06/12/2020.
//

#include "scp.hpp"

scp::column::column() {
    this->index = -1;
    this->cost = 0;
    this->covered_lines.clear();
}

bool scp::column::operator==(const scp::column &rhs) const {
    return index == rhs.index;
}

bool scp::column::operator!=(const scp::column &rhs) const {
    return !(rhs == *this);
}

scp::line::line() {
    this->index = -1;
    this->covered_by_columns.clear();
}


std::unordered_map<int, struct scp::line *> scp::intersect_by_key(
        const std::unordered_map<int, struct scp::line *> &a,
        const std::unordered_map<int, struct scp::line *> &b) {
    auto res = std::unordered_map<int, struct scp::line *>();

    for (auto &it : a) {
        if (b.find(it.first) != b.end()) {
            res.insert(it);
        }
    }

    return res;
}

std::unordered_map<int, struct scp::column *> scp::intersect_by_key(
        const std::unordered_map<int, struct scp::column *> &a,
        const std::unordered_map<int, struct scp::column *> &b) {
    auto res = std::unordered_map<int, struct scp::column *>();

    for (auto &it : a) {
        if (b.find(it.first) != b.end()) {
            res.insert(it);
        }
    }

    return res;
}

scp::column *scp::line::min_column(const std::unordered_map<int, struct scp::line *> &uncovered_lines) {
    auto *column = this->covered_by_columns.begin()->second;

    for (auto &it : this->covered_by_columns) {
        if ((it.second->cost
             / (float) intersect_by_key(uncovered_lines,
                                        it.second->covered_lines).size())
            < (column->cost / (float) intersect_by_key(uncovered_lines,
                                                       column->covered_lines).size())) {
            column = it.second;
        }
    }

    return column;
}

bool scp::line::operator==(const scp::line &rhs) const {
    return index == rhs.index;
}

bool scp::line::operator!=(const scp::line &rhs) const {
    return !(rhs == *this);
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

    std::getline(cin, str_line); // go pointer to next line

    int line_index;

    for (int i = 0; i < n_columns; ++i) {
        cin.clear();

        auto *column = new scp::column();
        cin >> column->index;
        cin >> column->cost;

        std::stringstream ss;

        str_line = "";
        std::getline(cin, str_line);

        ss << str_line;
        while (!ss.eof()) {
            ss >> line_index;

            line *line;
            if (lines.find(line_index) == lines.end()) {
                line = new scp::line();
                line->index = line_index;
                line->covered_by_columns.insert({column->index, column});
                column->covered_lines.insert({line->index, line});
                this->lines.insert({line->index, line});
            } else {
                line = lines[line_index];
                line->covered_by_columns.insert({column->index, column});
                column->covered_lines.insert({line->index, line});
            }
        }

        this->columns.insert({column->index, column});
    }
}

scp::solution scp::generate_individuous() {
    auto solution = scp::solution();
    auto uncovered_lines = std::unordered_map<int, struct scp::line *>(this->lines.begin(),
                                                                       this->lines.end());

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(1, n_lines);

    while (!uncovered_lines.empty()) {
        std::unordered_map<int, struct scp::line *>::iterator it;
        while ((it = uncovered_lines.find(dist(mt))) == uncovered_lines.end());
        auto line = it->second;
        auto column = line->min_column(uncovered_lines);

        solution.columns.insert({column->index, column});
        for (auto &covered_line : column->covered_lines) {
            uncovered_lines.erase(covered_line.first);
        }
    }

    return solution;
}

std::vector<struct scp::solution> scp::generate_initial_population() {
    auto pop = std::vector<struct scp::solution>();

    while (pop.size() < N_POP) {
        pop.push_back(generate_individuous());
    }

    return pop;
}

float scp::fitness(scp::solution individuous) {
    return individuous.cost();
}

scp::column *scp::min_column(const std::unordered_map<int, struct scp::line *> &uncovered_lines,
                             const std::unordered_map<int, struct scp::column *> &cols) {
    auto *column = cols.begin()->second;

    for (auto &it : columns) {
        if ((it.second->cost
             / (float) intersect_by_key(uncovered_lines,
                                        it.second->covered_lines).size())
            < (column->cost / (float) intersect_by_key(uncovered_lines,
                                                       column->covered_lines).size())) {
            column = it.second;
        }
    }

    return column;
}

scp::solution scp::cross(std::vector<struct scp::solution> population) {
    std::sort(population.begin(), population.end(), [&population](const scp::solution &a,
                                                                  const scp::solution &b) {
        auto npop = (N_POP * (N_POP + 1));
        auto criteria = [&population, &npop](const scp::solution &s) {
            return (2 * std::distance(population.begin(),
                                      std::find(population.begin(),
                                                population.end(),
                                                s)))
                   / npop;
        };

        return criteria(a) < criteria(b);
    });

    auto chromosome_x = population.begin();
    auto chromosome_y = ++population.begin();
    auto chromosome_aux = scp::solution();
    chromosome_aux.columns.insert(chromosome_x->columns.begin(),
                                  chromosome_x->columns.end());
    chromosome_aux.columns.insert(chromosome_y->columns.begin(),
                                  chromosome_y->columns.end());

    auto chromosome_z = scp::solution();

    auto uncovered_lines = std::unordered_map<int, struct scp::line *>(this->lines.begin(),
                                                                       this->lines.end());

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(1, n_lines);

    while (!uncovered_lines.empty()) {
        std::unordered_map<int, struct scp::line *>::iterator it;
        while ((it = uncovered_lines.find(dist(mt))) == uncovered_lines.end());
        auto line = it->second;
        auto column = min_column(uncovered_lines,
                                 intersect_by_key(line->covered_by_columns,
                                                  chromosome_aux.columns));

        chromosome_z.columns.insert({column->index, column});
        for (auto &covered_line : column->covered_lines) {
            uncovered_lines.erase(covered_line.first);
        }
    }


    return chromosome_z;
}

scp::solution scp::mutate(scp::solution individuous) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist0(0, 1);
    std::uniform_int_distribution<int> dist1(1, n_columns);

    for (int i = 0; i < dist0(mt) * individuous.columns.size(); ++i) {
        auto *column = this->columns[dist1(mt)];

        individuous.columns.insert({column->index, column});
    }

    return individuous;
}

scp::solution scp::best_fitness(std::vector<struct solution> population) {
    auto best = *population.begin();

    for (auto &it : population) {
        if (fitness(it) < fitness(best)) {
            best = it;
        }
    }

    return best;
}

scp::solution scp::worst_fitness(std::vector<struct solution> population) {
    auto worst = *population.begin();

    for (auto &it : population) {
        if (fitness(it) > fitness(worst)) {
            worst = it;
        }
    }

    return worst;
}

void scp::run_genetics() {
//    Gerar população inicial
//    while critério-de-parada do
//    Avaliação: Calcula a qualidade dos cromossomos filhos
//    Seleção: Escolher cromossomos reprodutores
//    Cruzamento: Fazer o cruzamento dos reprodutores
//    Mutação: Gerar mutações da população
//    Busca local: Fazer busca na vizinhança da solução (cromossomo)s
//    Atualização: Atualizar a população
//    end-while


    auto population = generate_initial_population();

    int i = 0;
    while (i <= N_MAX) {
        auto candidate = cross(population);

        candidate = mutate(candidate);

        auto worst = worst_fitness(population);

        if (fitness(candidate) < fitness(worst)) {
            population.erase(std::find(population.begin(),
                                       population.end(),
                                       worst));
            population.push_back(candidate);

            i = 0;
        } else {
            i++;
        }

        cout << i << endl
             << "Best: " << best_fitness(population).cost() << endl
             << "Worst: " << worst_fitness(population).cost() << endl;
    }
}

bool scp::solution::operator==(const scp::solution &rhs) const {
    return columns == rhs.columns;
}

bool scp::solution::operator!=(const scp::solution &rhs) const {
    return !(rhs == *this);
}

float scp::solution::cost() {
    if (_cost == -1) {
        _cost = 0;

        for (auto &it : this->columns) {
            _cost += it.second->cost;
        }
    }

    return _cost;
}
