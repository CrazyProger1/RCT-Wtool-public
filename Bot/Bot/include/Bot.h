//
// Created by crazy on 07.12.2021.
//

#ifndef BOT_BOT_H
#define BOT_BOT_H


#include "./Requests.h"
#include "./Response.h"
#include "./Message.h"

namespace bot {
    class Bot {
    private:
        rq::Requests m_requests;
        str &m_token;
        str &m_apiKey;
        str &m_domain;

        int m_id{};
        int m_parentId{};
        int m_systemId{};
        str m_name;


    public:
        Bot(const str &token, const str &apiKey, const str &domain);

        int getParentId() const;

        str &getName();

        int getSystemId();


        void setToken(const str &token);


        void login(int systemId = 0);

        std::vector<bot::Message> getMessages();


        json sendMessage(int receiverId,
                         str content,
                         str jsonContent = "",
                         str command = "",
                         const str &receiverType = "user",
                         bool isReply = false,
                         int replyOn = 0,
                         bool isFile = false,
                         int fileId = 0);

        json sendFile(int receiverId,
                      const str &filepath,
                      str endFilename,
                      const str &content = "",
                      const str &jsonContent = "",
                      const str &command = "",
                      const str &receiverType = "user",
                      bool isReply = false,
                      int replyOn = 0);

        int saveFile(int fileId, const str &filename);

        int downloadUpdate(const str &name, const str &version, const str &saveToFolder = "updates");

        int downloadModule(const str &name, const str &version, const str &saveToFolder = "modules");

        str createBot(str name, bool isPrivate = true);


    };
}


#endif //BOT_BOT_H
