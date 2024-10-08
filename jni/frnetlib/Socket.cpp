//
// Created by fred on 06/12/16.
//

#include <mutex>
#include <csignal>
#include <iostream>
#include <vector>
#ifdef USE_SSL
#include <mbedtls/error.h>
#endif
#include <frnetlib/NetworkEncoding.h>
#include <frnetlib/Socket.h>
#include <frnetlib/Sendable.h>

namespace fr
{
    Socket::Socket()
    : ai_family(AF_UNSPEC),
      max_receive_size(0),
      socket_read_timeout(0),
      socket_write_timeout(0)
    {
        init_wsa();
    }

    Socket::Status Socket::send(const Sendable &obj)
    {
        if(!connected())
            return Socket::Status::Disconnected;

        return obj.send(this);
    }

    Socket::Status Socket::receive(Sendable &obj)
    {
        return obj.receive(this);
    }

    Socket::Status Socket::receive_all(void *dest, size_t buffer_size)
    {
        auto bytes_remaining = (ssize_t) buffer_size;
        while(bytes_remaining > 0)
        {
            size_t received = 0;
            Status status = receive_raw((char*)dest + (buffer_size - bytes_remaining), (size_t)bytes_remaining, received);
            bytes_remaining -= received;
            if(status != Socket::Status::Success)
            {
                if((ssize_t)buffer_size == bytes_remaining)
                    return status;
                if(status == Socket::Status::WouldBlock)
                    continue;
                return Socket::Status::Disconnected;
            }
        }

        return Socket::Status::Success;
    }

    void Socket::shutdown()
    {
        ::shutdown(get_socket_descriptor(), SHUT_RDWR);
    }

    void Socket::set_inet_version(Socket::IP version)
    {
        switch(version)
        {
            case Socket::IP::v4:
                ai_family = AF_INET;
                break;
            case Socket::IP::v6:
                ai_family = AF_INET6;
                break;
            case Socket::IP::any:
                ai_family = AF_UNSPEC;
                break;
            default:
                throw std::logic_error("Unknown Socket::IP value passed to set_inet_version()");
        }
    }

    std::string Socket::status_to_string(fr::Socket::Status status)
    {
#ifdef _WIN32
        auto wsa_err_to_str = [](int err) -> std::string {
        std::string buff(255, '\0');
        auto len = FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), &buff[0], buff.size(), NULL);
        if(len == 0)
            return "Unknown";
        buff.resize(len);
        return buff;
        };
        #define ERR_STR wsa_err_to_str(WSAGetLastError())
#else
#define ERR_STR strerror(errno)
#endif

        switch(status)
        {
            case Socket::Status::Unknown:
                return "Unknown";
            case Socket::Status::Success:
                return "Success";
            case Socket::Status::ListenFailed:
                return std::string("Listen Failed (").append(ERR_STR).append(")");
            case Socket::Status::BindFailed:
                return std::string("Bind Failed (").append(ERR_STR).append(")");
            case Socket::Status::Disconnected:
                return "The Socket Is Not Connected";
            case Socket::Status::Error:
                return "Error";
            case Socket::Status::WouldBlock:
                return "Would Block";
            case Socket::Status::ConnectionFailed:
                return "Connection Failed";
            case Socket::Status::HandshakeFailed:
                return "Handshake Failed";
            case Socket::Status::VerificationFailed:
                return "Verification Failed";
            case Socket::Status::MaxPacketSizeExceeded:
                return "Max Packet Size Exceeded";
            case Socket::Status::NotEnoughData:
                return "Not Enough Data";
            case Socket::Status::ParseError:
                return "Parse Error";
            case Socket::Status::HttpHeaderTooBig:
                return "HTTP Header Too Big";
            case Socket::Status::HttpBodyTooBig:
                return "HTTP Body Too Big";
            case Socket::Status::AddressLookupFailure:
#ifdef _WIN32
                return std::string("Address Lookup Failure (").append(wsa_err_to_str(WSAGetLastError())).append(")");
#else
                return std::string("Address Lookup Failure (").append(gai_strerror(errno)).append(")");
#endif
            case Socket::Status::SendError:
                return std::string("Send Error (").append(ERR_STR).append(")");
            case Socket::Status::ReceiveError:
                return std::string("Receive Error (").append(ERR_STR).append(")");
            case Socket::Status::AcceptError:
                return std::string("Accept Error (").append(ERR_STR).append(")");
            case Socket::Status::SSLError:
            {
#ifdef USE_SSL
                char buff[256] = {0};
                mbedtls_strerror(errno, buff, sizeof(buff));
                return std::string("SSL Error (").append(buff).append(")");
#else
                return "Generic SSL Error";
#endif
            }
            case Socket::Status::NoRouteToHost:
                return "No Route To Host";
            case Socket::Status::Timeout:
                return "Timeout";
            default:
                return "Unknown";
        }

        return "Internal Error";
    }

    void Socket::disconnect()
    {
        close_socket();
    }
}