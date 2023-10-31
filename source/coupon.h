#include"report.h"

class Coupon : public Report {
public:
    Coupon(std::string name, std::string day);
    int useCoupon();
};

Coupon::Coupon(std::string name, std::string day) : Report(day) {
    Report::name = name;
    Report::folder = "User/";
}

int Coupon::useCoupon() {
    std::ifstream in(folder + name + ".csv");

    if (!in.is_open()) {
        std::cout << "Can't find file" << std::endl;
        return 0;
    }
    in.close();

    std::vector<std::string>* userreport = readFile(folder + name + ".csv");
    std::vector<std::string> splitstr = split((*userreport)[0], ',');
    int canuse;

    if (splitstr.size() > 1) {
        canuse = std::stoi(splitstr[0]) / 10000 - std::stoi(splitstr[1]);
    }
    else {
        canuse = std::stoi(splitstr[0]) / 10000;
    }

    if (canuse == 0) {
        std::cout << "Cannot use";
        return 0;
    }
    canuse--;
    (*userreport)[0] = splitstr[0] + "," + std::to_string(std::stoi(splitstr[0]) / 10000 - canuse) + "\n";

    std::ofstream out(folder + name + ".csv");
    std::vector<std::string>::iterator itr = userreport->begin();
    out << *itr;
    for (itr = userreport->begin() + 1; itr != userreport->end(); ++itr) {
        out << *itr + "\n";
    }

    return 1;
}
