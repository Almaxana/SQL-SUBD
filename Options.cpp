#pragma once

#include "Options.h"
#include "ParserTree.h"

#include<iostream>
#include<variant>
#include<vector>
#include<map>
#include<iomanip>
#include<fstream>


struct S;

class MyCoolBD;

void MyCoolBD::CreateTable(const std::string& tableName_, std::map<std::string, std::tuple<std::string, bool, std::pair<std::string, std::string>>>& keys) {///type, is_PK, FK
    std::map<std::string, S> columns;
    for (auto key : keys) {
        std::pair<std::string, S> column = {key.first, S(std::get<0>(key.second))};
        column.second.is_primary_key = std::get<1>(key.second);
        column.second.is_foreign_key = std::get<2>(key.second);
        columns.insert(column);
    }
    BD.insert({tableName_, columns});
    table_sizes[tableName_] = 0;
}


void MyCoolBD::Insert(const std::string& destination_table, std::vector<std::string>& keys,  std::vector<std::string>& values_) {
    for (auto& column : BD[destination_table]) {
        column.second.values.push_back("NULL");
    }
    for (size_t i = 0; i < keys.size(); ++i) {
        if (BD[destination_table][keys[i]].is_primary_key) {
            for (auto it : BD[destination_table][keys[i]].values) {
                if (it == values_[i]) {
                    std::cout<<"Cannot INSERT. Uniqueness of PK failed\n\n\n";
                    for (auto& column : BD[destination_table]) {
                        column.second.values.erase(column.second.values.end() - 1);
                    }
                    return;
                }
            }
        }

        BD[destination_table][keys[i]].values[BD[destination_table][keys[i]].values.size() - 1] = values_[i];
    }

    table_sizes[destination_table]++;
}

void MyCoolBD::Update(const std::string& destination_table, std::vector<std::string>& keys,  std::vector<std::string>& values_, std::shared_ptr<Node> Where) {
    for (size_t i = 0; i < table_sizes[destination_table]; ++i) {
        if (accept(Where, *this, i)) {
            for (size_t j = 0; j < keys. size(); ++j) {
                BD[destination_table][keys[j]].values[i] = values_[j];
            }
        }

    }
}

void MyCoolBD::Delete(const std::string& destination_table, std::shared_ptr<Node> Where) {
    uint64_t new_table_size = table_sizes[destination_table];
    for (int64_t i = table_sizes[destination_table] - 1; i >=0; --i) {
        if (accept(Where, *this, i)) {
            --new_table_size;
            for (auto& column : BD[destination_table]) {
                column.second.values.erase(column.second.values.begin() + i);
            }
        }
    }
    table_sizes[destination_table] = new_table_size;
}



void MyCoolBD::SelectFrom(std::vector<std::pair<std::string, std::string>>& what_columns) {
    size_t rows_number = 0;
    for (auto key : what_columns) {
        rows_number = std::max(rows_number, table_sizes[key.first]);
        std::cout<<std::setw(13)<<std::left<<key.second;
    }
    std::cout<<"\n";
    for (size_t i = 0; i < rows_number; ++i) {
        for (auto key : what_columns) {
            if (i >= BD[key.first][key.second].values.size()) std::cout<<std::setw(13)<<std::left<<"NULL";
            else std::cout<<std::setw(13)<<std::left<<BD[key.first][key.second].values[i];
        }
        std::cout<<"\n";
    }
}

void MyCoolBD::SelectFromWhere(std::vector<std::pair<std::string, std::string>>& what_columns, std::shared_ptr<Node> Where) {
    size_t rows_number = 0;
    for (auto key : what_columns) {
        rows_number = std::max(rows_number, table_sizes[key.first]);
        std::cout<<std::setw(10)<<std::left<<key.second<<" ";
    }
    std::cout<<"\n";

    for (size_t i = 0; i < rows_number; ++i) {
        if (accept(Where, *this, i)) {
            for (auto key : what_columns) {
                if (i >= BD[key.first][key.second].values.size()) std::cout<<std::setw(10)<<std::left<<"NULL";
                else std::cout<<std::setw(10)<<std::left<<BD[key.first][key.second].values[i];
            }
            std::cout<<"\n";
        }

    }
}

