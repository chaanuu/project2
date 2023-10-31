#include <iostream>
#include <string>
#include <regex>
#include <cstdio>
#include "TUI/tui.h"
#include "filelog.h"


class admin_UI {
protected:
    tui::text access_to_admin;
    tui::text access_wrong;
    tui::text admin_access;
    tui::text select_menu;
    tui::text select_menu_wrong;
    tui::text current_password;
    tui::text current_password_wrong;
    tui::text new_password;
    tui::text change_success;
    tui::text new_password_wrong;
    tui::text input_date;
    tui::text file_delete;
    tui::text file_delete_wrong;

public:
    admin_UI() {

        access_to_admin.setText("Enter your password for administrator mode access.");
        access_wrong.setText("! incorrect password. Type it again please.");
        admin_access.setText("You have accessed administrator mode.");
        select_menu.setText("1) Reset Logs 2) Change the administrator password 3) Quit");
        select_menu_wrong.setText("Invalid input.");
        current_password.setText("Please enter your current password");
        current_password_wrong.setText("! incorrect password. Type it again please.");
        new_password.setText("Please enter your new password");
        change_success.setText("Your password has been changed.");
        new_password_wrong.setText("! Invalid format. Please enter your new password again.");
        input_date.setText("Enter the date in yyyymmdd format:");
        file_delete.setText(" File has been deleted.");
        file_delete_wrong.setText("Failed to delete the file.");
    }
};

class admin : admin_UI {
private:
        string password_IV = "12345678"; 

public:
        int selectMenu() {
            tui::output::draw(admin_access);
            tui::output::draw(select_menu);
            int menu;
            cin >> menu;
            return menu;
        }

        void clear() {
            std::string date;
            tui::output::draw(input_date);
            std::cin >> date;
            if (remove(filename.c_str()) == 0) {
                tui::output::draw(file_delete);
            }
            else {
                tui::output::draw(file_delete_wrong);
            }


        }

        void change() {
            while (true) {
                tui::output::draw(current_password);
                cin.ignore();
                string password_IP;
                getline(cin, password_IP);
                if (password_IV == password_IP) {
                    break;
                }
                else {
                    tui::output::draw(current_password_wrong);
                }
            }
            while (true) {
                std::string Newpassword_IP;
                tui::output::draw(new_password);
                std::cin >> Newpassword_IP;
                std::regex pattern(R"(^[0-9|a-z|A-Z]*$)");

                if (std::regex_match(Newpassword_IP, pattern)) {
                    tui::output::draw(change_success);
                    password_IV = Newpassword_IP;
                    break;
                }
                else {
                    tui::output::draw(new_password_wrong);
                }
            }
        }

        void Exit() {
            exit(0);
        }

        int main() {
            while (true) {
                tui::output::draw(access_to_admin);
                string password_IP;
                getline(cin, password_IP);
                if (password_IV == password_IP) {
                    break;
                }
                else {
                    tui::output::draw(access_wrong);
                }
            }
            while (true) {
                int menu = selectMenu();
                switch (menu) {
                case 1:
                    clear();
                    break;
                case 2:
                    change();
                    break;
                case 3:
                    Exit();
                    break;
                default:
                    tui::output::draw(select_menu_wrong);
                }
            }
            return 0;
        }

 }
