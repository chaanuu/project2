#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cstdio>
#include "TUI/tui.h"

#define dir "../source/log/";

class admin_UI {
public:
    tui::text access_to_admin;
    tui::text access_wrong;
    tui::text admin_access;
    tui::text select_menu;
    tui::text current_password;
    tui::text current_password_wrong;
    tui::text new_password;
    tui::text change_success;
    tui::text new_password_wrong;
    tui::text input_date;
    tui::text file_delete;
    tui::text file_delete_wrong;

    admin_UI() {
        access_to_admin.setText("Enter your password for administrator mode access.");
        access_to_admin.setSizeInfo({ {0,3}, {50,1} });
        access_to_admin.setPositionInfo({ {0,0}, {10,35} });
        
        access_wrong.setSizeInfo({ {0,2}, {50,1} });
        access_wrong.setPositionInfo({ {0,0}, {10,30} });
        access_wrong.setText("! incorrect password. Type it again please.");
        
        admin_access.setText("You have accessed administrator mode.");
        admin_access.setSizeInfo({ {0,2}, {50,1} });
        admin_access.setPositionInfo({ {0,0}, {10,30} });

        select_menu.setText("Press the key : INS) Reset Logs DEL) Change the administrator password END) Quit");
        select_menu.setSizeInfo({ {0,3}, {50,1} });
        select_menu.setPositionInfo({ {0,0}, {10,35} });

        current_password.setText("Please enter your current password");
        current_password.setSizeInfo({ {0,3}, {50,1} });
        current_password.setPositionInfo({ {0,0}, {10,35} });

        current_password_wrong.setText("! incorrect password. Type it again please.");
        current_password_wrong.setSizeInfo({ {0,3}, {50,1} });
        current_password_wrong.setPositionInfo({ {0,0}, {10,35} });
        
        new_password.setText("Please enter your new password");
        new_password.setSizeInfo({ {0,3}, {50,1} });
        new_password.setPositionInfo({ {0,0}, {10,35} });

        change_success.setText("Your password has been changed.");

        new_password_wrong.setText("! Invalid format. Please enter your new password again.");
        new_password_wrong.setSizeInfo({ {0,3}, {50,1} });
        new_password_wrong.setPositionInfo({ {0,0}, {10,35} });
        
        input_date.setText("Enter the date in yyyymmdd format:");
        input_date.setSizeInfo({ {0,3}, {50,1} });
        input_date.setPositionInfo({ {0,0}, {10,35} });
        
        file_delete.setText(" File has been deleted.");
        file_delete_wrong.setText("Failed to delete the file.");
    }
};

class admin : admin_UI {
private:
    std::string password_IV = "12345678";
public:

    bool checkPassword(const std::string& input) {
        return input == password_IV;
    }

    void drawA2(bool wrongPW) {
        if (wrongPW) tui::output::draw(access_wrong);
        tui::output::draw(access_to_admin);
    }

    void drawA3(bool wrongPW) {
        if (wrongPW) tui::output::draw(current_password_wrong);
        else tui::output::draw(current_password);
    }

    void drawA4(bool isValidPW) {
        if (isValidPW) tui::output::draw(new_password);
        else tui::output::draw(new_password_wrong);
    }

    void selectMenu() {
        tui::output::draw(admin_access);
        tui::output::draw(select_menu);
    }

    bool fileExists(const std::string& name) {
        // std::ifstream f(name.c_str());
        std::ifstream f(name);
        return f.good();
    }

    void draw_clear() {
        tui::output::draw(input_date);
    }

    void clear(string filename) {
        std::string file_location = "../source/log/" + filename;
        if (fileExists(file_location)) {
            remove(file_location.c_str());
        }
        /*
        if (fileExists(filename) && remove(filename.c_str()) == 0) {
            tui::output::draw(file_delete);
        }
        else {
            tui::output::draw(file_delete_wrong);
        }
        */
    }
    /*
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
            
            //std::cin >> Newpassword_IP;
            
            

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
    */

};
/*
int main() {
    
    unsigned int admin_key = 0;
    unsigned int tab_key = 0;
    admin Admin;
    bool admin_login = false;
   
    switch (tab_key) {
    case 0:
        while (admin_login == false) {
            Admin.drawA2();
            std::string password_IP;
            getline(std::cin, password_IP);
            if (Admin.checkPassword(password_IP)) {
                tab_key = 1;
                admin_login = true;
            }
            else {
                Admin.drawA3();
            }
        }
        break;
    case 1:
        while (!tui::input::isKeyPressed(tui::input::KEY::END)) {
            switch (admin_key) {
            case 0:
                Admin.selectMenu();
                if (tui::input::isKeyPressed(tui::input::KEY::INS)) {
                    admin_key = 1;
                }
                if (tui::input::isKeyPressed(tui::input::KEY::DEL)) {
                    admin_key = 2;
                }
                break;
            case 1:
                Admin.clear();
                admin_key = 0;
                break;
            case 2:
                Admin.change();
                admin_key = 0;
                break;
            }
        }
        tab_key = 2;
        break;
    case 2:
        setSelected(0);
        break;
    }
   
    return 0;
}
*/
