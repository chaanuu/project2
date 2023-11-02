#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cstdio>
#include "TUI/tui.h"

#define dir SOURCE_FILE_LOCATION"log/";

class admin_UI {
public:
    tui::text access_to_admin;
    tui::text access_wrong;
    tui::text admin_access;
    tui::text select_menu;
    tui::text current_password;
    tui::text current_password_wrong;
    tui::text new_password;
    tui::text new_password_wrong;
    tui::text input_date;

    admin_UI() {
        access_to_admin.setText("Enter your password for administrator mode access.");
        access_to_admin.setSizeInfo({ {0,3}, {50,1} });
        access_to_admin.setPositionInfo({ {0,0}, {5,10} });
        
        access_wrong.setSizeInfo({ {0,2}, {50,1} });
        access_wrong.setPositionInfo({ {0,0}, {5,25} });
        access_wrong.setText("! incorrect password. Type it again please.");
        
        admin_access.setText("You have accessed administrator mode.");
        admin_access.setSizeInfo({ {0,3}, {50,1} });
        admin_access.setPositionInfo({ {0,0}, {5,10} });

        select_menu.setText("Press the key : INS) Reset Logs | DEL) Change the administrator password | Enter) Quit");
        select_menu.setSizeInfo({ {0,3}, {100,1} });
        select_menu.setPositionInfo({ {0,0}, {5,15} });

        current_password.setText("Please enter your current password");
        current_password.setSizeInfo({ {0,3}, {50,1} });
        current_password.setPositionInfo({ {0,0}, {5,10} });

        current_password_wrong.setText("! incorrect password. Type it again please.");
        current_password_wrong.setSizeInfo({ {0,3}, {50,1} });
        current_password_wrong.setPositionInfo({ {0,0}, {5,20} });
        
        new_password.setText("Please enter your new password");
        new_password.setSizeInfo({ {0,3}, {50,1} });
        new_password.setPositionInfo({ {0,0}, {5,10} });

        new_password_wrong.setText("! Invalid format. Please enter your new password again.");
        new_password_wrong.setSizeInfo({ {0,3}, {50,1} });
        new_password_wrong.setPositionInfo({ {0,0}, {5,20} });
        
        input_date.setText("Enter the date in yyyymmdd format:");
        input_date.setSizeInfo({ {0,3}, {50,1} });
        input_date.setPositionInfo({ {0,0}, {5,10} });
        
    }
};

class admin : admin_UI {

public:

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
    };
};

