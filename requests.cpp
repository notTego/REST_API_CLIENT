#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <unistd.h>
#include "nlohmann/json.hpp"
#include "helpers.h"
#include "requests.h"

using json = nlohmann::json;
using namespace std; 


char *compute_get_request(const char *host, const char *url,
                          const char *cookies)
{
    string request = "GET " + string(url) + " HTTP/1.1\r\n";
    request += "Host: " + string(host) + "\r\n";

    if (cookies != NULL && strcmp(cookies, "") != 0)
    {
        request += "Cookie: connect.sid=" + string(cookies) + "\r\n";
    }

    request += "\r\n";

    char *request_message = new char[request.length() + 1];
    strcpy(request_message, request.c_str());

    return request_message;
}

char *compute_delete_request(const char *host, const char *url,
                             const char *cookies, const char* JWT)
{
    string request = "DELETE " + string(url) + " HTTP/1.1\r\n";
    request += "Host: " + string(host) + "\r\n";

    if (cookies != NULL && strcmp(cookies, "") != 0)
    {
        request += "Cookie: connect.sid=" + string(cookies) + "\r\n";
    }
    request += "Authorization: Bearer " + string(JWT) + "\r\n";

    request += "\r\n";

    char *request_message = new char[request.length() + 1];
    strcpy(request_message, request.c_str());

    return request_message;
}

char *compute_post_request(bool add, const char *host, const char *url,
                           const char *content_type, const json &data,
                           const char *cookies, const char *JWT)
{
    string request = "POST " + string(url) + " HTTP/1.1\r\n";
    request += "Host: " + string(host) + "\r\n";

    if (add)
        request += "Authorization: Bearer " + string(JWT) + "\r\n";

    request += "Content-Type: " + string(content_type) + "\r\n";

    if (cookies != nullptr && strcmp(cookies, "") != 0)
        request += "Cookie: connect.sid=" + string(cookies) + "\r\n";

    string body_data = data.dump();
    int content_length = body_data.length();

    request += "Content-Length: " + to_string(content_length) + "\r\n";
    request += "\r\n" + body_data;

    char *request_message = new char[request.length() + 1];
    strcpy(request_message, request.c_str());

    return request_message;
}

char *compute_get_books_request(const char *host, const char *url,
                                const char *cookies, const char *JWT)
{
    string request = "GET " + string(url) + " HTTP/1.1\r\n";
    request += "Host: " + string(host) + "\r\n";

    if (cookies != NULL && strcmp(cookies, "") != 0)
    {
        request += "Cookie: connect.sid=" + string(cookies) + "\r\n";
    }

    if (JWT != NULL && strcmp(JWT, "") != 0)
    {
        request += "Authorization: Bearer " + string(JWT) + "\r\n";
    }

    request += "\r\n";

    char *request_message = new char[request.length() + 1];
    strcpy(request_message, request.c_str());

    return request_message;
}
