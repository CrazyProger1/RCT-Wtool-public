//
// Created by crazy on 07.12.2021.
//

#include "../include/Bot.h"
#include <sys/stat.h>


namespace bot {
    Bot::Bot(const str &token, const str &apiKey, const str &domain) : m_token((str &) token),
                                                                       m_apiKey((str &) apiKey),
                                                                       m_domain((str &) domain) {

    }

    void Bot::login(int systemId) {
        rq::Data data;
        rq::Params params;

        data["token"] = m_token;

        if (systemId != 0)
            params["system_id"] = std::to_string(systemId);

        logInfo("HOST: " + m_domain);

        try {
            rq::Response response = m_requests.post(
                    m_domain + "/api/bots/login",
                    &data,
                    &params,
                    nullptr
            );

            if (response.getStatusCode() == 200) {
                json jsonData = response.getJson();
                try {
                    m_id = jsonData["id"];
                } catch (std::invalid_argument &e) {
                    logError("LOGIN ERROR (ID IS WRONG)");
                    exit(1);
                }

                m_name = std::move(jsonData["name"]);
                m_parentId = jsonData["parent_id"];
                m_systemId = jsonData["system_id"];

                logInfo("SUCCESSFULLY LOGGED IN");
            } else if (response.getStatusCode() == 401) {
                logError("LOGIN WAS UNSUCCESSFUL (TOKEN IS WRONG)");
                exit(1);
            } else {
                logError("LOGIN WAS UNSUCCESSFUL");
                Sleep(60000);
                login();
            }

        } catch (rq::RequestsException &e) {
            logError("LOGIN WAS UNSUCCESSFUL (CONNECTION ERROR)");
            Sleep(60000);
            login();
        } catch (std::exception &e) {
            logError("LOGIN WAS UNSUCCESSFUL (UNKNOWN ERROR)");
            Sleep(60000);
            login();
        }


    }

    std::vector<bot::Message> Bot::getMessages() {
        logInfo("SENDING REQUEST FOR GETTING MESSAGES");

        rq::Headers headers;
        rq::Params params;


        headers["token"] = m_token;
        params["receiver_type"] = "bot";

        std::vector<Message> messages;

        try {
            rq::Response response = m_requests.get(
                    m_domain + "/api/messages",
                    &params,
                    &headers
            );
            json jsonData = response.getJson();

            if (response.getStatusCode() == 200)
                for (auto msgData = jsonData.begin(); msgData != jsonData.end(); ++msgData) {
                    json msgContent = msgData.value();

                    const str &msgId = msgData.key();

                    Message msg(std::stoi(msgId));

                    msg.parseJson(msgContent);
                    messages.push_back(msg);
                    logInfo("MESSAGE PUSHED TO LIST ( " + msgId + " )");
                }
            else {
                logError("MESSAGES WERE NOT GET");
            }
        } catch (rq::RequestsException &e) {
            logError("MESSAGES WERE NOT GET");
        } catch (std::exception &e) {
            logError("MESSAGES WERE NOT GET");
        }

        return messages;

    }

    int Bot::saveFile(int fileId, const str &filename) {
        rq::Requests requests;
        rq::Params params;
        rq::Headers headers;

        headers["token"] = m_token;
        params["receiver_type"] = "bot";

        try {
            rq::Response response = requests.download(m_domain + "/api/files/" + std::to_string(fileId), filename,
                                                      &params,
                                                      &headers);

            return response.getStatusCode();
        } catch (std::exception &e) {
            logError("FILE DOWNLOADING ERROR");
        }


    }

    json Bot::sendMessage(int receiverId,
                          str content,
                          str jsonContent,
                          str command,
                          const str &receiverType,
                          bool isReply,
                          int replyOn,
                          bool isFile,
                          int fileId) {
        rq::Data data;
        rq::Headers headers;
        rq::Params params;


        data["content"] = std::move(content);
        data["json_content"] = std::move(jsonContent);
        data["command"] = std::move(command);


        headers["token"] = m_token;

        params["sender_id"] = std::to_string(m_id);
        params["sender_type"] = "bot";
        params["receivers_type"] = receiverType;
        params["receivers_ids"] = std::to_string(receiverId);
        params["is_reply"] = std::to_string(isReply);
        params["reply_on"] = std::to_string(replyOn);

        if (isFile) {
            params["message_type"] = "file";
            params["file_id"] = std::to_string(fileId);
        }

        try {
            rq::Response response = m_requests.post(
                    m_domain + "/api/messages",
                    &data,
                    &params,
                    &headers
            );

            return response.getJson();
        } catch (std::exception &e) {
            logError("MESSAGE SENDING ERROR");
        }


    }


    int Bot::getParentId() const {
        return m_parentId;
    }


    str &Bot::getName() {
        return m_name;
    }

    json Bot::sendFile(int receiverId,
                       const str &filepath,
                       str endFilename,
                       const str &content,
                       const str &jsonContent,
                       const str &command,
                       const str &receiverType,
                       bool isReply,
                       int replyOn) {

        rq::Headers headers;
        rq::Params params;


        params["sender_type"] = "bot";
        params["filename"] = std::move(endFilename);

        headers["token"] = m_token;
        try {
            rq::Response response = m_requests.upload(m_domain + "/api/files", filepath, &params, &headers);

            json filesIds = response.getJson();

            sendMessage(receiverId, content, jsonContent, command,
                        receiverType, isReply, replyOn, true, filesIds[0]);
        } catch (std::exception &e) {

        }


    }

    int Bot::downloadUpdate(const str &name, const str &version, const str &saveToFolder) {

        if (!fs::exists(saveToFolder))
            mkdir(saveToFolder.c_str());

        str folderToSave = saveToFolder + "/" + name;
        mkdir(folderToSave.c_str());

        rq::Response response = m_requests.download(m_domain + "/api/updates/" + name + "/" + version,
                                                    saveToFolder + "/" + name + "/" + version);

        return response.getStatusCode();
    }

    int Bot::downloadModule(const str &name, const str &version, const str &saveToFolder) {
        if (!fs::exists(saveToFolder))
            mkdir(saveToFolder.c_str());

        str folderToSave = saveToFolder + "/" + name;
        mkdir(folderToSave.c_str());

        rq::Response response = m_requests.download(m_domain + "/api/modules/" + name + "/" + version,
                                                    saveToFolder + "/" + name + "/" + name + ".exe");

        return response.getStatusCode();
    }

    str Bot::createBot(str name, bool isPrivate) {
        rq::Data data;
        rq::Headers headers;
        rq::Params params;


        data["name"] = std::move(name);
        data["is_private"] = isPrivate;

        headers["token"] = m_token;
        headers["api_key"] = m_apiKey;


        params["is_api_requesting"] = std::to_string(true);

        try {
            rq::Response response = m_requests.post(
                    m_domain + "/api/bots/register",
                    &data,
                    &params,
                    &headers
            );
            if (response.getStatusCode() == 200) {
                json jsonResp = response.getJson();
                return jsonResp["token"];
            } else {
                throw std::invalid_argument("Api key or token are wrong");
            }
        } catch (std::exception &e) {
            logError("BOT CREATING ERROR");
            return m_token;
        }
    }

    int Bot::getSystemId() {
        return m_systemId;
    }

    void Bot::setToken(const str &token) {
        m_token = token;
    }


}