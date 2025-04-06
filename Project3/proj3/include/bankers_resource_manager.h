
#ifndef PROJ3_INCLUDE_BANKERS_RESOURCE_MANAGER_H_
#define PROJ3_INCLUDE_BANKERS_RESOURCE_MANAGER_H_

#include <cstddef>  // for std::size_t
#include <string>
#include <vector>
#include <algorithm>  // Include this for std::all_of used in bankers_thread.cc
#include <thread_mutex.h>  // Include thread mutex header

class BankersResourceManager {
 public:
  // Constructor with available resources
  explicit BankersResourceManager(const std::vector<std::size_t>& available);

  // Add a new process with maximum resource needs
  void AddMax(const std::vector<std::size_t>& max_demand);

  // Request resources for a process
  // Returns true if request is granted
  bool Request(std::size_t process_id, const std::vector<std::size_t>& request);

  // Release all resources held by a process
  bool Release(std::size_t process_id);

  // Release specific resources from a process
  // This was missing in the header but implemented in the .cc file
  bool Release(std::size_t process_id, const std::vector<std::size_t>& release);

  // Get available resources
  std::vector<std::size_t> GetAvailable() const;

  // Get allocation for a specific process
  std::vector<std::size_t> GetAllocation(std::size_t process_id) const;

  // Get maximum needs for a specific process
  std::vector<std::size_t> GetMax(std::size_t process_id) const;

  // Get a string representation of the current state
  std::string GetStateString() const;

 private:
  // Mutex for thread synchronization
  mutable ThreadMutex mutex_;
  
  // Check if system is in a safe state
  bool IsSafeState() const;

  // Available resources
  std::vector<std::size_t> available_;

  // Maximum demand of each process
  std::vector<std::vector<std::size_t>> max_;

  // Current allocation to each process
  std::vector<std::vector<std::size_t>> allocation_;

  // Number of resource types
  std::size_t n_resources_;
};

#endif  // PROJ3_INCLUDE_BANKERS_RESOURCE_MANAGER_H_