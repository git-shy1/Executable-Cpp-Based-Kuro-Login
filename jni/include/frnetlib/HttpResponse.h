//
// Created by fred on 10/12/16.
//

#ifndef FRNETLIB_HTTPRESPONSE_H
#define FRNETLIB_HTTPRESPONSE_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Http.h"


namespace fr
{
    class HttpResponse : public Http
    {
    public:
        //Constructors
        HttpResponse()=default;
        HttpResponse(HttpResponse&&)=default;
        HttpResponse(const HttpResponse&)= default;
        HttpResponse &operator=(const HttpResponse &)=default;
        HttpResponse &operator=(HttpResponse &&)=default;

        /*!
         * Parse a raw request or response from a string
         * into the object.
         *
         * @param data The request/response to parse
         * @param datasz The length of data in bytes
         * @return NotEnoughData if parse needs to be called again. Success on success, other on error.
         */
        fr::Socket::Status parse(const char *data, size_t datasz) override;

        /*!
         * Constructs a HttpResponse, ready to send.
         *
         * @return The constructed HTTP response.
         */
        std::string construct(const std::string &host) const override;

    private:
        /*!
         * Parses the request header.
         *
         * @param header_end_pos The position in 'body' of the end of the header
         */
        bool parse_header(size_t header_end_pos);

        //State
        bool header_ended{false};
        size_t content_length{0};
        size_t chunk_offset{0};
    };
}

#endif //FRNETLIB_HTTPRESPONSE_H
