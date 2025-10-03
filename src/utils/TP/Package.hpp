#pragma once

#include <string>
#include <sstream>

class Package {
public:
    virtual ~Package() = default; 
    virtual std::string serialize() const = 0; 
};

class RequestPackage : public Package {
public:
    // 构造函数：请求包
    RequestPackage(const std::string& command, const std::string& params = "")
        : command(command), params(params) {}

    // 序列化为字符串
    std::string serialize() const override {
        std::stringstream ss;
        ss << command;
        if (!params.empty()) {
            ss << " " << params;
        }
        ss << "\n";
        return ss.str();
    }

    // 解析字符串为 RequestPackage
    static RequestPackage parse(const std::string& data) {
        std::string command, params;
        size_t pos = data.find(' ');
        if (pos == std::string::npos) {
            command = data.substr(0, data.find('\n'));
        } else {
            command = data.substr(0, pos);
            params = data.substr(pos + 1, data.find('\n') - pos - 1);
        }
        return RequestPackage(command, params);
    }

    std::string getCommand() const { return command; }
    std::string getParams() const { return params; }

private:
    std::string command; 
    std::string params;  
};

class ResponsePackage : public Package {
public:
    // 构造函数：响应包
    ResponsePackage(const std::string& status, const std::string& message = "")
        : status(status), message(message) {}

    // 序列化为字符串
    std::string serialize() const override {
        std::stringstream ss;
        ss << status;
        if (!message.empty()) {
            ss << " " << message;
        }
        ss << "\n";
        return ss.str();
    }

    static ResponsePackage parse(const std::string& data) {
        std::string status, message;
        size_t pos = data.find(' ');
        if (pos == std::string::npos) {
            status = data.substr(0, data.find('\n'));
        } else {
            status = data.substr(0, pos);
            message = data.substr(pos + 1, data.find('\n') - pos - 1);
        }
        return ResponsePackage(status, message);
    }

    std::string getStatus() const { return status; }
    std::string getMessage() const { return message; }

private:
    std::string status;  // 状态（OK, ERROR）
    std::string message; // 消息
};