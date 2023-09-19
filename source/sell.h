#pragma once
#include "TUI/tui.h"

struct sell {
	tui::box order_box;
	tui::text textInBox;
	tui::list list;

	void setInterface() {
		//Make Box for Order List
		order_box.setAppearance(tui::box_appearance::thin_line);
		order_box.setSizeInfo({ tui::vec2i(0, 0), tui::vec2f(30, 90) });
		order_box.setPositionInfo({ tui::vec2i(0, 0), tui::vec2f(68, 4) });
		order_box.setTitle("Order Box");
		order_box.setTitlePosition(tui::POSITION::CENTER);

		//Text in order Box
		textInBox.setSizeInfo({ {0,0}, {28,85} });
		textInBox.setPositionInfo({ {0,0}, {70,6} });
		tui::symbol_string str;
		str = "Iced Americano\t2.0\n\n";
		str += "Iced CaffeLatte\t2.5\n\n";
		str += "Iced CaffeMocha\t2.8\n\n";
		str += "Strawberry Smoothie\t4.0\n\n";
		str += "아이스 아메리카노\t4.0\n\n";
		textInBox.setText(str);

		//LIST
		list.setSizeInfo({ {0,0}, {28,28} });
		list.setPositionInfo({ {0,0}, {3,6} });
		list.setEntries({
			{"Iced Americano    2.0", tui::CHECK_STATE::CHECKED}, {"Iced CaffeLatte     2.9", tui::CHECK_STATE::NOT_CHECKED}, {"a2"}, {"a3"}, {"a4"}, {"a5"},
			{
				"Iced", tui::CHECK_STATE::NONCHECKABLE, nullptr, nullptr, nullptr,
				{
					{"Americano    2.0"}, {"CaffeLatte     2.9"}, {"b3"}, {"b4"}, {"b5"}, {"b6"},
					{
						"b7", tui::CHECK_STATE::NONCHECKABLE, nullptr, nullptr, nullptr,
						{
							{"x0", tui::CHECK_STATE::NOT_CHECKED}, {"x1"}, {"x2"}
						}
					}
				}
			},
			{"a7"}, {"a8"}, {"a9"}, {"a10"},
			{
				"a11", tui::CHECK_STATE::NOT_CHECKED, nullptr, nullptr, nullptr,
				{
					{"c1"}, {"c2"}
				}
			}
		});
	}
};