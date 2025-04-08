#ifndef BANKERS_RESOURCE_MANAGER_H_
#define BANKERS_RESOURCE_MANAGER_H_

#include <thread_mutex.h>
#include <vector>
#include <string>
#include <algorithm>

class BankersResourceManager {
 public:
  // Constructor
  BankersResourceManager(const std::vector<std::size_t>& available);

  // Register a new process with its maximum resource requirements
  void AddMax(const std::vector<std::size_t>& max_demand);

  // Request resources for a process
  bool Request(std::size_t process_id, const std::vector<std::size_t>& request);

  // Release resources held by a process
  // bool Release(std::size_t process_id, const std::vector<std::size_t>& release);
  
  // Release all resources held by a process
  bool Release(std::size_t process_id);

  // Check if the current state is safe
  bool IsSafeState() const;

  // Get string representation of the system state
  std::string GetStateString() const;

  // Getters
  std::vector<std::size_t> GetAvailable() const;
  std::vector<std::size_t> GetAllocation(std::size_t process_id) const;
  std::vector<std::size_t> GetMax(std::size_t process_id) const;

 private:
  // Helper method to check if a request is valid
  bool IsRequestValid(std::size_t process_id, const std::vector<std::size_t>& request);
  
  // Helper method to check if a process can complete with given resources
  bool CanProcessComplete(std::size_t pid, const std::vector<std::size_t>& available_work) const;
  
  // Helper method to find a safe execution sequence
  bool FindSafeSequence(std::vector<size_t>& safe_sequence);
  
  // Helper method to display the current state of a process
  void DisplayProcessState(std::size_t process_id);

  // Available resources
  std::vector<std::size_t> available_;
  
  // Maximum demand for each process
  std::vector<std::vector<std::size_t>> max_;
  
  // Current allocation for each process
  std::vector<std::vector<std::size_t>> allocation_;
  
  // Number of resource types
  std::size_t n_resources_;
  
  // Mutex for thread safety
  mutable ThreadMutex mutex_;
};

#endif  // BANKERS_RESOURCE_MANAGER_H_