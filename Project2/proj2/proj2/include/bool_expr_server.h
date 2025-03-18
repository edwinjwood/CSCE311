#ifndef BOOL_EXPR_SERVER_H_
#define BOOL_EXPR_SERVER_H_

#include <string>

// Function declaration for the server start function
int start_server(const std::string& file_path, const std::string& server_name, 
                 char unit_separator, char eot);

#endif  // BOOL_EXPR_SERVER_H_