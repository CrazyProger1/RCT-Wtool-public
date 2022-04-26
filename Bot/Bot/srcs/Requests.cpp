//
// Created by crazy on 30.11.2021.
//

#include "../include/Requests.h"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

size_t writeData(void *ptr, size_t size, size_t nmemb, std::string *data) {
    data->append((char *) ptr, size * nmemb);
    return size * nmemb;
}


namespace rq {


    Requests::Requests() {
        static bool wasInitialized;

        if (!wasInitialized) {
            curl_global_init(CURL_GLOBAL_ALL);
            logInfo("REQUESTS INITIALIZED");

            wasInitialized = true;
        }

    }


    Response Requests::get(
            str url,
            Params *params,
            Headers *headers,
            const std::string &endOfUrl) {

        m_pCurl = curl_easy_init();

        logInfo("PREPARING TO GET REQUEST ( " + url + " )");

        struct curl_slist *currentSlist;
        currentSlist = nullptr;

        if (headers != nullptr) {
            for (auto &header: *headers) {
                std::string h = header.first + ":" + header.second;
                currentSlist = curl_slist_append(currentSlist, h.c_str());
            }
        }

        if (params != nullptr) {
            int paramCounter = 0;
            url += "?";

            for (auto &param: *params) {

                url += param.first + "=" + param.second;
                paramCounter++;
                if (paramCounter < params->size()) {
                    url += "&";
                }
            }
        }

        url += endOfUrl;

        str content;


        logInfo("STARTING GET REQUEST ( " + url + " )");
        curl_easy_setopt(m_pCurl, CURLOPT_CAINFO, "cacert.pem");
        curl_easy_setopt(m_pCurl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, currentSlist);

        curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, writeData);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &content);

        long statusCode = perform();

        curl_slist_free_all(currentSlist);


        return {statusCode, content};
    }


    Response Requests::post(
            str url,
            Data *data,
            Params *params,
            Headers *headers,
            const std::string &endOfUrl) {

        m_pCurl = curl_easy_init();

        logInfo("PREPARING TO POST REQUEST ( " + url + " )");

        std::string jsonData;

        if (data != nullptr) {
            jsonData = data->dump();
        }

        if (params != nullptr) {
            int paramCounter = 0;
            url += "?";

            for (auto &param: *params) {

                url += param.first + "=" + param.second;
                paramCounter++;
                if (paramCounter < params->size()) {
                    url += "&";
                }
            }
        }


        struct curl_slist *currentSlist;
        currentSlist = nullptr;
        if (headers != nullptr) {
            for (auto &header: *headers) {
                std::string h = header.first + ":" + header.second;
                currentSlist = curl_slist_append(currentSlist, h.c_str());
            }
        }

        url += endOfUrl;


        currentSlist = curl_slist_append(currentSlist, "Content-Type: application/json");

        std::string content;


        logInfo("STARTING POST REQUEST ( " + url + " )");
        curl_easy_setopt(m_pCurl, CURLOPT_CAINFO, "cacert.pem");
        curl_easy_setopt(m_pCurl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, currentSlist);
        curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
        curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, writeData);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &content);

        long statusCode = perform();

        curl_slist_free_all(currentSlist);


        return {statusCode, content};

    }


    Response
    Requests::download(str url, const str &outFilePath, Params *params, Headers *headers, const str &endOfUrl) {
        m_pCurl = curl_easy_init();

        logInfo("PREPARING TO GET REQUEST ( " + url + " )");


        struct curl_slist *currentSlist;
        currentSlist = nullptr;

        if (headers != nullptr) {
            for (auto &header: *headers) {
                std::string h = header.first + ":" + header.second;
                currentSlist = curl_slist_append(currentSlist, h.c_str());
            }
        }

        if (params != nullptr) {
            int paramCounter = 0;
            url += "?";

            for (auto &param: *params) {

                url += param.first + "=" + param.second;
                paramCounter++;
                if (paramCounter < params->size()) {
                    url += "&";
                }
            }
        }

        logInfo("STARTING DOWNLOADING FROM ( " + url + " )");

        url += endOfUrl;

        FILE *fp;

        fp = fopen(outFilePath.c_str(), "wb");


        str content;
        curl_easy_setopt(m_pCurl, CURLOPT_CAINFO, "cacert.pem");
        curl_easy_setopt(m_pCurl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, currentSlist);

        curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, fp);

        long statusCode = perform();

        curl_slist_free_all(currentSlist);

        if (statusCode != 200)
            logError("STATUS CODE IS " + std::to_string(statusCode) + ", DOWNLOADING ERROR");
        else
            logInfo("FILE DOWNLOADED, SAVED IN " + outFilePath);

        fclose(fp);

        return {statusCode, content};
    }

    int Requests::perform() {
        CURLcode res;
        long statusCode;
        res = curl_easy_perform(m_pCurl);
        if (res == CURLE_OK) {
            curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &statusCode);
        } else {
            logError("CURL ERROR " + std::to_string(res));
            throw RequestsException();
        }
        logInfo("REQUEST WAS SUCCESSFUL ( STATUS CODE: " + std::to_string(statusCode) + " )");


        curl_easy_cleanup(m_pCurl);
        return statusCode;
    }

    Response Requests::upload(str url,
                              const str &filePath,
                              Params *params,
                              Headers *headers,
                              const str &endOfUrl) {


        logInfo("PREPARING TO POST REQUEST ( " + url + " )");

        if (!fs::exists(filePath)) {
            logError("FILE <" + filePath + "> DOES NOT EXISTS");
            throw std::invalid_argument("File <" + filePath + "> does not exists");
        }

        m_pCurl = curl_easy_init();

        curl_httppost *post = nullptr;
        curl_httppost *last = nullptr;

        struct curl_slist *currentSlist;
        currentSlist = nullptr;

        if (headers != nullptr) {
            for (auto &header: *headers) {
                std::string h = header.first + ":" + header.second;
                currentSlist = curl_slist_append(currentSlist, h.c_str());
            }
        }

        if (params != nullptr) {
            int paramCounter = 0;
            url += "?";

            for (auto &param: *params) {

                url += param.first + "=" + param.second;
                paramCounter++;
                if (paramCounter < params->size()) {
                    url += "&";
                }
            }
        }

        m_pCurl = curl_easy_init();

        curl_formadd(&post, &last,
                     CURLFORM_COPYNAME, "file",
                     CURLFORM_FILE, filePath.c_str(),
                     CURLFORM_END);

        str content;

        logInfo("STARTING POST REQUEST ( " + url + " )");
        curl_easy_setopt(m_pCurl, CURLOPT_CAINFO, "cacert.pem");
        curl_easy_setopt(m_pCurl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(m_pCurl, CURLOPT_HTTPPOST, post);
        curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, currentSlist);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, writeData);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &content);

        long statusCode = perform();

        curl_slist_free_all(currentSlist);

        if (statusCode == 200) {
            logInfo("FILE ( " + filePath + " ) WAS UPLOADED SUCCESSFULLY");
        }

        return {statusCode, content};
    }
}


