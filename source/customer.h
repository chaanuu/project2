#pragma once
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "sqlite3.h"

using namespace std;

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
    std::string dbPath;

    static int callback(void* data, int argc, char** argv, char** azColName) {
        auto* records = static_cast<vector<orderEntry>*>(data);
        records->emplace_back(atoi(argv[0]), argv[1], atoi(argv[2]), argv[3]);
        return 0;
    }

    static int callback2(void* data, int argc, char** argv, char** azColName) {
        auto* records = static_cast<vector<couponEntry>*>(data);
        records->emplace_back(atoi(argv[0]), argv[1], argv[2]);
        return 0;
    }

    static int callback3(void* data, int argc, char** argv, char** azColName) {
        auto* record = static_cast<couponEntry*>(data);
        // 첫 번째 행만 처리하고 이후 호출은 무시
        record->id = atoi(argv[0]);
        record->hp = argv[1];
        record->exp = argv[2];
        return 0;
    }

    string format_date(const tm& date) {
        stringstream ss;
        ss << 1900 + date.tm_year;
        ss << setw(2) << setfill('0') << 1 + date.tm_mon;
        ss << setw(2) << setfill('0') << date.tm_mday;
        return ss.str();
    }

    string get_current_date() {
        time_t now = time(nullptr);
        tm* ltm = localtime(&now);
        return format_date(*ltm);
    }

    string get_date_plus_days(int days) {
        time_t now = time(nullptr);
        now += days * 24 * 3600; // 하루는 86400초 (24시간 * 60분 * 60초)
        tm* ltm = localtime(&now);
        return format_date(*ltm);
    }

    string get_date_minus_days(int days) {
        time_t now = time(nullptr);
        now -= days * 24 * 3600; // 하루는 86400초 (24시간 * 60분 * 60초)
        tm* ltm = localtime(&now);
        return format_date(*ltm);
    }

public:
    DB(const std::string& dbPath) : dbPath(dbPath), db(nullptr) {
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
            cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
            exit(0);
        }
        checkDB();
    }

    ~DB() {
        if (db) {
            sqlite3_close(db);
        }
    }

    vector<orderEntry> getRecordsByHp(const string hpValue) {
        vector<orderEntry> records;
        string sql = "SELECT * FROM orderLog WHERE hp = '" + hpValue + "'";
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), callback, &records, &errMsg) !=
            SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
        return records;
    }

    unsigned int makeCoupon(const string hpValue) {
        unsigned int coupon_count = 0;
        unsigned int total = 0;

        vector<orderEntry> record = getRecordsByHp(hpValue);
        vector<orderEntry>::iterator iter;
        vector<orderEntry> deleteEntries;
        struct orderEntry editEntry;

        string sql;
        string date = get_current_date();
        string seven_days_later = get_date_plus_days(7);
        char* errMsg = nullptr;

        // 현재 주문 총액 구하기
        for (iter = record.begin(); iter != record.end(); iter++) {
            total += iter->amount;
        }

        // 쿠폰 추가하기
        coupon_count = total / 10000;
        sql = "INSERT INTO couponBook (hp, exp) VALUES ('" + hpValue + "', '" + seven_days_later + "')";
        for (int i = 0; i < coupon_count; i++) {
            if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
                cerr << "SQL error: " << errMsg << endl;
                sqlite3_free(errMsg);
            }
        }

        int coupon_amount = coupon_count * 10000;
        // 삭제할 Entry와 수정할 Entry를 찾기
        for (iter = record.begin(); iter != record.end(); iter++) {
            if ((coupon_amount - iter->amount) < 0) {
                editEntry = *iter;
                editEntry.amount -= coupon_amount;
                break;
            }
            deleteEntries.emplace_back(*iter);
            coupon_amount -= iter->amount;
        }

        // 삭제할 Entry 삭제하기
        for (iter = deleteEntries.begin(); iter != deleteEntries.end(); iter++) {
            sql = "DELETE FROM orderLog WHERE id = " + to_string(iter->id);
            if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
                cerr << "SQL error: " << errMsg << endl;
                sqlite3_free(errMsg);
            }
        }

        // 수정할 Entry 수정하기
        sql = "UPDATE orderLog SET amount = " + to_string(editEntry.amount) +
            " WHERE id = " + to_string(editEntry.id);

        if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }

        return coupon_count;
    }

    vector<couponEntry> getCouponsAvailable(const string hpValue) {
        vector<couponEntry> coupons;
        string sql = "SELECT * FROM couponBook WHERE hp = '" + hpValue + "'";
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), callback2, &coupons, &errMsg) !=
            SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
        return coupons;
    }

    void useCoupon(const string hpValue) {
        vector<couponEntry> usable_coupon = getCouponsAvailable(hpValue);
        char* errMsg = nullptr;
        // 찾은 쿠폰을 삭제합니다.
        string sql = "DELETE FROM couponBook WHERE id = " + to_string(usable_coupon[0].id);
        if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
            system("pause");
        }
    }

    void checkDB() {

        string sevenDaysAgoDate = get_date_minus_days(7);  // 7일 전 날짜 문자열로 변환
        
        // orderLog 테이블에서 7일보다 이전인 기록 삭제
        string sql =
            "DELETE FROM orderLog WHERE date <= '" + sevenDaysAgoDate + "'";
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
            system("pause");
        }
        
        // couponBook 테이블에서 7일보다 이전인 기록 삭제
        sql = "DELETE FROM couponBook WHERE exp <= '" + sevenDaysAgoDate + "'";
        if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
            system("pause");
        }
    }

    void appendOrder(string hp, int amount) {

        string crrdate = get_current_date();
        char* errMsg = nullptr;
        std::string sql = "INSERT INTO orderLog (hp, amount, date) VALUES ('" + hp + "', " + to_string(amount) + ", '" + crrdate + "');";
        if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
            system("pause");
        }
        makeCoupon(hp);
    }
};

/*
class DB {

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

public:
    DB(const string& dbPath);
    ~DB();

    vector<orderEntry> getRecordsByHp(const string hpValue);
    unsigned int makeCoupon(const string hpValue); // main.cpp
    vector<couponEntry> getCouponsAvailable(const string hpValue);  
    void useCoupon(const string hpValue); //main.cpp
    void checkDB(); 
    void appendOrder(string hp, int amount);

};
*/