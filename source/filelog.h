#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "sell.h"

auto livetime() {
    auto now = std::chrono::system_clock::now();
    //현재 시간 불러오기
    return now;
}

string changeNum(int num) {
    string temp;
    if (num < 10)
        temp = "0" + std::to_string(num); //한자리수인 경우 앞에 0을 붙임
    else
        temp = std::to_string(num);

    return temp;
}

int filelog(OrderInfo orderinfo) {
    //-------------------------------------------------------------------------------
    std::time_t time_now = std::chrono::system_clock::to_time_t(livetime());
    auto time_point = std::chrono::system_clock::to_time_t(livetime());
    struct std::tm timeinfo;
    localtime_s(&timeinfo, &time_now);

    std::string year = std::to_string(timeinfo.tm_year + 1900);
    std::string month = changeNum (timeinfo.tm_mon + 1);
    std::string day = changeNum(timeinfo.tm_mday);
    std::string hour = changeNum(timeinfo.tm_hour);
    std::string minute = changeNum(timeinfo.tm_min);
    std::string second = changeNum(timeinfo.tm_sec);


    std::string date = year + month + day;
    std::string time = hour + minute + second;
    //--------------------------------------------------------------------------여기까지 현재시간 저장 코드
        
    std::string FileMake = date + ".csv"; //파일로그 파일경로 수정필요 앞에 문자열 경로 추가"..\\..\\" 

    std::ofstream outputFile(FileMake, std::ios::app); //파일생성, 파일작성
    if (!outputFile) {
        std::cerr << "파일을 열 수 없습니다." << std::endl;
        return 1;
    }
    else {
        string orderdata;
        int totalPrice =0;
        for (auto iter = orderinfo.information.begin(); iter != orderinfo.information.end(); iter++) {
            orderdata += "," + to_string(iter->first) + "," + to_string(iter->second);
            totalPrice += getMenuPrice(iter->first) * iter->second;
        }
        outputFile << orderinfo.number << "," << date << "," << time  <<"," << to_string(totalPrice) << orderdata << "\n";


        
        outputFile.close();
    }

    return 0;
}
