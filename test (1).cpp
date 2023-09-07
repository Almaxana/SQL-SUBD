#include "gtest/gtest.h"
#include "../ParserSQL.cpp"
#include "../Options.cpp"
#include "../ParserTree.cpp"


TEST(BDtestSuite, CreateTable) {
    MyCoolBD bd1;
    std::ostringstream os;

    Parse(os, "CREATE TABLE table1 ( id int, name varchar(50), rating double)", bd1);
    Parse(os, "CREATE TABLE table2 ( id_2 int, name_2 varchar(50), active bool)", bd1);

    Parse(os, "SELECT * FROM table1, table2", bd1);

    std::string output = "id           name         rating       active       id_2         name_2       \n";
    ASSERT_EQ(os.str(), output);

}

TEST(BDtestSuite, Where) {
    MyCoolBD bd1;
    std::ostringstream os;

    Parse(os, "CREATE TABLE table1 ( id int, name varchar(50), rating double)", bd1);


    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 1, \"A\", 10 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 2, \"B\", 4 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 3, \"C\", 7 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 4, \"D\", 3 )", bd1);


    Parse(os, "SELECT table1.id FROM table1 WHERE ( NOT ( ( table1.name = \"A\" ) OR ( table1.rating > 6 ) ) )", bd1);

    std::string output = "id        \n"
                         "2         \n"
                         "4         \n";
    ASSERT_EQ(os.str(), output);
}

TEST(BDtestSuite, Insert) {
    MyCoolBD bd1;
    std::ostringstream os;

    Parse(os, "CREATE TABLE table1 ( id int, name varchar(50), rating double)", bd1);

    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 1, \"A\", 10 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name ) VALUES ( 1, \"A\" )", bd1);

    Parse(os, "SELECT * FROM table1", bd1);

    std::string output = "id           name         rating       \n"
                         "1            \"A\"          10           \n"
                         "1            \"A\"          NULL         \n";
    ASSERT_EQ(os.str(), output);

}

TEST(BDtestSuite, Delete) {

    MyCoolBD bd1;
    std::ostringstream os;

    Parse(os, "CREATE TABLE table2 ( id_2 int, name_2 varchar(50), rating double)", bd1);

    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 1, \"A\", 2 )", bd1);
    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 2, \"B\", 10 )", bd1);
    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 3, \"M\", NULL )", bd1);
    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 4, \"D\", 7 )", bd1);
    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 5, \"E\", NULL )", bd1);

    Parse(os,"DELETE FROM table2 WHERE ( ( table2.id_2 = 2 ) OR ( table2.name_2 = \"M\" ) )", bd1);

    Parse(os, "SELECT table2.id_2, table2.name_2 FROM table2", bd1);

    std::string output = "id_2         name_2       \n"
                         "1            \"A\"          \n"
                         "4            \"D\"          \n"
                         "5            \"E\"          \n";
    ASSERT_EQ(os.str(), output);

}

TEST(BDtestSuite, Update) {
    MyCoolBD bd1;
    std::ostringstream os;

    Parse(os, "CREATE TABLE table2 ( id_2 int, name_2 varchar(50), rating double)", bd1);

    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 1, \"A\", 2 )", bd1);
    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 2, \"B\", 10 )", bd1);
    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 3, \"M\", NULL )", bd1);
    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 4, \"D\", 7 )", bd1);
    Parse(os,"INSERT INTO table2 ( id_2, name_2, rating ) VALUES ( 5, \"E\", NULL )", bd1);

    Parse(os, "UPDATE table2 SET rating = 100 WHERE ( ( table2.rating = NULL ) AND ( table2.name_2 = \"E\" ) )", bd1);

    Parse(os, "SELECT * FROM table2", bd1);

    std::string output = "id_2         name_2       rating       \n"
                         "1            \"A\"          2            \n"
                         "2            \"B\"          10           \n"
                         "3            \"M\"          NULL         \n"
                         "4            \"D\"          7            \n"
                         "5            \"E\"          100          \n";
    ASSERT_EQ(os.str(), output);

}

