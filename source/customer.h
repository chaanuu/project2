#include <iostream>
#include <string>
#include "sqlite3.h"

class MembershipDB {
private:
    sqlite3* db;
    std::string dbPath;

    bool executeSQL(const std::string& sql) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

public:
    MembershipDB(const std::string& dbPath) : dbPath(dbPath), db(nullptr) {}

    ~MembershipDB() {
        if (db) {
            sqlite3_close(db);
        }
    }

    bool open() {
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        std::string createTableSQL = R"(
            CREATE TABLE IF NOT EXISTS Membership (
                PhoneNumber TEXT PRIMARY KEY,
                TotalAmount INTEGER NOT NULL,
                CouponsUsed INTEGER DEFAULT 0
            );
        )";

        return executeSQL(createTableSQL);
    }

    bool executeOrder(const std::string& phoneNumber, int orderAmount) {
        sqlite3_stmt* stmt;
        std::string sql = "SELECT TotalAmount FROM Membership WHERE PhoneNumber = ?";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_text(stmt, 1, phoneNumber.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Customer exists
            int currentAmount = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);

            sql = "UPDATE Membership SET TotalAmount = TotalAmount + ? WHERE PhoneNumber = ?";
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                return false;
            }
            sqlite3_bind_int(stmt, 1, orderAmount);
            sqlite3_bind_text(stmt, 2, phoneNumber.c_str(), -1, SQLITE_STATIC);
        }
        else {
            // New customer
            sqlite3_finalize(stmt);

            sql = "INSERT INTO Membership (PhoneNumber, TotalAmount) VALUES (?, ?)";
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                return false;
            }
            sqlite3_bind_text(stmt, 1, phoneNumber.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, orderAmount);
        }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        return true;
    }

    int getCouponsAvailable(const std::string& phoneNumber) {
        sqlite3_stmt* stmt;
        std::string sql = "SELECT TotalAmount, CouponsUsed FROM Membership WHERE PhoneNumber = ?";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return 0;
        }

        sqlite3_bind_text(stmt, 1, phoneNumber.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int totalAmount = sqlite3_column_int(stmt, 0);
            int couponsUsed = sqlite3_column_int(stmt, 1);
            sqlite3_finalize(stmt);

            int couponsEarned = totalAmount / 20000;
            return couponsEarned - couponsUsed;
        }

        sqlite3_finalize(stmt);
        return 0;
    }

    bool useCoupon(const std::string& phoneNumber) {
        sqlite3_stmt* stmt;
        std::string sql = "SELECT CouponsUsed FROM Membership WHERE PhoneNumber = ?";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_text(stmt, 1, phoneNumber.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Customer exists
            int couponsUsed = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);

            // Check if there are available coupons to use
            if (getCouponsAvailable(phoneNumber) > 0) {
                sql = "UPDATE Membership SET CouponsUsed = CouponsUsed + 1 WHERE PhoneNumber = ?";
                if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                    return false;
                }
                sqlite3_bind_text(stmt, 1, phoneNumber.c_str(), -1, SQLITE_STATIC);
            }
            else {
                // No available coupons to use
                sqlite3_finalize(stmt);
                return false;
            }
        }
        else {
            // No such customer exists
            sqlite3_finalize(stmt);
            return false;
        }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        return true;
    }

};