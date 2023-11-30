#pragma once
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "sqlite3.h"

struct orderEntry {
    int id;
    string hp;
    int amount;
    string date;

    orderEntry() : id(0), hp(""), amount(0), date("") {}

    orderEntry(int id, string hp, int amount, string date)
        : id(id), hp(hp), amount(amount), date(date) {}
};

struct couponEntry {
    int id;
    string hp;
    string exp;

    couponEntry() : id(0), hp(""), exp("") {}

    couponEntry(int id, string hp, string exp) : id(id), hp(hp), exp(exp) {}
};

class DB {
private:
    sqlite3* db;
    string dbPath;

    static int callback(void* data, int argc, char** argv, char** azColName);
    static int callback2(void* data, int argc, char** argv, char** azColName);
    static int callback3(void* data, int argc, char** argv, char** azColName);

    string yyyymmdd();

public:
    DB(const string& dbPath);
    ~DB();

    vector<orderEntry> getRecordsByHp(const string hpValue);
    unsigned int makeCoupon(const string hpValue);
    vector<couponEntry> getCouponsAvailable(const string hpValue);
    void useCoupon(const string hpValue);
    void checkDB();
};