TEST(BDtestSuite, InnerJoin1) {

    MyCoolBD bd1;
    std::ostringstream os;

    Parse(os, "CREATE TABLE table1 ( id int, name varchar(50), rating double)", bd1);
    Parse(os, "CREATE TABLE table2 ( id_2 int, name_2 varchar(50), rating_2 double)", bd1);

    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 1, \"A\", 10 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 2, \"B\", 5 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 3, \"C\", 7 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 4, \"D\", NULL )", bd1);

    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 1, \"A\", 2 )", bd1);
    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 2, \"B\", 10 )", bd1);
    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 3, \"M\", NULL )", bd1);
    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 4, \"D\", 7 )", bd1);
    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 5, \"E\", NULL )", bd1);

    Parse(os, "SELECT * FROM table1, table2 INNER JOIN table2 ON ( table1.id = 2 )", bd1);

    std::string output = "id           name         rating       id_2         name_2       rating_2     \n"
                         "2            \"B\"          5            1            \"A\"          2            \n"
                         "2            \"B\"          5            2            \"B\"          10           \n"
                         "2            \"B\"          5            3            \"M\"          NULL         \n"
                         "2            \"B\"          5            4            \"D\"          7            \n"
                         "2            \"B\"          5            5            \"E\"          NULL         \n";

    ASSERT_EQ(os.str(), output);
}

TEST(BDtestSuite, InnerJoin2) {

    MyCoolBD bd1;
    std::ostringstream os;

    Parse(os, "CREATE TABLE table1 ( id int, name varchar(50), rating double)", bd1);
    Parse(os, "CREATE TABLE table2 ( id_2 int, name_2 varchar(50), rating_2 double)", bd1);

    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 1, \"A\", 10 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 2, \"B\", 5 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 3, \"C\", 7 )", bd1);
    Parse(os, "INSERT INTO table1 ( id, name, rating ) VALUES ( 4, \"D\", NULL )", bd1);

    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 1, \"A\", 2 )", bd1);
    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 2, \"B\", 10 )", bd1);
    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 3, \"M\", NULL )", bd1);
    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 4, \"D\", 7 )", bd1);
    Parse(os, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 5, \"E\", NULL )", bd1);

    Parse(os, "SELECT * FROM table1, table2 INNER JOIN table2 ON ( ( table1.id = 2 ) AND (table2.rating_2 = NULL ) )", bd1);

    std::string output = "id           name         rating       id_2         name_2       rating_2     \n"
                         "2            \"B\"          5            3            \"M\"          NULL         \n"
                         "2            \"B\"          5            5            \"E\"          NULL         \n";

    ASSERT_EQ(os.str(), output);
}

TEST(BDtestSuite, LeftRightJoin) {

    MyCoolBD bd1;

    std::ostringstream os1;

    Parse(os1, "CREATE TABLE table1 ( id int, name varchar(50), rating double)", bd1);
    Parse(os1, "CREATE TABLE table2 ( id_2 int, name_2 varchar(50), rating_2 double)", bd1);

    Parse(os1, "INSERT INTO table1 ( id, name, rating ) VALUES ( 1, \"A\", 10 )", bd1);
    Parse(os1, "INSERT INTO table1 ( id, name, rating ) VALUES ( 2, \"B\", 5 )", bd1);
    Parse(os1, "INSERT INTO table1 ( id, name, rating ) VALUES ( 3, \"C\", 7 )", bd1);
    Parse(os1, "INSERT INTO table1 ( id, name, rating ) VALUES ( 4, \"D\", NULL )", bd1);

    Parse(os1, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 1, \"A\", 2 )", bd1);
    Parse(os1, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 2, \"B\", 10 )", bd1);
    Parse(os1, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 3, \"M\", NULL )", bd1);
    Parse(os1, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 4, \"D\", 7 )", bd1);
    Parse(os1, "INSERT INTO table2 ( id_2, name_2, rating_2 ) VALUES ( 5, \"E\", NULL )", bd1);

    Parse(os1, "SELECT * FROM table2, table1 LEFT JOIN table1 ON ( table1.id = table2.id_2 )", bd1);

    std::string output = "id_2      name_2    rating_2  id        name      rating    \n"
                         "1         \"A\"       2         1         \"A\"       10        \n"
                         "2         \"B\"       10        2         \"B\"       5         \n"
                         "3         \"M\"       NULL      3         \"C\"       7         \n"
                         "4         \"D\"       7         4         \"D\"       NULL      \n"
                         "5         \"E\"       NULL      NULL      NULL      NULL      \n";

    ASSERT_EQ(os1.str(), output);


    std::ostringstream os2;

    output = "id        name      rating    id_2      name_2    rating_2  \n"
             "1         \"A\"       10        1         \"A\"       2         \n"
             "2         \"B\"       5         2         \"B\"       10        \n"
             "NULL      NULL      NULL      3         \"M\"       NULL      \n"
             "4         \"D\"       NULL      4         \"D\"       7         \n"
             "NULL      NULL      NULL      5         \"E\"       NULL      \n";

    Parse(os2, "SELECT * FROM table1, table2 RIGHT JOIN table2 ON ( table1.name = table2.name_2 )", bd1);

    ASSERT_EQ(os2.str(), output);
}


