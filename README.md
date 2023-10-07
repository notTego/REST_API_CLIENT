# RESTful API Client: JSON Interaction and Error Handling

## JSON Library Usage

I chose to utilize the nlohmann JSON library due to its excellent documentation, which facilitated the effective utilization of this library.

## Main Implementation

To maintain a clean and organized user interface, I avoided using an "if...else..." structure and, instead, employed a switch-case implementation. Every function was defined as part of an enum, and I utilized the `parseCommand` function to retrieve the corresponding command for each string. This command is then used within a switch statement.

For the `ADD_BOOK` command, I employed `cin.getline()` to read strings containing two or more words. I set the buffer length to 50 and implemented a secure read function to prevent potential buffer overflows. Additionally, I included a `cin.get()` call before reading the title to ensure that the "\n" character after "add_book" did not interfere.

Validation of numerical inputs involves checking if `cin` failed while reading input into an int variable. In case of failure, the error state is cleared, and the rest of the values read by `cin` are disregarded, with a message promptly informing the user that the ID is not a valid number.

For the `LOGOUT` command, care is taken to reset the cookie and token after the user has ended the session.

## Error Handling

There are two types of errors:
- Locally handled errors include situations such as invalid book IDs or a user not being logged in.
- Errors resulting from issues in the server's reply, such as a book ID not being found, are also managed.

## Outputs

As per the instructions, the response from the server is parsed, and only relevant information is printed, avoiding the printing of the entire reply. Specifically:
- For `LOGIN`, the cookie is extracted and printed.
- Similarly, for `ENTER_LIBRARY`, the token is handled in the same manner.
- For `GET_BOOKS` and `GET_BOOK`, the entire JSON response is printed, starting from the first '{'. Additionally, extra relevant outputs have been added.

## Coding Style and Naming Conventions

- Parsing functions use camel case, while other functions adhere to the traditional C naming convention with underscores.
- There is no space between the function name and (), and {} starts on the next line.
- For `if`, `while`, etc., there is a space before () and {} starts on the same line.
- The maximum line length is limited to 80 characters.

Final memory cleanup is performed for both the message sent to the server and the server's response.
