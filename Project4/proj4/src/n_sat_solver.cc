#include <n_sat_solver.h>
#include <bool_expr_parser.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static const bool DEBUG = false; // Set to true for debugging output

void debug_print(const std::string& msg) {
    if (DEBUG) {
        std::cerr << "[DEBUG] " << msg << std::endl;
    }
}

NSatSolver::NSatSolver(std::size_t n_threads, const std::string& filename, 
                       std::size_t n_vars)
    : n_threads_(n_threads), filename_(filename), n_vars_(n_vars) {
    // ENTERING CRITICAL SECTION EXITING CRITICAL SECTION
    // No synchronization needed in constructor as this executes before threads are created
    LoadExpressions();
}

void NSatSolver::LoadExpressions() {
    debug_print("Loading expressions from file: " + filename_);
    
    // OPENING FILE
    int fd = open(filename_.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error: Could not open file " << filename_ << std::endl;
        return;
    }
    
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        std::cerr << "Error: Could not get file size" << std::endl;
        return;
    }
    
    // MEMORY MAPPING FILE
    char* addr = static_cast<char*>(mmap(nullptr, sb.st_size, 
                                       PROT_READ, MAP_PRIVATE, fd, 0));
    if (addr == MAP_FAILED) {
        close(fd);
        std::cerr << "Error: Could not map file" << std::endl;
        return;
    }
    
    // READING FILE
    const char* current = addr;
    const char* end = addr + sb.st_size;
    size_t line_count = 0;
    
    while (current < end) {
        const char* newline = static_cast<const char*>(memchr(current, '\n', end - current));
        size_t length = newline ? newline - current : end - current;
        
        if (length > 0) {
            std::string line(current, length);
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();  // Handle Windows-style line endings
            }
            
            // Skip empty lines and comments
            if (!line.empty() && line[0] != '/' && line[0] != '#') {
                // Trim whitespace
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                line.erase(line.find_last_not_of(" \t\r\n") + 1);
                
                debug_print("Raw line " + std::to_string(line_count) + ": " + line);
                
                // Process with Explode
                std::string processed_line = Explode(line.c_str());
                
                debug_print("Processed line " + std::to_string(line_count) + ": " + processed_line);
                
                if (!processed_line.empty()) {
                    expressions_.push_back(processed_line);
                    line_count++;
                }
            }
        }
        
        current = newline ? newline + 1 : end;
    }
    
    // UNMAPPING FILE
    munmap(addr, sb.st_size);
    
    // CLOSING FILE
    close(fd);
    
    debug_print("Loaded " + std::to_string(expressions_.size()) + " expressions");
}

// START ROUTINE
void* NSatSolver::ThreadSolve(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    NSatSolver* solver = data->solver;
    
    data->stats = {0, 0};
    
    // Only keep the expression_indices code path, since that's what we're using
    for (size_t idx : data->expression_indices) {
        if (idx >= solver->expressions_.size()) continue;
        
        const std::string& expr = solver->expressions_[idx];
        
        debug_print("\n=== Thread " + std::to_string(data->thread_id) + " ===");
        debug_print("Evaluating expression " + std::to_string(idx) + ": " + expr);
        debug_print("Using " + std::to_string(solver->n_vars_) + " variables");
        
        // Use the SatSolver function to evaluate the expression
        bool is_sat = SatSolver(solver->n_vars_, expr);
        
        debug_print("Result for expression " + std::to_string(idx) + ": " + 
                  (is_sat ? "SAT" : "UNSAT"));
        
        if (is_sat) {
            data->stats.sat_count++;
        } else {
            data->stats.unsat_count++;
        }
        
        debug_print("Running counts - SAT: " + std::to_string(data->stats.sat_count) +
                  " UNSAT: " + std::to_string(data->stats.unsat_count));
    }
    
    // Create result with thread ID stored
    ThreadStats* result = new ThreadStats(data->stats);
    result->thread_id = data->thread_id;
    
    delete data;
    return result;
}