void MyCoolBD::SelectFromInnerJoin(std::vector<std::pair<std::string, std::string>>& what_columns,
                                   std::shared_ptr<Node> Join, const std::string& tableName1, const std::string& tableName2) {

    for (auto key : what_columns) {
        std::cout<<std::setw(10)<<std::left<<key.second;
    }
    std::cout<<"\n";

    for (size_t i = 0; i < table_sizes[tableName1]; ++i) {
        for (size_t j = 0; j < table_sizes[tableName2]; ++j) {
            if (acceptForTwoRows(Join, *this, i, j, tableName1, tableName2)) {
                for (auto key : what_columns) {
                    size_t index;
                    if (key.first == tableName1) index = i;
                    else index = j;
                    std::cout<<std::setw(10)<<std::left<<BD[key.first][key.second].values[index];
                }
                std::cout<<"\n";
            }
        }
    }

}

void MyCoolBD::SelectFromLeftJoin(std::vector<std::pair<std::string, std::string>>& what_columns, std::shared_ptr<Node> Join,
                                  const std::string& tableName1, const std::string& tableName2) {

    for (auto key : what_columns) {
        std::cout<<std::setw(10)<<std::left<<key.second;
    }
    std::cout<<"\n";

    for (size_t i = 0; i < table_sizes[tableName1]; ++i) {
        bool find_accept = false;
        for (size_t j = 0; j < table_sizes[tableName2]; ++j) {
            if (acceptForTwoRows(Join, *this, i, j, tableName1, tableName2)) {
                find_accept = true;
                for (auto key : what_columns) {
                    size_t index;
                    if (key.first == tableName1) index = i;
                    else index = j;
                    std::cout<<std::setw(10)<<std::left<<BD[key.first][key.second].values[index];
                }
                std::cout<<"\n";
            }

        }
        if (!find_accept) {
            for (auto key : what_columns) {
                if (key.first == tableName1) std::cout<<std::setw(10)<<std::left<<BD[key.first][key.second].values[i];
                else std::cout<<std::setw(10)<<std::left<<"NULL";

            }
            std::cout<<"\n";
        }
    }

}

void MyCoolBD::SelectFromRightJoin(std::vector<std::pair<std::string, std::string>>& what_columns, std::shared_ptr<Node> Join,
                                   const std::string& tableName1, const std::string& tableName2) {
    SelectFromLeftJoin(what_columns, Join, tableName2, tableName1);
}

void MyCoolBD::Print(const std::string& tableName) {
    for (auto str : BD[tableName]) {
        std::cout<<std::setw(10)<<std::left<<str.first<<" ";
    }
    std::cout<<"\n";
    for (size_t i = 0; i < table_sizes[tableName]; ++i) {
        for (auto column : BD[tableName]) {
            std::cout<<std::setw(10)<<std::left<<column.second.values[i]<<" ";
        }
        std::cout<<"\n";
    }
}

void MyCoolBD::DropTable(const std::string& tableName_) {
    BD.erase(tableName_);
    table_sizes.erase(tableName_);
}

std::vector<std::string> MyCoolBD::GetTableKeys(const std::string& tableName) {
    std::vector<std::string> table_keys;
    for (auto key : BD[tableName]) {
        table_keys.push_back(key.first);
    }
    return table_keys;
}

void MyCoolBD::SaveIntoFile(const std::string& path) {
    std::ofstream out (path.c_str(), std::ios::out);

    if (out.fail()) {
        std::cout<<"Cannot open "<<path<<" for saving bd\n";
        return;
    }

    for (auto table : BD) {
        std::string tableName = table.first;

        out<<"CREATE TABLE "<<tableName<<" (";
        size_t j = 0;
        for (auto key : BD[tableName]) {
            if (j != 0) out<<",";
            out<<" "<<key.first<<" "<<key.second.type;
            ++j;
        }
        out<<" )\n";


        std::string insert_template = "INSERT INTO " + tableName + " ( ";
        j = 0;
        for (auto column : BD[tableName]) {
            if (j != 0) insert_template = insert_template + ",";
            insert_template = insert_template + column.first + " ";
            ++j;
        }
        insert_template = insert_template + ") VALUES (";

        for (size_t i = 0; i < table_sizes[tableName]; ++i) {
            j = 0;
            out<<insert_template;
            for (auto column : BD[tableName]) {
                if (j != 0) out<<",";
                out<<" "<<column.second.values[i];
                ++j;
            }
            out<<" )\n";
        }

    }
}