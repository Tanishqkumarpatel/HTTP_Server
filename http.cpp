#include "http.hpp"

const std::map<std::string, std::string> mimeTypes{
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".json", "application/json"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".txt", "text/plain"},
    {".ico", "image/x-icon"},
    {".svg", "image/svg+xml"},
    {".pdf", "application/pdf"}
};

static bool isValidKey(const std::string& key) {
    auto it = key.cbegin();
    while (it != key.cend()) {
        if (*it == ' ' || *it == '\r' || *it == '\n') {
            return false;
        }
        it++;
    }
    return true;
}

std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* utc_tm = std::gmtime(&now_c);
    std::ostringstream oss;
    oss << std::put_time(utc_tm, "%Y-%m-%d %H:%M:%S UTC");
    std::string now_c_string = oss.str();
    return now_c_string; 
}

HttpRequest parse_request(const std::string& req) {
    HttpRequest http_req {};
    std::stringstream ss;
    ss.str(req);
    ss >> http_req.method >> http_req.resource >> http_req.version;
    ss.get();   // skip '\r'
    ss.get();   // skip '\n'

    while (true) {
        std::string key {};
        std::string value {};
        std::getline(ss, key, ':');
        if (ss.peek() == ' ') {
            ss.get();   // skip space ' '
        }
        std::getline(ss, value, '\r');
        ss.get();   // skip \n 
        http_req.headers[key] = value;
        if (ss.peek()=='\r') {
            ss.get();   // skip '\r'
            ss.get();   // skip '\n'
            break;
        }
    }
    
    if (http_req.headers.find("Content-Length") != http_req.headers.end()) {
        int length = atoi(http_req.headers["Content-Length"].c_str());
        std::string data(length, '\0');
        ss.read(&data[0], length);
        http_req.body = data;
    }

    return http_req;

}


std::string build_response(const HttpResponse& res) {
    std::string text_res {};
    text_res.append(res.version);
    text_res.append(" ");
    text_res.append(std::to_string(res.status_code));
    text_res.append(" ");
    text_res.append(res.status_text);
    text_res.append("\r\n");
    
    auto it = res.headers.cbegin();
    while (it != res.headers.cend())
    {
        text_res.append(it->first);
        text_res.append(": ");
        text_res.append(it->second);
        text_res.append("\r\n");
        it++;
    }
    text_res.append("\r\n");    // end of header
    text_res.append(res.body);

    return text_res;    
}

std::optional<HttpResponse> isValidRequest(const HttpRequest& req) {
    HttpResponse res = {};

    if (req.version != "HTTP/1.1") {
        res.status_code=505;
        res.status_text="HTTP Version Not Supported";
        res.headers["Date"]=getTimestamp();
        res.headers["Warning"]="199 - \"Only Supports HTTP/1.1\"";
        return res;
    }

    bool validMethod {false};
    
    for (int i=0; i<(int)std::size(methods);i++) {
        if (req.method == methods[i]) {
            validMethod = true;
            break;
        }
    }

    if (!validMethod) {
        res.status_code=501;
        res.status_text="Not Implemented";
        res.headers["Date"]=getTimestamp();
        res.headers["Warning"]="199 - \"Onlt Supports - GET, POST, PUT, HEAD, DELETE Methods (case-sensitive)\"";
        return res;
    }
    auto host_ptr = req.headers.find("Host");
    if (host_ptr == req.headers.end() || host_ptr->second == "") {
        res.status_code=400;
        res.status_text="Bad Request";
        res.headers["Date"]=getTimestamp();
        res.headers["Warning"]="199 - \"Host header is mandatory\"";
        return res;
    }
    
    auto it = req.headers.cbegin();
    while (it != req.headers.cend()) {
        if (!isValidKey(it->first)) {
            res.status_code=400;
            res.status_text="Bad Request";
            res.headers["Date"]=getTimestamp();
            res.headers["Warning"]="199 - Bad Header";
            return res;
        }
        it++;
    }

    return std::nullopt;
    
}