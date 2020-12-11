//
// Created by maicon on 06/12/2020.
//

#include "scp.hpp"

bool a_less_b(const double &a, const double &b) {
    return std::fabs(a - b) > 1.0e-05 && a < b;

}

int random_int_in_range(int a, int b) {
    static std::random_device rd;
    static std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(a, b);

    return dist(mt);
}

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


std::map<int, struct scp::line *> scp::intersect_by_key(
        const std::map<int, struct scp::line *> &a,
        const std::map<int, struct scp::line *> &b) {
    auto res = std::map<int, struct scp::line *>();

    for (auto &it : a) {
        if (b.find(it.first) != b.end()) {
            res.insert(it);
        }
    }

    return res;
}

std::map<int, struct scp::column *> scp::intersect_by_key(
        const std::map<int, struct scp::column *> &a,
        const std::map<int, struct scp::column *> &b) {
    auto res = std::map<int, struct scp::column *>();

    for (auto &it : a) {
        if (b.find(it.first) != b.end()) {
            res.insert(it);
        }
    }

    return res;
}

scp::column *scp::line::min_column(const std::map<int, struct scp::line *> &uncovered_lines) {
    auto *column = this->covered_by_columns.begin()->second;

    for (auto &it : this->covered_by_columns) {
        if (a_less_b((it.second->cost
                      / (double) intersect_by_key(uncovered_lines,
                                                  it.second->covered_lines).size()),
                     (column->cost / (double) intersect_by_key(uncovered_lines,
                                                               column->covered_lines).size()))) {
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
    auto uncovered_lines =
            std::map<int, struct scp::line *>(this->lines.begin(),
                                              this->lines.end());
    std::map<int, struct scp::line *>::iterator it;

    while (!uncovered_lines.empty()) {
        it = uncovered_lines.begin();
        std::advance(it, random_int_in_range(0, (int) uncovered_lines.size() - 1));
        auto *line = it->second;
        auto *column = line->min_column(uncovered_lines);

        for (auto &item : column->covered_lines) {
            uncovered_lines.erase(item.first);
            solution.columns_cover_line[item.first]++;
        }
        solution.insert_column(column);
    }

    return solution;
}

void scp::erase_redundancy(scp::solution &individuous) {
    auto to_verify = std::map<int, struct column *>(individuous.columns.begin(),
                                                    individuous.columns.end());

    std::map<int, struct column *>::iterator it;
    while (!to_verify.empty()) {
        it = to_verify.begin();
        std::advance(it, random_int_in_range(0, (int) to_verify.size() - 1));
        auto *column = it->second;

        to_verify.erase(it);

        bool reduntancy = true;
        for (auto &item : column->covered_lines) {
            if (individuous.columns_cover_line[item.first] < 2) {
                reduntancy = false;
            }
        }
        if (reduntancy) {
            individuous.columns.erase(column->index);
            for (auto &item : column->covered_lines) {
                individuous.columns_cover_line[item.first]--;
            }
        }
    }
}

std::set<struct scp::solution, scp::population_cmp> scp::generate_initial_population() {
    auto pop = std::set<struct scp::solution, scp::population_cmp>();

    while (pop.size() < N_POP) {
        auto individuous = generate_individuous();
        erase_redundancy(individuous);
        pop.insert(individuous);
    }

    return pop;
}

double scp::fitness(scp::solution &individuous) {
    return individuous.cost;
}

scp::column *scp::min_column(const std::map<int, struct scp::line *> &uncovered_lines,
                             const std::map<int, struct scp::column *> &cols) {
    auto *column = cols.begin()->second;

    for (auto &it : cols) {
        if (a_less_b((it.second->cost
                      / (double) intersect_by_key(uncovered_lines,
                                                  it.second->covered_lines).size()),
                     (column->cost
                      / (double) intersect_by_key(uncovered_lines,
                                                  column->covered_lines).size()))) {
            column = it.second;
        }
    }

    return column;
}

scp::solution scp::cross(std::set<struct scp::solution, scp::population_cmp> population) {
    auto chromosome_x = population.rbegin();
    auto chromosome_y = std::next(population.rbegin());
    auto chromosome_aux = scp::solution();
    chromosome_aux.columns.insert(chromosome_x->columns.begin(),
                                  chromosome_x->columns.end());
    chromosome_aux.cost += chromosome_x->cost;
    chromosome_aux.columns.insert(chromosome_y->columns.begin(),
                                  chromosome_y->columns.end());
    chromosome_aux.cost += chromosome_y->cost;
    chromosome_aux.columns_cover_line.insert(chromosome_x->columns_cover_line.begin(),
                                             chromosome_x->columns_cover_line.end());
    chromosome_aux.columns_cover_line.insert(chromosome_y->columns_cover_line.begin(),
                                             chromosome_y->columns_cover_line.end());

    auto chromosome_z = scp::solution();

    auto uncovered_lines = std::map<int, struct scp::line *>(this->lines.begin(),
                                                             this->lines.end());

    std::map<int, struct scp::line *>::iterator it;
    while (!uncovered_lines.empty()) {
        it = uncovered_lines.begin();
        std::advance(it, random_int_in_range(0, (int) uncovered_lines.size() - 1));
        auto *line = it->second;
        auto *column = min_column(uncovered_lines,
                                  intersect_by_key(line->covered_by_columns,
                                                   chromosome_aux.columns));

        chromosome_z.insert_column(column);
        for (auto &covered_line : column->covered_lines) {
            uncovered_lines.erase(covered_line.first);
            chromosome_z.columns_cover_line[covered_line.first]++;
        }
    }

    erase_redundancy(chromosome_z);

    return chromosome_z;
}

void scp::mutate(scp::solution &individuous) {
    if (random_int_in_range(0, 1)) {
        auto n = neighbor(individuous);

        erase_redundancy(individuous);

        individuous.cost = n.cost;
        individuous.columns = n.columns;
        individuous.columns_cover_line = n.columns_cover_line;
    }
}


scp::solution scp::neighbor(scp::solution &individuous) {
    auto neighbor = scp::solution();
    neighbor.columns.insert(individuous.columns.begin(),
                            individuous.columns.end());
    neighbor.cost = individuous.cost;
    neighbor.columns_cover_line.insert(individuous.columns_cover_line.begin(),
                                       individuous.columns_cover_line.end());

    auto *column = this->columns[random_int_in_range(1, n_columns)];

    neighbor.insert_column(column);
    for (auto &it : column->covered_lines) {
        neighbor.columns_cover_line[it.first]++;
    }

    erase_redundancy(individuous);

    return neighbor;
}

void scp::local_search(solution &individuous) {
    auto best_local = scp::solution();
    best_local.columns.insert(individuous.columns.begin(),
                              individuous.columns.end());
    best_local.cost = individuous.cost;
    best_local.columns_cover_line.insert(individuous.columns_cover_line.begin(),
                                         individuous.columns_cover_line.end());
    auto neighborhood = std::set<struct solution, population_cmp>();
    bool retry = false;

    do {
        for (int i = 0; i < 100; i++) {
            neighborhood.insert(neighbor(best_local));
        }

        if (a_less_b(neighborhood.rbegin()->cost, best_local.cost)) {
            best_local = *neighborhood.rbegin();
            retry = true;
        } else {
            retry = false;
        }
    } while (retry);

    individuous.cost = best_local.cost;
    individuous.columns = best_local.columns;
    individuous.columns_cover_line = best_local.columns_cover_line;
}

scp::solution scp::best_fitness(std::set<struct solution, scp::population_cmp> &population) {
    return *population.rbegin();
}

scp::solution scp::worst_fitness(std::set<struct solution, scp::population_cmp> &population) {
    return *population.begin();
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

        mutate(candidate);

//        local_search(candidate);

        auto worst = worst_fitness(population);

        if (a_less_b(fitness(candidate), fitness(worst))) {
            population.erase(population.find(worst));
            population.insert(candidate);

            i = 0;
        } else {
            i++;
        }

        cout << "i: " << i << endl
             << "Best: " << best_fitness(population).cost << endl
             << "Worst: " << worst_fitness(population).cost << endl
             << "Pop size: " << population.size() << endl
             << "Time: " << double(clock() - begin) / CLOCKS_PER_SEC << "s" << endl;
    }
}

void scp::run_genetics_local_search() {
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

        mutate(candidate);

        local_search(candidate);

        auto worst = worst_fitness(population);

        if (a_less_b(fitness(candidate), fitness(worst))) {
            population.erase(population.find(worst));
            population.insert(candidate);

            i = 0;
        } else {
            i++;
        }

//        cout << "i: " << i << endl
//             << "Best: " << best_fitness(population).cost << endl
//             << "Worst: " << worst_fitness(population).cost << endl
//             << "Pop size: " << population.size() << endl
//             << "Time: " << double(clock() - begin) / CLOCKS_PER_SEC << "s" << endl;
    }

    result = best_fitness(population);
}

bool scp::solution::operator==(const scp::solution &rhs) const {
    return columns == rhs.columns;
}

bool scp::solution::operator!=(const scp::solution &rhs) const {
    return !(rhs == *this);
}

void scp::solution::insert_column(scp::column *column) {
    this->cost += column->cost;

    this->columns.insert({column->index, column});
}

void scp::solution::erase_column(column column) {
    this->cost -= column.cost;

    this->columns.erase(column.index);
}

scp::column scp::solution::worst_fitness_column() {
    auto worst = *this->columns.begin()->second;

    for (auto &it : this->columns) {
        if (a_less_b(worst.cost, it.second->cost)) {
            worst = *it.second;
        }
    }

    return worst;
}
