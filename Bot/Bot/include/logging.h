//
// Created by crazy on 07.12.2021.
//

#ifndef BOT_LOGGING_H
#define BOT_LOGGING_H

#include <iostream>
#include <string>
#include <fstream>

typedef std::string str;

const str logFilepath = "logs.txt";

void logInfo(const str &info = "hello");

void logError(const str &info = "hello");

void checkLogOverflow();

void clearLog();

#endif //BOT_LOGGING_H
