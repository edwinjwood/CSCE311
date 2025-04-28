#ifndef N_SAT_SOLVER_H_
#define N_SAT_SOLVER_H_

#include <string>
#include <vector>
#include <pthread.h>

struct ThreadStats {
    size_t thread_id;  // Needed for sorting results
    size_t sat_count;
    size_t unsat_count;
    
    ThreadStats() : thread_id(0), sat_count(0), unsat_count(0) {}
    ThreadStats(size_t sat, size_t unsat) : thread_id(0), sat_count(sat), unsat_count(unsat) {}
};

// Forward declaration
class NSatSolver;

struct ThreadData {
    size_t thread_id;
    // These two fields can be removed as they're not used in the round-robin approach
    // size_t start_idx;  
    // size_t end_idx;
    NSatSolver* solver;
    ThreadStats stats;
    std::vector<size_t> expression_indices;
};

class ThreadMutexGuard {
public:
    explicit ThreadMutexGuard(pthread_mutex_t& mutex) : mutex_(mutex) {
        pthread_mutex_lock(&mutex_);
    }
    
    ~ThreadMutexGuard() {
        pthread_mutex_unlock(&mutex_);
    }
    
private:
    pthread_mutex_t& mutex_;
};

class NSatSolver {
public:
    NSatSolver(std::size_t n_threads, const std::string& filename, std::size_t n_vars);
    
    void Solve();
    
private:
    void LoadExpressions();
    void PrintResults(const std::vector<ThreadStats>& stats);
    
    static void* ThreadSolve(void* arg);
    
    friend struct ThreadData;
    
    std::size_t n_threads_;
    std::string filename_;
    std::size_t n_vars_;
    std::vector<std::string> expressions_;
    pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;
};

#endif  // N_SAT_SOLVER_H_