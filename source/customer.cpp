#include"customer.h"

using namespace std;

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
        // ù ��° �ุ ó���ϰ� ���� ȣ���� ����
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
        now += days * 24 * 3600; // �Ϸ�� 86400�� (24�ð� * 60�� * 60��)
        tm* ltm = localtime(&now);
        return format_date(*ltm);
    }

    string get_date_minus_days(int days) {
        time_t now = time(nullptr);
        now -= days * 24 * 3600; // �Ϸ�� 86400�� (24�ð� * 60�� * 60��)
        tm* ltm = localtime(&now);
        return format_date(*ltm);
    }

public:
    DB(const std::string& dbPath) : dbPath(dbPath), db(nullptr) {
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
            cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
            exit(0);
        }
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

        // ���� �ֹ� �Ѿ� ���ϱ�
        for (iter = record.begin(); iter != record.end(); iter++) {
            total += iter->amount;
        }

        // ���� �߰��ϱ�
        coupon_count = total / 10000;
        sql = "INSERT INTO couponBook (hp, exp) VALUES (" + hpValue + ", " + seven_days_later +
            ")";
        for (int i = 0; i < coupon_count; i++) {
            if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
                cerr << "SQL error: " << errMsg << endl;
                sqlite3_free(errMsg);
            }
        }

        // ������ Entry�� ������ Entry�� ã��
        for (iter = record.begin(); iter != record.end(); iter++) {
            if ((total - iter->amount) < 0) {
                editEntry = *iter;
                editEntry.amount -= total;
                break;
            }
            deleteEntries.emplace_back(*iter);
            total -= iter->amount;
        }

        // ������ Entry �����ϱ�
        for (iter = deleteEntries.begin(); iter != deleteEntries.end(); iter++) {
            sql = "DELETE FROM orderLog WHERE id = " + to_string(iter->id);
            if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
                cerr << "SQL error: " << errMsg << endl;
                sqlite3_free(errMsg);
            }
        }

        // ������ Entry �����ϱ�
        sql = "UPDATE orderLog SET amount " + to_string(editEntry.amount) +
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
        struct couponEntry used_coupon;

        // �ش� ��ȭ��ȣ�� ���� ���� ���� ID�� ã���ϴ�.
        string sql = "SELECT MIN(id) FROM couponBook WHERE hp = '" + hpValue + "'";
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), callback3, &used_coupon, &errMsg) !=
            SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }

        // ã�� ������ �����մϴ�.
        sql = "DELETE FROM couponBook WHERE id = " + to_string(used_coupon.id);
        if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
    }

    void checkDB() {
        string sevenDaysAgoDate = get_date_minus_days(7);  // 7�� �� ��¥ ���ڿ��� ��ȯ

        // orderLog ���̺��� 7�Ϻ��� ������ ��� ����
        string sql =
            "DELETE FROM orderLog WHERE date <= '" + sevenDaysAgoDate + "'";
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }

        // couponBook ���̺��� 7�Ϻ��� ������ ��� ����
        sql = "DELETE FROM couponBook WHERE exp <= '" + sevenDaysAgoDate + "'";
        if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
    }
};