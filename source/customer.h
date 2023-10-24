#pragma once

#include "sqlite3.h"

#include <iostream>
#include <string>

class DatabaseManager {
private:
    const char* DB_FILE;
    sqlite3* db = nullptr;

public:
    DatabaseManager() : DB_FILE("customer.db") {
        if (!openDatabase()) {
            exit(0);
        }
        else {
            printf("customer.db ���� ���� \n");
        }
    }

    ~DatabaseManager() {
        closeDatabase();
    }

    bool openDatabase() {
        int result = sqlite3_open(DB_FILE, &db);
        if (result != SQLITE_OK) {
            std::cerr << "SQLite3 ���� ����: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        return true;
    }

    void closeDatabase() {
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    bool createTable() {
        const char* createTableSQL = "CREATE TABLE IF NOT EXISTS orders ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "phone_number TEXT, "
            "order_count INTEGER)";
        char* errorMessage = nullptr;
        int result = sqlite3_exec(db, createTableSQL, 0, 0, &errorMessage);

        if (result != SQLITE_OK) {
            std::cerr << "���̺� ���� ����: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
            return false;
        }
        return true;
    }

    bool processOrder(const std::string& phoneNumber) {
        std::string query = "SELECT * FROM orders WHERE phone_number = ?";
        sqlite3_stmt* stmt = nullptr;

        int result = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
        if (result != SQLITE_OK) {
            std::cerr << "���� �غ� ����: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, phoneNumber.c_str(), -1, SQLITE_STATIC);

        result = sqlite3_step(stmt);
        if (result == SQLITE_ROW) {
            // �̹� �ֹ��� �����ϴ� ���, order_count�� ������Ŵ
            int currentCount = sqlite3_column_int(stmt, 2);
            currentCount++;
            sqlite3_finalize(stmt);

            query = "UPDATE orders SET order_count = ? WHERE phone_number = ?";
            result = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
            if (result != SQLITE_OK) {
                std::cerr << "���� �غ� ����: " << sqlite3_errmsg(db) << std::endl;
                return false;
            }

            sqlite3_bind_int(stmt, 1, currentCount);
            sqlite3_bind_text(stmt, 2, phoneNumber.c_str(), -1, SQLITE_STATIC);

            result = sqlite3_step(stmt);
        }
        else if (result == SQLITE_DONE) {
            // �ֹ��� �������� �ʴ� ���, ���ο� ���� ����
            sqlite3_finalize(stmt);

            query = "INSERT INTO orders (phone_number, order_count) VALUES (?, 1)";
            result = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
            if (result != SQLITE_OK) {
                std::cerr << "���� �غ� ����: " << sqlite3_errmsg(db) << std::endl;
                return false;
            }

            sqlite3_bind_text(stmt, 1, phoneNumber.c_str(), -1, SQLITE_STATIC);

            result = sqlite3_step(stmt);
        }

        sqlite3_finalize(stmt);
        return (result == SQLITE_DONE);
    }


};