void NSatSolver::Solve() {
    if (expressions_.empty()) return;

    std::cout << "Thread  Sat  Unsat" << std::endl;
    
    // Use a consistent round-robin distribution for all file sizes
    std::vector<std::vector<size_t>> thread_expressions(n_threads_);
    
    // Distribute expressions in a round-robin fashion
    for (size_t i = 0; i < expressions_.size(); ++i) {
        size_t thread_id = i % n_threads_;
        thread_expressions[thread_id].push_back(i);
    }
    
    std::vector<pthread_t> threads(n_threads_);
    std::vector<ThreadData*> thread_data;
    
    // Create thread data with assigned expressions
    for (size_t i = 0; i < n_threads_; ++i) {
        if (thread_expressions[i].empty()) continue;
        
        // Create with simplified ThreadData structure
        ThreadData* data = new ThreadData{
            i,                // thread_id
            this,             // solver
            {0, 0},           // stats
            {}                // expression_indices (empty vector)
        };
        data->expression_indices = thread_expressions[i];
        thread_data.push_back(data);
        
        debug_print("Thread " + std::to_string(i) + " assigned " + 
                   std::to_string(thread_expressions[i].size()) + " expressions: ");
        
        // Debug print to show which expressions are assigned to each thread
        if (DEBUG) {
            std::string exprs;
            for (size_t idx : thread_expressions[i]) {
                exprs += std::to_string(idx) + " ";
            }
            debug_print("  Expressions: " + exprs);
        }
    }
    
    // Create all threads (let them run in parallel)
    for (size_t i = 0; i < thread_data.size(); ++i) {
        ThreadData* data = thread_data[i];
        
        // CREATING PTHREADS
        int result = pthread_create(&threads[i], nullptr, &NSatSolver::ThreadSolve, data);
        if (result != 0) {
            std::cerr << "Failed to create thread " << i << std::endl;
            delete data;
            thread_data[i] = nullptr;
        }
    }
    
    // Now join all threads and collect results
    std::vector<ThreadStats> stats;
    for (size_t i = 0; i < threads.size() && i < thread_data.size(); ++i) {
        if (thread_data[i] == nullptr) continue;
        
        // JOINING PTHREADS
        void* thread_result;
        pthread_join(threads[i], &thread_result);
        if (thread_result) {
            stats.push_back(*static_cast<ThreadStats*>(thread_result));
            delete static_cast<ThreadStats*>(thread_result);
        }
    }

    // Sort results by thread ID
    std::sort(stats.begin(), stats.end(), 
        [](const ThreadStats& a, const ThreadStats& b) { 
            return a.thread_id < b.thread_id; 
        });

    // Print results in order
    // ENTERING CRITICAL SECTION
    {
        ThreadMutexGuard guard(mutex_);
        for (const auto& stat : stats) {
            std::cout << std::setw(6) << stat.thread_id
                    << std::setw(5) << stat.sat_count
                    << std::setw(7) << stat.unsat_count
                    << std::endl;
        }
    }
    // EXITING CRITICAL SECTION

    PrintResults(stats);
}

void NSatSolver::PrintResults(const std::vector<ThreadStats>& stats) {
    size_t total_sat = 0;
    size_t total_unsat = 0;
    
    for (const auto& stat : stats) {
        total_sat += stat.sat_count;
        total_unsat += stat.unsat_count;
    }
    
    // ENTERING CRITICAL SECTION
    {
        ThreadMutexGuard guard(mutex_);
        std::cout << std::setw(6) << "Total"
                 << std::setw(5) << total_sat
                 << std::setw(7) << total_unsat
                 << std::endl;
    }
    // EXITING CRITICAL SECTION
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] 
                  << " <number of threads> <input file> <number of variables>" 
                  << std::endl;
        return 1;
    }

    try {
        std::size_t n_threads = std::stoul(argv[1]);
        std::string filename = argv[2];
        std::size_t n_vars = std::stoul(argv[3]);

        NSatSolver solver(n_threads, filename, n_vars);
        solver.Solve();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}