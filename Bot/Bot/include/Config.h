//
// Created by crazy on 07.12.2021.
//

#ifndef BOT_CONFIG_H
#define BOT_CONFIG_H

#include <fstream>
#include <iostream>
#include "../include/types.h"

const char CONFIG_FILEPATH[] = "config.json";

class Config {
private:
    str &m_configFilepath;
    json m_config;
public:

    explicit Config(const str &cfp = CONFIG_FILEPATH);

    void load();

    void save();

    str getValue(const str &key);

    void setValue(const str &key, const str &value);
};


#endif //BOT_CONFIG_H
