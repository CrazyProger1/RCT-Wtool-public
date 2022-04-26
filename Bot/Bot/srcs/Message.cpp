//
// Created by crazy on 07.12.2021.
//

#include "../include/Message.h"


namespace bot {

    void Message::parseJson(json &jsonData) {
        for (json::iterator msgField = jsonData.begin(); msgField != jsonData.end(); ++msgField) {
            if (jsonData[msgField.key()] == nullptr)
                jsonData[msgField.key()] = "";
        }
        m_senderId = jsonData["sender_id"];
        m_senderType = jsonData["sender_type"];

        m_messageType = jsonData["message_type"];

        m_command = jsonData.value("command", "");
        m_jsonContent = jsonData.value("json_content", "{}");
        m_content = jsonData.value("content", "");

        m_fileId = jsonData.value("file_id", 0);
        m_filename = jsonData.value("filename", "");
        m_sendingDatetime = jsonData.value("sending_datetime", "");

        m_isReply = jsonData.value("is_reply", false);
        m_replyOn = jsonData.value("reply_on", 0);


    }

    Message::Message(int id) {
        m_id = id;
    }

    int Message::getId() {
        return m_id;
    }

    int Message::getSenderId() {
        return m_senderId;
    }

    str &Message::getContent() {
        return m_content;
    }

    json Message::getJsonContent() {
        return json::parse(m_jsonContent);
    }

    str &Message::getCommand() {
        return m_command;
    }

    int Message::getFileId() {
        return m_fileId;
    }

    str &Message::getFilename() {
        return m_filename;
    }

}