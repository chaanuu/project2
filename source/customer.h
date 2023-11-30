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
public:
    DB(const string& dbPath);
    ~DB();

    vector<orderEntry> getRecordsByHp(const string hpValue);
    unsigned int makeCoupon(const string hpValue);
    vector<couponEntry> getCouponsAvailable(const string hpValue);
    void useCoupon(const string hpValue);
    void checkDB();

private:
    sqlite3* db;
    string dbPath;

    static int callback(void* data, int argc, char** argv, char** azColName);
    static int callback2(void* data, int argc, char** argv, char** azColName);
    static int callback3(void* data, int argc, char** argv, char** azColName);

    string format_date(const tm& date);
    string get_current_date();
    string get_date_plus_days(int days);
    string get_date_minus_days(int days);
};