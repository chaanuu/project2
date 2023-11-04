#include "TUI/tui.h"
#include "path.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <filesystem>

class Report{
public:
    std::vector<int> menucode;
    int menucodeint;
    static std::string folder;
    std::string day;
    //std::string name;

    void createMenucode();
    void newReportFile(std::string fileName);
    std::vector<std::string>* readLogfile(std::string orderNum);

    Report(std::string day);
    void editReport(std::string orderNum);
    static std::vector<std::string>* readFile(const std::string& str);
    static std::vector<std::string> split(std::string str, char delimiter);

    tui::symbol_string printReport();
    static std::vector<std::string> readExistingFile();
};

std::string Report::folder = SOURCE_FILE_LOCATION"Daily/";

void Report::createMenucode() {
    std::ifstream in(SOURCE_FILE_LOCATION"database/menuDB.CSV");
    std::string s;

    while (in) {
        getline(in, s);
        menucode.push_back(std::stoi(s.substr(0, 4)));
    }

    in.close();
}

Report::Report(std::string day) {
    createMenucode();
    Report::day = day;
    Report::name = day;
    Report::folder = SOURCE_FILE_LOCATION"Daily/";
}

void Report::newReportFile(std::string fileName) {
    std::ofstream out(folder + fileName + ".csv");

    out << "0\n";

    std::ifstream in(SOURCE_FILE_LOCATION"database/menuDB.CSV");
    std::string s;

    while (in) {
        getline(in, s);
        out << s.substr(0, s.length()) + "0,0,\n";
    }

    in.close();
    out.close();
}

void Report::editReport(std::string orderNum) {
    std::vector<std::string>* log = readLogfile(orderNum);

    std::ifstream in(folder + name + ".csv");

    if (!in.is_open()) {
        newReportFile(name);
        std::cout << "new file";
        in.close();
    }

    std::vector<std::string>* reportstr = readFile(folder + name + ".csv");

    std::vector<std::string>::iterator itr = log->begin();
    std::vector<std::string> firststr = split((*reportstr)[0], ',');
    std::string firstreturn = std::to_string(std::stoi(*itr) + std::stoi(firststr[0]));
    if (firststr.size() > 1) {
        firstreturn += "," + firststr[1];
    }

    (*reportstr)[0] = firstreturn;

    for (std::vector<std::string>::iterator it = log->begin() + 1; it != log->end(); ++it) {
        std::vector<int>::iterator it1 = find(menucode.begin(), menucode.end(), std::stoi((*it).substr(0, 4)));
        std::vector<std::string> edit = split((*reportstr)[it1 - menucode.begin() + 1], ',');
        std::vector<std::string> logsplit = split(*it, ',');
        edit[3] = std::to_string(std::stoi(logsplit[1]) + std::stoi(edit[3]));
        edit[4] = std::to_string(std::stoi(edit[2]) * std::stoi(edit[3]));

        std::string editfinish;
        for (std::vector<std::string>::iterator it2 = edit.begin(); it2 != edit.end(); ++it2) {
            editfinish += *it2 + ",";
        }

        (*reportstr)[it1 - menucode.begin() + 1] = editfinish;
    }

    std::ofstream out(folder + name + ".csv");
    for (std::vector<std::string>::iterator it = reportstr->begin(); it != reportstr->end(); ++it) {
        out << *it;
        out << "\n";
    }

    delete reportstr;
    delete log;
}

std::vector<std::string>* Report::readFile(const std::string& str) {
    std::ifstream in(str);
    std::vector<std::string>* strs = new std::vector<std::string>();

    if (!in.is_open()) {
        std::cout << "Can't find file" << std::endl;
        return strs;
    }

    std::string s;

    while (in) {
        getline(in, s);
        strs->push_back(s);
    }

    while (1) {
        std::vector<std::string>::reverse_iterator rit = strs->rbegin();
        if (rit == strs->rend()-1) {
            break;
        }
        else {
            if (*rit == *(rit + 1) || (*rit) == "") {
                strs->pop_back();
            }
            else {
                break;
            }
        }
    }
    in.close();
    return strs;
}

