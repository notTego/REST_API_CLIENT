#ifndef _REQUESTS_
#define _REQUESTS_

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
                          const char *cookies);

char *compute_delete_request(const char *host, const char *url,
                             const char *cookies, const char* JWT);

char *compute_post_request(bool add, const char *host, const char *url,
                           const char *content_type, const json &data,
                           const char *cookies, const char *JWT);

char *compute_get_books_request(const char *host, const char *url,
                                const char *cookies, const char *JWT);

#endif
