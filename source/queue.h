#pragma once

#include "TUI/tui.h"
#include "sell.h"
#include "menu.h"

#include<vector>
#include<map>
#include<tuple>

using namespace std;

class Queue_UI {
protected:
	tui::box queueBox1;
	tui::box queueBox2;
	tui::box queueBox3;
	tui::box queueBox4;
	tui::box queueBox5;

	tui::text textInBox1;
	tui::text textInBox2;
	tui::text textInBox3;
	tui::text textInBox4;
	tui::text textInBox5;

	tui::text guide;

	tui::symbol_string queueBoxText;
	tui::symbol_string guideText;


public:
		Queue_UI() {
		//Text to guide this Tab
		guideText << tui::COLOR::LIGHTBLUE << "DELETE + [1-5]";
		guideText += " to delete order gone out";
		guide.setSizeInfo({ {0,2}, {50,1} });
		guide.setPositionInfo({ {0,0}, {5,92} });
		guide.setText(guideText);

		//Make Box1
		queueBox1.setAppearance(tui::box_appearance::thin_line);
		queueBox1.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(90, 15) });
		queueBox1.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(3, 3) });
		queueBox1.setTitle("1");
		queueBox1.setTitlePosition(tui::POSITION::CENTER);

		//Make Box2
		queueBox2.setAppearance(tui::box_appearance::thin_line);
		queueBox2.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(90, 15) });
		queueBox2.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(3, 21) });
		queueBox2.setTitle("2");
		queueBox2.setTitlePosition(tui::POSITION::CENTER);

		//Make Box3
		queueBox3.setAppearance(tui::box_appearance::thin_line);
		queueBox3.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(90, 15) });
		queueBox3.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(3, 39) });
		queueBox3.setTitle("3");
		queueBox3.setTitlePosition(tui::POSITION::CENTER);

		//Make Box4
		queueBox4.setAppearance(tui::box_appearance::thin_line);
		queueBox4.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(90, 15) });
		queueBox4.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(3, 57) });
		queueBox4.setTitle("4");
		queueBox4.setTitlePosition(tui::POSITION::CENTER);

		//Make Box5
		queueBox5.setAppearance(tui::box_appearance::thin_line);
		queueBox5.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(90, 15) });
		queueBox5.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(3, 75) });
		queueBox5.setTitle("5");
		queueBox5.setTitlePosition(tui::POSITION::CENTER);

		//Text in Box1
		textInBox1.setSizeInfo({ {0,0}, {88,14} });
		textInBox1.setPositionInfo({ {0,1}, {4,4} });
		

		//Text in Box2

		textInBox2.setSizeInfo({ {0,0}, {88,14} });
		textInBox2.setPositionInfo({ {0,1}, {4,20} });
	

		//Text in Box3
		textInBox3.setSizeInfo({ {0,0}, {88,14} });
		textInBox3.setPositionInfo({ {0,1}, {4,40} });
		

		//Text in Box4
		textInBox4.setSizeInfo({ {0,0}, {88,14} });
		textInBox4.setPositionInfo({ {0,1}, {4,58} });
	

		//Text in Box5
		textInBox5.setSizeInfo({ {0,0}, {88,14} });
		textInBox5.setPositionInfo({ {0,1}, {4,76} });
		


	}
};

class Queue :Queue_UI {
private:
	std::vector<OrderInfo> QueueInfo;

public:
	Queue() {}

	void drawUI() {
		tui::output::draw(queueBox1);
		tui::output::draw(queueBox2);
		tui::output::draw(queueBox3);
		tui::output::draw(queueBox4);
		tui::output::draw(queueBox5);

		tui::output::draw(textInBox1);
		tui::output::draw(textInBox2);
		tui::output::draw(textInBox3);
		tui::output::draw(textInBox4);
		tui::output::draw(textInBox5);

		tui::output::draw(guide);

	}

	void addOrder(const OrderInfo& order) {
		QueueInfo.push_back(order);
	}

	void printQueue() {
		
		int i = 0;
		tui::symbol_string currentLine;
		// 출력할 주문 정보가 있을 경우
		for (; i < QueueInfo.size() && i < 5; i++) {
			
			std::string info = std::to_string(QueueInfo[i].number)+ " ";

			for (const auto& pair : QueueInfo[i].information) {
				int menuId = pair.first;
				unsigned int quantity = pair.second;
				std::string menuName = getMenuName(menuId);
				info += menuName + " " + std::to_string(quantity) + " ";
			}

			currentLine = info;
			queueBoxText << tui::COLOR::WHITE << currentLine;

			switch (i) {
			case 0:
				textInBox1.setText(queueBoxText);
				break;
			case 1:
				textInBox2.setText(queueBoxText);
				break;
			case 2:
				textInBox3.setText(queueBoxText);
				break;
			case 3:
				textInBox4.setText(queueBoxText);
				break;
			case 4:
				textInBox5.setText(queueBoxText);
				break;
			}
			
			queueBoxText.clear();
		}
		

		// 주문 큐에 있는 주문 수가 5개 미만인 경우, 나머지 박스를 비움
		for (; i < 5; i++) {
			switch (i) {
			case 0:
				textInBox1.setText(queueBoxText);
				break;
			case 1:
				textInBox2.setText(queueBoxText);;
				break;
			case 2:
				textInBox3.setText(queueBoxText);
				break;
			case 3:
				textInBox4.setText(queueBoxText);
				break;
			case 4:
				textInBox5.setText(queueBoxText);
				break;
			}
		}
	
		
	}

	void removeQueue() {
		

		if (tui::input::isKeyPressed(tui::input::KEY::F1)) {
			removeQueue1();
		}
		if (tui::input::isKeyPressed(tui::input::KEY::F2)) {
			removeQueue2();
		}
		if (tui::input::isKeyPressed(tui::input::KEY::F3)) {
			removeQueue3();
		}
		if (tui::input::isKeyPressed(tui::input::KEY::F4)) {
			removeQueue4();
		}
		if (tui::input::isKeyPressed(tui::input::KEY::F5)) {
			removeQueue5();
		}
	}
	

	void removeQueue1() {
		if (QueueInfo.size() > 0) {
			QueueInfo.erase(QueueInfo.begin());
		}
	}

	void removeQueue2() {
		if (QueueInfo.size() > 1) {
			QueueInfo.erase(QueueInfo.begin() + 1);
		}
	}

	void removeQueue3() {
		if (QueueInfo.size() > 2) {
			QueueInfo.erase(QueueInfo.begin() + 2);
		}
	}

	void removeQueue4() {
		if (QueueInfo.size() > 3) {
			QueueInfo.erase(QueueInfo.begin() + 3);
		}
	}

	void removeQueue5() {
		if (QueueInfo.size() > 4) {
			QueueInfo.erase(QueueInfo.begin() + 4);
		}
	}




};
