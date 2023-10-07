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

#define IP_ADDRESS  "34.254.242.81"
#define RREGISTER    "/api/v1/tema/auth/register"
#define RAUTH        "/api/v1/tema/auth/login"
#define RACCESS      "/api/v1/tema/library/access"
#define RBOOKS       "/api/v1/tema/library/books"
#define RLOGOUT      "/api/v1/tema/auth/logout"

using json = nlohmann::json;
using namespace std;


enum Command {
    REGISTER,
    LOGIN,
    ENTER_LIBRARY,
    GET_BOOKS,
    GET_BOOK,
    ADD_BOOK,
    DELETE_BOOK,
    LOGOUT,
    HELP,
    EXIT,
    UNKNOWN
};

json parseResponse(char* response, string &JWT) {
    // Find the position where the JSON body starts, update JWT
    char* jsonStart = strstr(response, "{");
    json parsedJson;
    if (jsonStart != nullptr) {
        try {
            // Parse the JSON
            parsedJson = json::parse(jsonStart);

            // Access the token from the parsed JSON
            JWT = parsedJson["token"].dump();
        } catch (const json::parse_error& e) {
            cout << "Failed to parse JSON: " << e.what() << endl;
        }
    }

    return parsedJson;
}

Command parseCommand(const string& commandStr) {
    if (commandStr == "register") {
        return REGISTER;
    } else if (commandStr == "login") {
        return LOGIN;
    } else if (commandStr == "enter_library") {
        return ENTER_LIBRARY;
    } else if (commandStr == "get_books") {
        return GET_BOOKS;
    } else if (commandStr == "get_book") {
        return GET_BOOK;
    } else if (commandStr == "add_book") {
        return ADD_BOOK;
    } else if (commandStr == "delete_book") {
        return DELETE_BOOK;
    } else if (commandStr == "logout") {
        return LOGOUT;
    } else if (commandStr == "help") {
        return HELP;
    } else if (commandStr == "exit") {
        return EXIT;
    } else {
        return UNKNOWN;
    }
}