std::vector<std::string>* Report::readLogfile(std::string orderNum) {
    std::vector<std::string>* logs = readFile(SOURCE_FILE_LOCATION"log/" + day + ".csv");
    std::vector<std::string> logstr;

    for (std::vector<std::string>::reverse_iterator itr = logs->rbegin(); itr != logs->rend(); ++itr) {
        logstr = split(*itr, ',');
        if (logstr[0] == orderNum)
            break;
    }

    std::vector<std::string>* reportlog = new std::vector<std::string>();
    reportlog->push_back(logstr[3]);

    for (int i = 4; i < logstr.size(); i += 2) {
        reportlog->push_back(logstr[i] + "," + logstr[i + 1]);
    }

    delete logs;
    return reportlog;
}

std::vector<std::string> Report::split(std::string input, char delimiter) {
    std::vector<std::string> answer;
    std::stringstream ss(input);
    std::string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}

tui::symbol_string Report::printReport() {
    std::vector<std::string>* reportstr = readFile(folder + name + ".csv");

    tui::symbol_string returnstr = "Total Revenue : " + (*reportstr)[0] + "\n";
    int category = 0;
    for (std::vector<std::string>::iterator itr = reportstr->begin() + 1; itr != reportstr->end(); ++itr) {
        std::vector<std::string> splitstr = split(*itr, ',');
        if (std::stoi(splitstr[0]) / 1000 != category) {
            category = std::stoi(splitstr[0]) / 1000;
            returnstr += "Category\n";
        }
        returnstr += splitstr[1] + " (" + splitstr[2] + "won) : " + splitstr[3] + " cup " + splitstr[4] + "won sold\n";
    }

    return returnstr;
}

std::vector<std::string> Report::readExistingFile() {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path() / SOURCE_FILE_LOCATION"Daily")) {
            std::cout << entry.path() << std::endl;
            files.push_back(entry.path().filename().string());
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        system("pause");
    }

    return files;
}

class Report_UI {
protected:
    tui::list* dayList;
    tui::box reportBox;
    tui::symbol_string report_string = "please select date";
    tui::text report;

    friend class Report;
public:
    Report_UI() {
        //LIST default
        setdayList();

        reportBox.setAppearance(tui::box_appearance::thin_line);
        reportBox.setSizeInfo({ {0,0}, {58,92} });
        reportBox.setPositionInfo({ {0,0}, {40,3} });

        report.setSizeInfo({ {0,0}, {55,90} });
        report.setPositionInfo({ {0,0}, {42,5} });
        report.setText(report_string);

        makeList();
    }

    void setdayList() {
        delete dayList;
        dayList = new tui::list;

        dayList->setSizeInfo({ {0,0}, {30,75} });
        dayList->setPositionInfo({ {0,1}, {3,6} });
    }

    void drawUI() {
        tui::output::draw(*dayList);
        tui::output::draw(reportBox);
        tui::output::draw(report);

        dayList->activate();
    }

    void makeList() {
        std::vector<std::string> files = Report::readExistingFile();
        setdayList();
        while (!files.empty()) {
            tui::symbol_string lastFile = files.back();  // ������ ��Ҹ� ������
            tui::list_entry entry(lastFile, tui::CHECK_STATE::NONCHECKABLE, nullptr, nullptr, nullptr);
            files.pop_back();               // ������ ��Ҹ� ����
            dayList->insertEntryAt(entry, 0);
        }
        dayList->update();
    }

    void update(Report& report_instance) {
        report_string = report_instance.printReport();
        report.setText(report_string);
    }

    void load_report() {
        int position = dayList->getCurrentPosition();
        tui::list_entry currentEntry = dayList->getEntryAt(position);
        tui::symbol_string reportDay = currentEntry.getFileName();
        std::string fileName = reportDay.getStdString();
        Report report = Report(fileName.substr(0, 8));
        update(report);
    }
};
