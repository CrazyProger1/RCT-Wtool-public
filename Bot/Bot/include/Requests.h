//
// Created by crazy on 30.11.2021.
//

#ifndef BOT_REQUESTS_H
#define BOT_REQUESTS_H


#include <map>
#include <curl/curl.h>
#include "./types.h"
#include "Response.h"
#include "../include/logging.h" // !




namespace rq {
    typedef std::map<std::string, std::string> Params;
    typedef std::map<std::string, std::string> Headers;
    typedef json Data;

    class RequestsException : std::exception {

    };

    class Requests {
    private:
        CURL *m_pCurl{};
    private:
        int perform();

    public:
        Requests();

        Response get(str url,
                     Params *params = nullptr,
                     Headers *headers = nullptr,
                     const str &endOfUrl = "");

        Response post(str url,
                      Data *data = nullptr,
                      Params *params = nullptr,
                      Headers *headers = nullptr,
                      const str &endOfUrl = "");

        Response download(str url,
                          const str &outFilePath,
                          Params *params = nullptr,
                          Headers *headers = nullptr,
                          const str &endOfUrl = "");

        Response upload(str url,
                        const str &filePath,
                        Params *params = nullptr,
                        Headers *headers = nullptr,
                        const str &endOfUrl = "");


    };


}

#endif //BOT_REQUESTS_H