int main() {
    string input;
    Command command;

    char *message;
    char *response;
    string cookies = "";
    string JWT = "";
    int sockfd;
    int port = 8080;
    json json_web_token;
    string username, password;
    json credential;
    string dummy;
    json parsed;
    string book_id;
    json book;

    cout << "Accepted commands: register, login, enter_library, ";
    cout << "get_books, get_book, add_book, delete_book, logout, ";
    cout << "exit, help." << endl;

    bool loop = true;
    while (loop) {
        cout << "Enter a command: ";
        cin >> input;

        command = parseCommand(input);
        switch (command) {
            case REGISTER:
                // Code for REGISTER command
                sockfd = open_connection((char*)IP_ADDRESS, port, AF_INET,
                                         SOCK_STREAM, 0);

                cout << "username=";
                cin >> username;
                cout << "password=";
                cin >> password;

                credential["username"] = username;
                credential["password"] = password;

                message = compute_post_request(false, (char*)IP_ADDRESS,
                                               (char*)RREGISTER,
                                               "application/json", credential,
                                               "", JWT.c_str());
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                parsed = parseResponse(response, dummy);
                if (parsed.contains("error") && !parsed["error"].is_null()) {
                    string errorMessage = parsed["error"];
                    cout << "Username already taken.\n" << endl;
                }
                break;
            case LOGIN:
                // Code for LOGIN command
                sockfd = open_connection((char*)IP_ADDRESS, port, AF_INET,
                                         SOCK_STREAM, 0);

                if (cookies != "")
                {
                    cout << "Already logged in. ";
                    cout<<"Please log out before reattempting.\n" << endl;
                }
                else
                {
                    cout << "username=";
                    cin >> username;
                    cout << "password=";
                    cin >> password;

                    credential["username"] = username;
                    credential["password"] = password;

                    message = compute_post_request(false, (char*)IP_ADDRESS,
                                                   (char*)RAUTH, "application/json",
                                                   credential,
                                                   "", JWT.c_str());

                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);

                    vector<string> parsedResponse;

                    char *token;
                    token = strtok(response, " ;=");
                    do {
                        string aux = token;
                        parsedResponse.push_back(aux);
                        token = strtok(NULL, " ;=");
                    } while (token);

                    cout << (parsedResponse[1] == "200" ||
                            parsedResponse[1] == "201" ?
                            "Successful log in.\n" : 
                            "Your username or password might be incorrect.\n");

                    bool found = false;
                    for (const auto& entry : parsedResponse) {
                        if (found) {
                            // Cookies gets the next entry after connect.sid
                            cookies = entry;
                            break;
                        }
                        if (entry == "connect.sid") {
                            found = true;
                        }
                    }

                    cout<<"cookie: \""<< cookies<< "\"\n\n";
                }
                break;
            case ENTER_LIBRARY:
                // Code for ENTER_LIBRARY command
                sockfd = open_connection((char*)IP_ADDRESS, port, AF_INET,
                                         SOCK_STREAM, 0);

                if (cookies == "")
                {
                    cout << "Please log in first.\n" << endl;
                }
                else
                {
                    message = compute_get_request((char*)IP_ADDRESS,
                                                  (char*)RACCESS,
                                                  cookies.c_str());

                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);

                    json_web_token = parseResponse(response, JWT);
                    cout << "token: ";
                    cout << json_web_token["token"].dump()<<"\n\n";
                }
                break;
            case GET_BOOKS:
                // Code for GET_BOOKS command
                sockfd = open_connection((char*)IP_ADDRESS, port, AF_INET,
                                         SOCK_STREAM, 0);
                
                if (JWT == "") {
                    cout << "Access to the library not granted.\n" << endl;
                } else {
                    string tokk = json_web_token["token"];
                    message = compute_get_books_request((char*)IP_ADDRESS,
                                                        (char*)RBOOKS,
                                                        cookies.c_str(),
                                                        tokk.c_str());
    
                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd); 
                    
                    char* start = strchr(response, '[');
                    if (start) {
                        cout << start << endl << endl;
                    }
                }
                break;
            case GET_BOOK:
                // Code for GET_BOOK command
                sockfd = open_connection((char*)IP_ADDRESS, port, AF_INET,
                                         SOCK_STREAM, 0);

                if (cookies == "")
                {
                    cout << "Please log in first.\n" << endl;
                }
                else if (JWT == "")
                {
                    cout << "Access to the library not granted.\n" << endl;
                }
                else
                {                
                    int id;
                    while(true) {
                        cout << "book_id=";
                        cin >> id;

                        if(!cin.fail())
                            break;

                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid ID. ";
                        cout << "Please enter a valid numeric ID.\n";
                    }
                    
                    book_id = to_string(id);

                    string root_books_id = "/api/v1/tema/library/books/";
                    root_books_id += book_id;

                    string tokk = json_web_token["token"];
                    message = compute_get_books_request((char*)IP_ADDRESS,
                                                        root_books_id.c_str(),
                                                        cookies.c_str(),
                                                        tokk.c_str());

                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);

                    string dummy;
                    json parsed = parseResponse(response, dummy);

                    // In this context, if our request contains a valid error
                    // message, it is because the book ID has not been found.
                    if (parsed.contains("error") &&
                        !parsed["error"].is_null()) {
                        string errorMessage = parsed["error"];
                        cout << "Book ID not found.\n" << endl;
                        continue;
                    }

                    char* start = strchr(response, '{');
                    if (start) {
                        cout << start << endl << endl;
                    }
                }
                break;
            case ADD_BOOK:
                // Code for ADD_BOOK command
                sockfd = open_connection((char*)IP_ADDRESS, port, AF_INET,
                                         SOCK_STREAM, 0);

                if (cookies == "")
                {
                    cout << "Please log in first.\n" << endl;
                }
                else if (JWT == "")
                {
                    cout << "Access to the library not granted.\n" << endl;
                }
                else
                {
                    cin.get();
                    char read[50];
                    cout << "title=";
                    cin.getline(read, 50);
                    book["title"] = read;

                    cout << "author=";
                    cin.getline(read, 50);
                    book["author"] = read;

                    cout << "genre=";
                    cin.getline(read, 50);
                    book["genre"] = read;

                    cout << "publisher=";
                    cin.getline(read, 50);
                    book["publisher"] = read;

                    // Verify valid numerical input
                    int no_pages;
                    while(true) {
                        cout << "page_count=";
                        cin >> no_pages;

                        if(!cin.fail())
                            break;

                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input. ";
                        cout << "Please enter a valid number.\n";
                    }

                    book["page_count"] = no_pages;

                    string tokk = json_web_token["token"];
                    message = compute_post_request(true, (char*)IP_ADDRESS,
                                                   (char*)RBOOKS,
                                                   "application/json",
                                                   book, cookies.c_str(),
                                                   tokk.c_str());

                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);
                }
                break;
            case DELETE_BOOK:
                // Code for DELETE_BOOK command
                sockfd = open_connection((char*)IP_ADDRESS, port, AF_INET,
                                         SOCK_STREAM, 0);

                if (cookies == "")
                {
                    cout << "Please log in first.\n" << endl;
                }
                else if (JWT == "")
                {
                    cout << "Access to the library not granted.\n" << endl;
                }
                else
                {

                    int id;
                    while(true) {
                        cout << "book_id=";
                        cin >> id;

                        if(!cin.fail())
                            break;

                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid ID. Please enter a valid numeric ID.\n";
                    }
                    
                    book_id = to_string(id);

                    string root_books_id = "/api/v1/tema/library/books/";
                    root_books_id += book_id;

                    string tokk = json_web_token["token"];
                    message = compute_delete_request((char*)IP_ADDRESS,
                                                     root_books_id.c_str(),
                                                     cookies.c_str(),
                                                     tokk.c_str());

                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);

                    string dummy;
                    json parsed = parseResponse(response, dummy);

                    if (parsed.contains("error") &&
                        !parsed["error"].is_null()) {
                        string errorMessage = parsed["error"];
                        cout << "Book ID not found.\n" << endl;
                    }
                }
                break;
            case LOGOUT:
                // Code for LOGOUT command
                sockfd = open_connection((char*)IP_ADDRESS, port, AF_INET,
                                         SOCK_STREAM, 0);

                if (cookies == "")
                {
                    cout << "Please log in first.\n" << endl;
                }
                else
                {
                    cout<<"Logging out...\n";
                    message = compute_get_request((char*)IP_ADDRESS,
                                                  (char*)RLOGOUT,
                                                  cookies.c_str());

                    send_to_server(sockfd, message);
                    response = receive_from_server(sockfd);
                    cookies = "";
                    JWT = "";
                    cout<<"Logged out!\n";
                }
                break;
            case HELP:
                // Code for HELP command
                cout << "Accepted commands: register, login, enter_library, ";
                cout << "get_books, get_book, add_book, delete_book, logout, ";
                cout << "exit, help." << endl;
                break;
            case EXIT:
                // Code for EXIT command
                cout<<"Disconnecting client...\n";
                loop = false;
                break;
            case UNKNOWN:
                // Code for handling unknown command
                cout << "Unknown command.\n";
                break;
        }
    }

    cout<<"Client disconnected. Good bye!\n";
    return 0;
}
