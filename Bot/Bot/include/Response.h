//
// Created by crazy on 25.10.2021.
//

#ifndef BOT_RESPONSE_H
#define BOT_RESPONSE_H

#include <string>
#include "types.h"

namespace rq {
    class Response {
    private:
        int m_statusCode;
        str m_rawContent;


    public:
        Response(int statusCode, std::string &content);

        str getRawContent();

        json getJson();

        [[nodiscard]] int getStatusCode() const;
    };
}


#endif //BOT_RESPONSE_H
