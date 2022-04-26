//
// Created by crazy on 07.12.2021.
//

#ifndef BOT_MESSAGE_H
#define BOT_MESSAGE_H

#include "./types.h"
#include "./Requests.h"
#include "./Response.h"

namespace bot {
    class Message {
    private:
        int m_id = 0;

        int m_senderId = 0;
        str m_senderType;

        str m_messageType;
        str m_sendingDatetime;

        str m_content;
        str m_jsonContent;
        str m_command;

        int m_fileId{};
        str m_filename;

        bool m_isReply = false;
        int m_replyOn = 0;

    public:
        explicit Message(int id);

        void parseJson(json &jsonData);

        int getId();

        int getSenderId();

        str &getContent();

        json getJsonContent();

        str &getCommand();

        int getFileId();

        str &getFilename();

    };
}


#endif //BOT_MESSAGE_H
