//
// Created by crazy on 07.12.2021.
//

#include "../include/Config.h"
#include "../include/logging.h" // !

Config::Config(const str &cfp) : m_configFilepath((str &) cfp) {
    load();
}

void Config::load() {
    std::ifstream cf(m_configFilepath);
    str buffer, line;

    if (!cf.is_open()) {
        logError("CONFIG FILE CANNOT BE LOADED");
        exit(1);
    }
    while (getline(cf, line)) {
        buffer += line;
    }
    m_config = json::parse(buffer);

    logInfo("CONFIG LOADED");
}


void Config::save() {
    str commandToShowFile = "attrib " + m_configFilepath + " -S -H";
    str commandToHideFile = "attrib " + m_configFilepath + " +S +H";

    system(commandToShowFile.c_str());
    std::ofstream cf(m_configFilepath);

    cf << m_config;
    system(commandToHideFile.c_str());
}

str Config::getValue(const str &key) {
    return m_config[key];
}

void Config::setValue(const str &key, const str &value) {
    m_config[key] = value;
}

