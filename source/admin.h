#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cstdio>
#include "TUI/tui.h"

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
        std::string password_IV = "12345678"; 
        std::string filename;

public:
        int selectMenu() {
            tui::output::draw(admin_access);
            tui::output::draw(select_menu);
            int menu;
            std::cin >> menu;
            return menu;
        }

       bool fileExists(const std::string& name) {
        std::ifstream f(name.c_str());
        return f.good();
    }

    void clear() {
        std::string date;
        tui::output::draw(input_date);
        std::cin >> date;
        filename = date + ".csv";
        
        if (fileExists(filename) && remove(filename.c_str()) == 0) {
            tui::output::draw(file_delete);
        }
        else {
            tui::output::draw(file_delete_wrong);
        }
    }

        void change() {
            while (true) {
                tui::output::draw(current_password);
                std::cin.ignore();  
                std::string password_IP;   
                getline(std::cin, password_IP);
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
                std::cin.ignore();
                std::regex pattern(R"(^[0-9a-zA-Z]*$)");

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

 }

int main() {
    admin Admin;
    while (true) {
        tui::output::draw(Admin.access_to_admin);
        std::string password_IP;
        getline(std::cin, password_IP);
        if (Admin.password_IV == password_IP) {
            break;
        }
        else {
            tui::output::draw(Admin.access_wrong);
        }
    }
    while (true) {
        int menu = Admin.selectMenu();
        switch (menu) {
        case 1:
            Admin.clear();
            break;
        case 2:
            Admin.change();
            break;
        case 3:
            Admin.Exit();
            break;
        default:
            tui::output::draw(Admin.select_menu_wrong);
        }
    }
    return 0;
}
