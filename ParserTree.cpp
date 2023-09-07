#pragma once

#include "ParserTree.h"
#include "Options.h"

#include<iostream>
#include<algorithm>
#include<cstdio>
#include <variant>


class MyCoolBD;

std::variant<int, bool, float, double, std::string> GetValue(const std::string& str, const std::string& type);

bool accept(std::shared_ptr<Node> cur, MyCoolBD& bd, size_t i) {

    if (cur->operr == "NOT" && cur->left->left == nullptr) {
        bool (*func) (bool, bool) = cur->boolFunc;
        return func(true, accept(cur->right, bd, i));
    }
    if (cur->left->left != nullptr && cur->right->left != nullptr) {
        if (cur->boolFunc != nullptr) {
            bool (*func) (bool, bool) = cur->boolFunc;
            return func(accept(cur->left, bd, i), accept(cur->right, bd, i));
        } else {
            bool (*func) (std::variant<int, bool, float, double, std::string>, std::variant<int, bool, float, double, std::string>) = cur->func;
            return func(accept(cur->left, bd, i), accept(cur->right, bd,  i));
        }
    }
    {
        bool (* func)(std::variant<int, bool, float, double, std::string>, std::variant<int, bool, float, double, std::string>) = cur->func;
        std::variant<int, bool, float, double, std::string> a;
        std::variant<int, bool, float, double, std::string> b;

        std::pair<std::string, std::string> key;
        if (cur->left->is_key) {
            key = cur->left->value;
            a = GetValue(bd.BD[key.first][key.second].values[i], bd.BD[key.first][key.second].type);
        }

        if (cur->right->is_key) {
            key = cur->right->value;
            b = GetValue(bd.BD[key.first][key.second].values[i], bd.BD[key.first][key.second].type);
        } else b = GetValue(cur->right->value.second, bd.BD[key.first][key.second].type);

        return func(a, b);
    }
}

bool acceptForTwoRows(std::shared_ptr<Node> cur, MyCoolBD& bd, size_t i, size_t j, const std::string& table1, const std::string& table2) {
    if (cur->operr == "NOT" && cur->left->left == nullptr) {
        bool (*func) (bool, bool) = cur->boolFunc;
        return func(true, acceptForTwoRows(cur->right, bd, i, j, table1, table2));
    }
    if (cur->left->left != nullptr && cur->right->left != nullptr) {
        if (cur->boolFunc != nullptr) {
            bool (*func) (bool, bool) = cur->boolFunc;
            return func(acceptForTwoRows(cur->left, bd, i, j, table1, table2), acceptForTwoRows(cur->right, bd, i, j, table1, table2));
        } else {
            bool (*func) (std::variant<int, bool, float, double, std::string>, std::variant<int, bool, float, double, std::string>) = cur->func;
            return func(acceptForTwoRows(cur->left, bd, i, j, table1, table2), acceptForTwoRows(cur->right, bd,  i, j, table1, table2));
        }
    }
    {
        bool (* func)(std::variant<int, bool, float, double, std::string>, std::variant<int, bool, float, double, std::string>) = cur->func;
        std::variant<int, bool, float, double, std::string> a;
        std::variant<int, bool, float, double, std::string> b;

        std::pair<std::string, std::string> key;
        if (cur->left->is_key) {
            key = cur->left->value;

            std::string table = key.first;
            size_t index;
            if (table == table1) index = i;
            else index = j;

            a = GetValue(bd.BD[table][key.second].values[index], bd.BD[table][key.second].type);
        }

        if (cur->right->is_key) {
            key = cur->right->value;

            std::string table = key.first;
            size_t index;
            if (table == table1) index = i;
            else index = j;

            b = GetValue(bd.BD[table][key.second].values[index], bd.BD[table][key.second].type);
        } else b = GetValue(cur->right->value.second, bd.BD[key.first][key.second].type);

        return func(a, b);
    }
}

std::pair<std::string, std::string> GetTableAndKey(const std::string& node_str) {
    std::string tableName;
    std::string key;

    size_t dot_index = node_str.find(".");
    if (dot_index ==  std::string::npos) {
        return {"", node_str};
    }

    return {node_str.substr(0, dot_index), node_str.substr(dot_index +1)};
}

void walk(std::shared_ptr<Node> curNode) {
    if (curNode == nullptr) return;

    if (curNode->boolFunc != nullptr || curNode->func != nullptr) std::cout<<curNode->operr<<" ";
    else std::cout<<curNode->value.first;
    walk(curNode->left);
    walk(curNode->right);
}

bool Eq(std::variant<int, bool, float, double, std::string> a, std::variant<int, bool, float, double, std::string> b) {
    return a==b;
}

bool MoreThan(std::variant<int, bool, float, double, std::string> a, std::variant<int, bool, float, double, std::string> b) {
    return a > b;
}
bool MoreEqThan(std::variant<int, bool, float, double, std::string> a, std::variant<int, bool, float, double, std::string> b) {
    return a >= b;
}

bool LessThan(std::variant<int, bool, float, double, std::string> a, std::variant<int, bool, float, double, std::string> b) {
    return a < b;
}
bool LessEqThan(std::variant<int, bool, float, double, std::string> a, std::variant<int, bool, float, double, std::string> b) {
    return a <= b;
}

bool Conjunction(bool a, bool b) {
    return a && b;
}

bool Disjunction(bool a, bool b) {
    return a || b;
}

bool Negation(bool f , bool a) {
    return !a;
}

std::shared_ptr<Node> ParserTree(const std::string& s, MyCoolBD& bd) {
    std::shared_ptr<Node> curNode(new Node);
    std::shared_ptr<Node> root = curNode;
    for (size_t i = 0; i < s.length() - 1; ++i) {
        if (s[i] == ' ') continue;
        if (s[i] == '(') {
            std::shared_ptr<Node> node(new Node);
            curNode->left = node;
            node->parent = curNode;
            curNode = node;
            continue;
        }
        if (s[i] == ')') {
            curNode = curNode->parent;
            continue;
        }
        std::string node_str = "";
        while (i < s.length() && s[i] != ' ') {
            node_str+=s[i];
            ++i;
        }

        if (node_str == "NOT" || node_str == "=" || node_str == "<" || node_str == ">" || node_str == "<=" || node_str == ">=" || node_str == "AND" || node_str == "OR" || node_str == "IS") {

            if (node_str == "NOT") {
                curNode->value = {"", "null"};
                curNode = curNode->parent;
                curNode->boolFunc = Negation;
            } else if (node_str == "=" || node_str == "IS") {
                curNode->func = Eq;
            } else if (node_str == ">") {
                curNode->func = MoreThan;
            } else if (node_str == "<") {
                curNode->func = LessThan;
            } else if (node_str == ">=") {
                curNode->func = MoreEqThan;
            } else if (node_str == "<=") {
                curNode->func = LessEqThan;
            } else if (node_str == "AND") {
                curNode->boolFunc = Conjunction;
            } else if (node_str == "OR") {
                curNode->boolFunc = Disjunction;
            }
            curNode->operr = node_str;
            std::shared_ptr<Node> node(new Node);
            curNode->right = node;
            node->parent = curNode;
            curNode = node;
        } else {
            std::pair<std::string, std::string> pair_table_key = GetTableAndKey(node_str);///{tableName, column}
            if (pair_table_key.first != "") curNode->is_key = true;
            curNode->value = pair_table_key;
            curNode = curNode->parent;
        }

    }
    return root;

}