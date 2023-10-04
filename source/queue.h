#pragma once

#include "TUI/tui.h"
#include "sell.h"

#include<vector>
#include<map>
#include<tuple>

using namespace std;

struct Queue_Struct {
	int orderNum;
	map<int, unsigned int> orderBoxMap;
};

class Queue_UI {
protected:
	tui::box orderListBox;
public:
	Queue_UI() {
        //Make Box for OrderList
		orderBox.setAppearance(tui::box_appearance::thin_line);
		orderBox.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(30, 90) });
		orderBox.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(68, 4) });
		orderBox.setTitle("OrderList Box");
		orderBox.setTitlePosition(tui::POSITION::CENTER);

        //Make text input field

    }

};

class Queue : Queue_UI {
private:
	std::vector<Queue_Struct> orders; // 주문 대기열을 저장하는 벡터

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
    // 저장된 모든 주문 정보 출력(수정 필요)
    for (const auto& order : orders) {
        std::cout << order.orderNum << " ";
        for (const auto& entry : order.orderBoxMap) {
            std::cout << entry.first << " " << entry.second << " ";
        }
        std::cout << std::endl; // 주문 사이에 개행 문자를 추가
    }

    void removeOrderAndUpdate() {
        int orderNumToDelete;
        
        //버퍼와 숫자문자열만 그리고 개행문자시 removeOrder 실행

        removeOrder(orderNumToDelete); // 주문 삭제

        // 업데이트된 주문 목록 출력
        printOrders();
    }
}
};

void removeOrderAndUpdate() {
    std::string inputStr;
    int orderNumToDelete = -1; // 기본값으로 임의의 값 설정

    // 주문 번호를 입력받는 부분
    std::cout << "Enter the order number to delete: ";
    std::cin >> inputStr;

    // 입력 문자열이 숫자로만 이루어져 있는지 확인
    bool isNumeric = true;
    for (char c : inputStr) {
        if (!std::isdigit(c)) {
            isNumeric = false;
            break; // 한 번이라도 숫자가 아닌 문자를 만나면 루프 종료
        }
    }

    if (isNumeric) {
        // 숫자로만 이루어진 문자열을 정수로 변환
        orderNumToDelete = std::stoi(inputStr);

        // 해당 주문 번호가 벡터에 있는지 확인
        bool orderFound = false;
        for (auto it = orders.begin(); it != orders.end(); ++it) {
            if (it->orderNum == orderNumToDelete) {
                // 주문 번호가 발견된 경우에만 삭제
                orders.erase(it);
                orderFound = true;
                break;
            }
        }

        if (orderFound) {
            // 업데이트된 주문 목록 출력
            printOrders();
        } else {
            std::cout << "Order not found. No action taken." << std::endl;
        }
    } else {
        std::cout << "Invalid input. Please enter a numeric order number." << std::endl;
    }
}

getStringInput();
clear();