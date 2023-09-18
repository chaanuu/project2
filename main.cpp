#include "TUI/tui.h"
#include <cmath>

int main()
{

	//BOX
	tui::box main_box({ {0,0}, {100,100} });
	main_box.setTitle("TUI test for Project");
	main_box.setTitlePosition(tui::POSITION::END);

	//TABS
	tui::tabs<tui::DIRECTION::HORIZONTAL> tabs({ 0,33 });
	tabs.setTabs({ "SELL", "LIST", "REPORT", "ADMIN" });
	tabs.key_next = tui::input::KEY::CTRL_RIGHT;
	tabs.key_prev = tui::input::KEY::CTRL_LEFT;
	tabs.activate();

	tui::symbol_string t_desc;
	t_desc += "next tab: ";
	t_desc << tui::COLOR::GREEN << tui::input::getKeyName(tabs.key_next);
	t_desc += ", prev tab: ";
	t_desc << tui::COLOR::GREEN << tui::input::getKeyName(tabs.key_prev);
	tui::surface tabs_keys(t_desc);
	tabs_keys.setPositionInfo({ {-1,0}, {0,0}, {tui::POSITION::END, tui::POSITION::END} });

	//BOX for order list
	tui::surface_size order_box_size (tui::vec2i(0, 0), tui::vec2f(30, 90));
	tui::surface_position order_box_position(tui::vec2i(0, 0), tui::vec2f(68, 4));

	tui::box order_box;
	order_box.setAppearance(tui::box_appearance::thin_line);
	order_box.setSizeInfo(order_box_size);
	order_box.setPositionInfo(order_box_position);
	order_box.setTitle("Order Box");
	order_box.setTitlePosition(tui::POSITION::CENTER);

	//TEXT
	tui::text text({ {0,0}, {100,100} });
	text.setPositionInfo({ {5,3}, {0,0}, {tui::POSITION::BEGIN, tui::POSITION::BEGIN} });
	tui::symbol_string str;
	//for (char i = 33; i < 127; i++) { str.push_back(i); }
	str = "Iced Americano\t2.0\n\n";
	str += "Iced CaffeLatte\t2.5\n\n";
	str += "Iced CaffeMocha\t2.8\n\n";
	str += "Strawberry Smoothie\t4.0\n\n";
	str += "아이스 아메리카노\t4.0\n\n";
	text.setText(str);

	//TEXT2
	tui::text textA({ {0,0}, {20,20} });
	textA.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });
	tui::symbol_string str2;
	for (char i = 1; i < 127; i++) { str2.push_back(i); }
	str2 = "In Construction....\n";
	textA.setText(str2);

	//LIST
	tui::list list({ {0,0}, {28,28} });
	list.setPositionInfo({ {3,0}, {0,0}, {tui::POSITION::BEGIN, tui::POSITION::CENTER} });
	list.setEntries({
		// 공백 5칸 통일
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

	//INPUT_TEXT
	tui::input_text itxt({ {0,0}, {15,25} });
	itxt.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });

	//NAVIGATION GROUP
	tui::navigation_group tabs_nav({ &text, &textA, &list, &itxt, nullptr });
	tabs_nav.key_next = tui::input::KEY::CTRL_RIGHT;
	tabs_nav.key_prev = tui::input::KEY::CTRL_LEFT;
	tabs_nav.activate();

	tui::init();

	while (!tui::input::isKeyPressed(tui::input::KEY::ESC))
	{
		tui::output::clear();

		tui::output::draw(main_box);
		tui::output::draw(tabs);
		tui::output::draw(tabs_keys);
		tabs_nav.update();

		switch (tabs.getSelected())
		{
		case 0:
			tui::output::draw(order_box);
			tui::output::draw(text);
			break;
		case 1:
			tui::output::draw(textA);
			break;
		case 2:
			tui::output::draw(list);
			break;
		case 3:
			tui::output::draw(textA);
			break;
		}

		tui::output::display();
	}

	return 0;
}