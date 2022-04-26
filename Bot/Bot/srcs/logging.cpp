//
// Created by crazy on 07.12.2021.
//

#include "../include/logging.h"


void logInfo(const str &info) {
//    std::cout << "INFO: " << info << std::endl;

    std::ofstream lf;
    lf.open(logFilepath, std::ios::app);
    lf << "INFO: " << info << std::endl;
    lf.close();

}

void logError(const str &err) {
//    std::cerr << "ERROR: " << err << std::endl;

    std::ofstream lf;
    lf.open(logFilepath, std::ios::app);
    lf << "ERROR: " << err << std::endl;
    lf.close();
}

void checkLogOverflow() {
    std::ifstream lf(logFilepath, std::ifstream::ate | std::ifstream::binary);
    long long fSize = lf.tellg();
    if (fSize > 1000000) {
        lf.open(logFilepath);
        lf.close();
    }
}

void clearLog() {
    std::ofstream lf;
    lf.open(logFilepath);
    lf.close();

    logInfo("LOGS CLEARED");
}