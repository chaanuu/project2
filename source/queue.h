#pragma once

#include"sell.h"

#include<vector>
#include<map>
#include<tuple>

using namespace std;

struct Queue_Struct {
	int orderNum;
	map<int, unsigned int> orderBoxMap;
	
	Queue_Struct(int num) : orderNum(num) {}
};

class Queue_UI {
protected:
	
public:
	Queue_UI() {
	}
};



class Queue : Queue_UI {
private:
	std::vector<Queue_Struct> orders; // 주문 정보를 저장하는 벡터

public:
	Queue() {
	}

	void removeOrder(int orderNum) {
        // 주문 번호에 해당하는 주문을 삭제
        for (auto it = orders.begin(); it != orders.end(); ++it) {
            if (it->orderNum == orderNum) {
                orders.erase(it);
                break; // 삭제 후 반복문 종료
            }
        }
    }

    void printOrders() {
    // 저장된 모든 주문 정보 출력
    for (const auto& order : orders) {
        std::cout << order.orderNum << " ";
        for (const auto& entry : order.orderBoxMap) {
            std::cout << entry.first << " " << entry.second << " ";
        }
        std::cout << std::endl; // 주문 사이에 개행 문자를 추가
    }
}
};