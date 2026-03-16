#pragma once

#include <string>
#include <functional>
#include <map>
#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>

/**
 * @brief 简单的http服务器
 * 用于接收A2A协议的请求
 */

class HttpServer {
public:
    using RequestHandler = std::function<std::string(const std::string&)>;

    explicit HttpServer(int port) : port_(port), running_(false) {};

    ~HttpServer() {
        stop();
    }

    void register_handler(const std::string& path, RequestHandler handler) {
        handler_[path] = handler;
    }

    void start() {
        running_ = true;

        // 创建 soket
        int sever_fd = socket(AF_INET, SOCK_STREAM, 0); // sever 端的文件描述符，在 Linux/Unix 系统编程中，socket、文件、管道等资源都用一个整数来表示，这个整数叫做文件描述符（fd）
        if (sever_fd < 0) {
            throw std::runtime_error("Failed to create socket");
        }

        // 设置 socket 选项
        int opt = 1;
        setsockopt(sever_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // 绑定地址
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port_);

        if (bind(sever_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            close(sever_fd);
            throw std::runtime_error("Failed to bind to port: " + std::to_string(port_));
        }

        // 监听
        if (listen(sever_fd, 10) < 0)
        { // 在拒绝新的连接请求之前，最多可以排队10个连接。
            close(sever_fd);
            throw std::runtime_error("Failed to listen on port: " + std::to_string(port_));
        }

        std::cout << "Http Server listening on port: " << port_ << std::endl;

        // 接收连接
        while (running_) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);

            int client_fd = accept(sever_fd, (struct sockaddr*)&client_addr, &client_len);
            if (client_fd < 0) {
                continue;
            }

            // 处理请求（在新线程中）
            std::thread([this, client_fd]()
                        { this->handle_client(client_fd); })
                .detach();
        }

        close(sever_fd);
    }

    void stop() {
        running_ = false;
    }

private:
    void handle_client(int client_fd) {
        char buffer[8192] = {0};
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

        if (bytes_read < 0) {
            close(client_fd);
            return;
        }

        std::string request(buffer, bytes_read);

        // 解析 HTTP 请求
        std::istringstream request_stream(request);
        std::string method, path, version;
        request_stream >> method >> path >> version;    // 按空白字符分割，依次读取请求行的三个字段

        // 提取请求体
        std::string body;
        size_t body_pos = request.find("\r\n\r\n");
        if (body_pos != std::string::npos) {
            body = request.substr(body_pos + 4);
        }

        // 查找处理器
        std::string response_body;
        int status_code = 200;

        auto it = handler_.find(path);
        if (it != handler_.end()) {
            try {
                response_body = it->second(body);
            }
            catch (const std::exception& e) {
                status_code = 500;
                response_body = std::string("{\"error:\"\"") + e.what() + "\"}";
            }
        }
        else {
            status_code = 404;
            response_body = "{\"error\":\"Not found\"}";
        }

        // 构造 HTTP 响应
        std::ostringstream response;
        response << "HTTP/1.1 " << status_code << " OK\r\n";
        response << "Content-Type: application/json\r\n";
        response << "Content-Length: " << response_body.length() << "\r\n";
        response << "Access-Control-Allow-Origin: *\r\n";
        response << "\r\n";
        response << response_body;

        std::string response_str = response.str();
        write(client_fd, response_str.c_str(), response_str.length());

        close(client_fd);
    }

    int port_;
    bool running_;
    std::map<std::string, RequestHandler> handler_;
};