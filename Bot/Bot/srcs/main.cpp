#include <ctime>
#include "../include/Config.h"
#include "../include/logging.h" // !
#include "../include/Bot.h"
#include "../include/App.h"

str getCurrentTime() {
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    return std::to_string(now->tm_year + 1900) +
           "-" + std::to_string(now->tm_mon) +
           "-" + std::to_string(now->tm_mday) +
           " " + std::to_string(now->tm_hour) +
           ":" + std::to_string(now->tm_min);
}

int main() {
    checkLogOverflow();



    logInfo(getCurrentTime());
    logInfo("APPLICATION LAUNCHED");
    Config config;

    str token = config.getValue("token");
    str apiKey = config.getValue("api_key");
    str domain = config.getValue("domain");

    bot::Bot bot(
            token,
            apiKey,
            domain
    );

    App app(bot);
    app.run();

    logInfo("APP SUCCESSFULLY TERMINATED");
    logInfo(getCurrentTime());

    return 0;
}
