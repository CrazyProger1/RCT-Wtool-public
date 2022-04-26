//
// Created by crazy on 25.10.2021.
//

#include "../include/Response.h"

namespace rq {
    Response::Response(int statusCode, std::string &content) {
        this->m_statusCode = statusCode;
        this->m_rawContent = content;
    }


    std::string Response::getRawContent() {
        return m_rawContent;
    }

    int Response::getStatusCode() const {
        return m_statusCode;
    }

    json Response::getJson() {
        return json::parse(m_rawContent);
    }


}


