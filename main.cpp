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

  orderEntry(int id, string hp, int amount, string date)
      : id(id), hp(hp), amount(amount), date(date) {}
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

  string yyyymmdd() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year;
    ss << std::setw(2) << std::setfill('0') << 1 + ltm->tm_mon;
    ss << std::setw(2) << std::setfill('0') << ltm->tm_mday;
    return ss.str();
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

  unsigned int getCouponsAvailable(const string hpValue) {
    unsigned int coupon_count = 0;
    string currentDate = yyyymmdd(); // 현재 날짜 가져오기

    // 7일 전 날짜 계산
    tm ltm = {};
    strptime(currentDate.c_str(), "%Y%m%d", &ltm); // 문자열을 tm 구조체로 변환

   
    ltm.tm_mday -= 7;
    mktime(&ltm); 

    stringstream ss;
    ss << 1900 + ltm.tm_year;
    ss << std::setw(2) << std::setfill('0') << 1 + ltm.tm_mon;
    ss << std::setw(2) << std::setfill('0') << ltm.tm_mday;
    string sevenDaysAgoDate = ss.str(); // 7일 전 날짜 문자열로 변환

    string sql = "SELECT COUNT(*) FROM couponBook WHERE hp = '" + hpValue + "' AND exp >= '" + sevenDaysAgoDate + "'";
    char* errMsg = nullptr;

    if (sqlite3_exec(db, sql.c_str(), callback, &coupon_count, &errMsg) !=
        SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    return coupon_count;
}

  unsigned int makeCoupon(const string hpValue) {
    unsigned int coupon_count = 0;
    unsigned int total = 0;

    vector<orderEntry> record = getRecordsByHp(hpValue);
    vector<orderEntry>::iterator iter;
    vector<orderEntry> deleteEntries;
    struct orderEntry editEntry;

    string sql;
    string date = yyyymmdd();
    char* errMsg = nullptr;

    // 현재 주문 총액 구하기
    for (iter = record.begin(); iter != record.end(); iter++) {
      total += iter->amount;
    }

    // 쿠폰 추가하기
    coupon_count = total / 10000;
    sql = "INSERT INTO couponBook (hp, exp) VALUES (" + hpValue + ", " + date +
          ")";
    for (int i = 0; i < coupon_count; i++) {
      if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
      }
    }

    // 삭제할 Entry와 수정할 Entry를 찾기
    for (iter = record.begin(); iter != record.end(); iter++) {
      if ((total - iter->amount) < 0) {
        editEntry = *iter;
        editEntry.amount -= total;
        break;
      }
      deleteEntries.emplace_back(*iter);
      total -= iter->amount;
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
    sql = "UPDATE orderLog SET amount " + to_string(editEntry.amount) +
          " WHERE id = " + to_string(editEntry.id);

    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) != SQLITE_OK) {
      cerr << "SQL error: " << errMsg << endl;
      sqlite3_free(errMsg);
    }

    return coupon_count;
  }
};

int main() {
  DB db("database.db");
  return 0;
}
