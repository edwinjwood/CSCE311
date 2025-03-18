#include <bool_expr_client.h>
#include <domain_socket.h>
#include <bool_expr_parser.h> 
#include <iostream>
#include <sstream>
#include <csignal>
#include <vector>


const std::string Explode(const char input[], char delim) {
    std::string result;
    std::istringstream iss(input);
    std::string token;
    
    // Process each token separated by the delimiter
    while (std::getline(iss, token, delim)) {
        if (!token.empty()) {
            result += token + " ";
        }
    }
    
    // Remove trailing space if any
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

// Global flag for clean shutdown
volatile sig_atomic_t keep_running = 1;

// Signal handler
void signal_handler(int) {
    keep_running = 0;  // Set flag to exit cleanly
}

// BooleanExpressionClient class - extends DomainSocketClient
class BooleanExpressionClient : public DomainSocketClient {
private:
    char eot_;
    char unit_separator_;

public:
    BooleanExpressionClient(const char* server_name, bool abstract) : DomainSocketClient(server_name, abstract), 
          eot_('.'),            // Default EOT
          unit_separator_(':')  // Default unit separator
    {}

    bool ConnectAndProcess(const std::string& truth_values) {
        // Connect to server
        std::cout << "BoolExprClient connecting..." << std::endl;
        
        if (!Init()) {
            return false;
        }

        // Get configuration from server
        if (!ReceiveConfiguration()) {
            return false;
        }

        // Send truth values and get response
        std::string formatted_values = FormatTruthValues(truth_values);
        ssize_t bytes_sent = Write(formatted_values, eot_);
        if (bytes_sent <= 0) {
            return false;
        }

        // Process server response
        return ProcessResponse(bytes_sent);
    }

private:
    bool ReceiveConfiguration() {
        std::string config;
        ssize_t config_bytes = Read(eot_, &config);
        
        if (config_bytes > 0 && config.length() >= 2) {
            unit_separator_ = config[0];
            eot_ = config[1];
            return true;
        }
        
        return false;
    }

    std::string FormatTruthValues(const std::string& truth_values) {
        std::istringstream iss(truth_values);
        std::string token;
        std::string formatted_values;
        bool first = true;
        
        while (iss >> token) {
            // Only accept valid T/F values
            if (token != "T" && token != "F") 
                continue;
            
            if (!first) {
                formatted_values += unit_separator_;
            } else {
                first = false;
            }
            formatted_values += token;
        }
        
        // If no valid values found, use a default
        if (formatted_values.empty()) {
            formatted_values = "F";
        }
        
        return formatted_values;
    }

    bool ProcessResponse(ssize_t bytes_sent) {
        std::string response;
        ssize_t bytes_received = Read(eot_, &response);
        
        if (bytes_received <= 0) {
            return false;
        }

        // Parse response
        int true_evaluations = 0;
        int false_evaluations = 0;
        int could_not_evaluate = 0;
        
        ParseResponse(response, true_evaluations, false_evaluations, could_not_evaluate);

        // Output results with nice formatting
        std::cout << "Finished with " << bytes_received << "B received, " 
                  << bytes_sent << "B sent." << std::endl;
        std::cout << "Results" << std::endl;
        std::cout << "True Evaluations: " << true_evaluations << std::endl;
        std::cout << "False Evaluations: " << false_evaluations << std::endl;
        std::cout << "Could Not Evaluate: " << could_not_evaluate << std::endl;
        
        return true;
    }

    void ParseResponse(const std::string& response, int& true_evaluations, int& false_evaluations, int& could_not_evaluate) {
        // Use Explode to split the response by unit separator
        std::string exploded = Explode(response.c_str(), unit_separator_);
        
        // Process the exploded string
        std::istringstream iss(exploded);
        std::string token;
        
        while (iss >> token) {
            if (token.empty()) 
                continue;
            
            char type = token.back();
            std::string number_str = token.substr(0, token.length() - 1);
            
            try {
                int count = std::stoi(number_str);
                switch (type) {
                    case 'T':
                        true_evaluations = count;
                        break;
                    case 'F':
                        false_evaluations = count;
                        break;
                    case 'E':
                        could_not_evaluate = count;
                        break;
                }
            } catch (...) {
                // Silently ignore parsing errors
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <server_name> <truth_values>" << std::endl;
        std::cerr << "Example: " << argv[0] << " bool_expr_sock T F T F F T" << std::endl;
        return 1;
    }

    // Set up signal handlers
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    // Ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    std::string server_name = argv[1];
    
    // Combine all remaining arguments into a single string for truth values
    std::string truth_values;
    for (int i = 2; i < argc; i++) {
        truth_values += argv[i];
        if (i < argc - 1) {
            truth_values += " ";
        }
    }

    // Create and use client
    BooleanExpressionClient client(server_name.c_str(), true);
    client.ConnectAndProcess(truth_values);

    return 0;
}