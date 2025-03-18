#include <bool_expr_server.h>
#include <domain_socket.h>
#include <bool_expr_parser.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <csignal>
#include <unistd.h>

// Global flag for clean shutdown
volatile sig_atomic_t keep_running = 1;

// Signal handler
void signal_handler(int signal_num) {
    (void)signal_num;
    keep_running = 0;
}

// BooleanExpressionServer class - extends DomainSocketServer
class BooleanExpressionServer : public DomainSocketServer {
private:
    std::vector<std::string> expressions_;
    char unit_separator_;

public:
    BooleanExpressionServer(const char* sock_path, bool abstract, char unit_separator, char eot, const std::vector<std::string>& expressions) 
    : DomainSocketServer(sock_path, abstract, eot), expressions_(expressions), unit_separator_(unit_separator) {}

    // Process a client connection
    void HandleClient(int client_socket) {
        if (client_socket < 0) return;
        
        std::cout << "Client connected" << std::endl;
        
        // Send configuration
        std::string config;
        config.push_back(unit_separator_);
        config.push_back(eot_);  // Access parent class eot_
        Write(client_socket, config);
        
        // Read truth values
        std::string buffer;
        ssize_t bytes_received = Read(client_socket, &buffer);
        
        if (bytes_received > 0) {
            // Use Explode to parse truth values with unit_separator_
            std::string exploded_buffer = Explode(buffer.c_str(), unit_separator_);
            std::string truth_values;
            
            // Extract only T and F
            for (char c : exploded_buffer) {
                if (c == 'T' || c == 'F') truth_values += c;
            }
            
            // Evaluate expressions
            int true_count = 0, false_count = 0, error_count = 0;
            EvaluateExpressions(truth_values, true_count, false_count, error_count);
            
            // Send response
            std::string response = std::to_string(true_count) + "T" + unit_separator_ +
                                  std::to_string(false_count) + "F" + unit_separator_ +
                                  std::to_string(error_count) + "E";
                                  
            ssize_t bytes_sent = Write(client_socket, response);
            
            if (bytes_sent > 0) {
                std::cout << "\t" << bytes_sent << "B sent, " 
                          << bytes_received << "B received" << std::endl;
            }
        }
        
        ::close(client_socket);
    }

private:
    // Evaluate all expressions with given truth values
    void EvaluateExpressions(const std::string& truth_values, int& true_count, int& false_count, int& error_count) {
        if (!truth_values.empty()) {
            try {
                // Build the truth value map
                std::unordered_map<char, bool> values_map = BuildMap(truth_values);
                
                // Process each expression
                for (const auto& expr_text : expressions_) {
                    if (!keep_running) break;
                    
                    try {
                        if (expr_text.empty()) continue;
                        
                        // Parse and evaluate the expression
                        BooleanExpressionParser parser(expr_text, values_map);
                        bool result = parser.Parse();
                        
                        // Count results
                        if (parser.HasError()) {
                            error_count++;
                        } else if (result) {
                            true_count++;
                        } else {
                            false_count++;
                        }
                    } catch (...) {
                        error_count++;
                    }
                }
            } catch (...) {
                error_count = expressions_.size();
            }
        } else {
            error_count = expressions_.size();
        }
    }
};

// Helper to clean up socket file
void cleanup_socket_file(const std::string& socket_path) {
    if (!socket_path.empty() && socket_path[0] != '\0') {
        unlink(socket_path.c_str());
    }
}

// Run the server
int start_server(const std::string& file_path, const std::string& server_name, char unit_separator, char eot) {
    // Set up signal handlers
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    signal(SIGPIPE, SIG_IGN);
    
    // Load expressions from file
    std::vector<std::string> expressions;
    std::ifstream file(file_path);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                // Pre-process expressions using Explode for consistency
                std::string exploded = Explode(line.c_str(), ' ');
                if (!exploded.empty()) expressions.push_back(exploded);
            }
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << file_path << std::endl;
        return 1;
    }

    // Main server loop
    while (keep_running) {
        cleanup_socket_file(server_name);
        
        try {
            // Create server with our custom class
            BooleanExpressionServer server(server_name.c_str(), true, 
                                          unit_separator, eot, expressions);
            
            if (!server.Init(5)) {
                sleep(1);
                continue;
            }

            // Handle client connections
            while (keep_running) {
                int client_socket = server.Accept();
                
                if (client_socket < 0) {
                    if (keep_running) sleep(0);
                    continue;
                }
                
                // Process this client
                server.HandleClient(client_socket);
            }
        } catch (...) {
            // If server crashes, we'll restart it
        }
        
        cleanup_socket_file(server_name);
    }
    
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <file_path> <server_name> "
                  << "<unit_separator> <eot>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    std::string server_name = argv[2];
    char unit_separator = argv[3][0];
    char eot = argv[4][0];

    return start_server(file_path, server_name, unit_separator, eot);
}