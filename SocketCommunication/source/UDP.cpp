#include "UDP.h"
#include "Predefined.h"

UDP::UDP()
{
    WSADATA wsaData;
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (ret != 0) 
    {
        std::cerr << "[Host] WSAStartup Failed : " << ret << std::endl;
        return;
    }

    host = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (host == INVALID_SOCKET) 
    {
        std::cerr << "[Host] Socket Creation Failed : " << WSAGetLastError() << std::endl;
        WSACleanup();

        return;
    }
}

UDP::~UDP()
{
    stopThread();

    closesocket(host);
    WSACleanup();

    return;
}

bool UDP::sendSimpleMessage(std::string _msg, std::string _ip, uint16_t _port)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    inet_pton(AF_INET, _ip.c_str(), &addr.sin_addr);

    int ret = sendto(host, _msg.c_str(), _msg.size(), 0, (sockaddr*)&addr, sizeof(addr));

    if (ret == SOCKET_ERROR) 
    {
        std::cerr << "[Host] Send Failed : " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

bool UDP::startReceiveThread(uint16_t _port)
{
    if (ports.find(_port) != ports.end()) return false;
    ports.insert(_port);

    auto func = [&]()
        {
            SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

            if (sock == INVALID_SOCKET)
            {
                std::cerr << "[Host " << _port << "] " << "Socket Creation Failed : " << WSAGetLastError() << std::endl;
                WSACleanup();

                return false;
            }

            sockaddr_in addr;
            int size = sizeof(addr);

            addr.sin_family = AF_INET;
            addr.sin_port = htons(_port);
            addr.sin_addr.s_addr = INADDR_ANY;

            if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
            {
                std::cerr << "[Host " << _port << "] " << "Bind Failed : " << WSAGetLastError() << std::endl;

                closesocket(sock);
                WSACleanup();

                return false;
            }

            while (ports.find(_port) != ports.end())
            {
                char msg[BUFFER_SIZE] = { 0, };
                int ret = recvfrom(sock, msg, BUFFER_SIZE, 0, (sockaddr*)&addr, &size);

                if (ret == SOCKET_ERROR) 
                {
                    std::cerr << "[Host " << _port << "] " << "Receive Failed : " << WSAGetLastError() << std::endl;
                }

                else
                {
                    processReceivedMessage(std::string(msg, ret), _port);
                }
            }
        };

    std::thread trd = std::thread(func);
    trd.detach();

    return true;
}

bool UDP::stopThread(uint16_t _port)
{
    ports.erase(_port);
    return true;
}

bool UDP::stopThread()
{
    ports.clear();
    return true;
}

void UDP::processReceivedMessage(std::string _msg, uint16_t _port)
{
    std::cerr << "[Host " << _port << "] " << _msg << std::endl;
}