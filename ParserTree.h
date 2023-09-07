#pragma once

#include<variant>
#include <iostream>
#include <vector>
#include <memory>


class MyCoolBD;

struct Node {
    bool (*func) (std::variant<int, bool, float, double, std::string>, std::variant<int, bool, float, double, std::string>) = nullptr;
    bool (*boolFunc)(bool a, bool b) = nullptr;

    std::pair<std::string, std::string> value;
    std::string operr;

    std::shared_ptr<Node> left = nullptr;
    std::shared_ptr<Node> right = nullptr;
    std::shared_ptr<Node> parent = nullptr;

    bool is_key = false;


};

bool accept(std::shared_ptr<Node> cur, MyCoolBD& bd, size_t i);

bool acceptForTwoRows(std::shared_ptr<Node> cur, MyCoolBD& bd, size_t i, size_t j, const std::string& table1, const std::string& table2);

std::shared_ptr<Node> ParserTree(const std::string& s, MyCoolBD& bd);

std::pair<std::string, std::string> GetTableAndKey(const std::string& node_str);

void walk(Node*);