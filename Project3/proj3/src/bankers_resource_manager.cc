// Implementation of Banker's Algorithm for deadlock avoidance
#include <bankers_resource_manager.h>
#include <algorithm>  // for std::min
#include <sstream>
#include <iostream>   // for debug output

BankersResourceManager::BankersResourceManager(const std::vector<std::size_t>& available)
    : available_(available), 
      n_resources_(available_.size()) {
  // Constructor initializes with the available resources vector
  // Each element represents the number of available instances of a resource type
}

void BankersResourceManager::AddMax(const std::vector<std::size_t>& max_demand) {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);

  // Validate max_demand size matches our resource types
  if (max_demand.size() != n_resources_)
    return;  // Simply return without adding if size mismatch

  // Register a new process with its maximum resource requirements
  max_.push_back(max_demand);
  // Initialize with zero allocation since process hasn't requested resources yet
  allocation_.push_back(std::vector<std::size_t>(n_resources_, 0));
  
  // Debug output showing the newly registered process
  std::cout << "New process registered with max demand: {";
  for (std::size_t i = 0; i < max_demand.size(); ++i) {
    std::cout << max_demand[i];
    if (i < max_demand.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;
}

bool BankersResourceManager::Request(std::size_t process_id,
                                   const std::vector<std::size_t>& request) {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);

  // Debug output showing which process is requesting what resources
  std::cout << "Thread " << process_id << " requested: {";
  for (std::size_t i = 0; i < request.size(); ++i) {
    std::cout << request[i];
    if (i < request.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  // Validate request size and process_id
  if (request.size() != n_resources_ || process_id >= allocation_.size())
    return false;

  // Display the current need (Max - Allocation) for this process
  std::cout << "Need: {";
  for (std::size_t i = 0; i < n_resources_; ++i) {
    std::cout << (max_[process_id][i] - allocation_[process_id][i]);
    if (i < n_resources_ - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;
  
  // Show currently available resources
  std::cout << "Available: {";
  for (std::size_t i = 0; i < available_.size(); ++i) {
    std::cout << available_[i];
    if (i < available_.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  // Step 1: Check if request exceeds the process's remaining need
  for (std::size_t i = 0; i < n_resources_; ++i) {
    if (request[i] > max_[process_id][i] - allocation_[process_id][i]) {
      std::cout << "Request exceeds max need. Request denied." << std::endl;
      return false;  // Process is requesting more than it declared it would need
    }
  }

  // Step 2: Check if enough resources are currently available
  for (std::size_t i = 0; i < n_resources_; ++i) {
    if (request[i] > available_[i]) {
      std::cout << "Not available. Request denied." << std::endl;
      return false;  // Not enough resources available right now
    }
  }

  // Step 3: Try to allocate resources (tentatively)
  // Save current state in case we need to roll back
  std::vector<std::size_t> saved_available = available_;
  std::vector<std::size_t> saved_allocation = allocation_[process_id];

  // Update state tentatively - pretend we grant the request
  for (std::size_t i = 0; i < n_resources_; ++i) {
    available_[i] -= request[i];
    allocation_[process_id][i] += request[i];
  }

  // Step 4: Check if system remains in a safe state after allocation
  // A safe state means all processes can eventually complete without deadlock

  // Run safety check to get the sequence of safe process execution
  const size_t n_processes = allocation_.size();
  std::vector<bool> can_complete(n_processes, false);
  std::vector<size_t> safe_sequence;
  auto available_work = available_;

  bool found_completable_process;
  bool is_safe = true;

  do {
    found_completable_process = false;

    for (size_t pid = 0; pid < n_processes; ++pid) {
      // Skip processes we've already determined can complete
      if (can_complete[pid]) 
        continue;

      // Check if this process's remaining needs can be satisfied
      bool has_enough_resources = true;
      for (size_t r = 0; r < n_resources_; ++r) {
        if (max_[pid][r] - allocation_[pid][r] > available_work[r]) {
          has_enough_resources = false;
          break;
        }
      }

      if (has_enough_resources) {
        // This process can complete, so simulate it finishing and releasing resources
        for (size_t r = 0; r < n_resources_; ++r) {
          available_work[r] += allocation_[pid][r];
        }
        can_complete[pid] = true;
        found_completable_process = true;
        // Add to our safe execution sequence
        safe_sequence.push_back(pid);
      }
    }
  } while (found_completable_process);

  // Check if all processes can complete (system is in safe state)
  for (bool process_can_complete : can_complete) {
    if (!process_can_complete) {
      is_safe = false;
      break;
    }
  }

  if (!is_safe) {
    // Step 5a: If not safe, restore previous state to prevent deadlock
    available_ = saved_available;
    allocation_[process_id] = saved_allocation;
    std::cout << "Unsafe state. Request denied." << std::endl;
    return false;  // Cannot grant request as it could lead to deadlock
  }

  // Step 5b: If safe, keep the allocation and show execution order
  std::cout << "Safe. Request allocated. Order: {";
  for (std::size_t i = 0; i < safe_sequence.size(); ++i) {
    std::cout << "P" << safe_sequence[i];
    if (i < safe_sequence.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  // Show updated available resources
  std::cout << "Available: {";
  for (std::size_t i = 0; i < available_.size(); ++i) {
    std::cout << available_[i];
    if (i < available_.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;
  
  // Show updated need for this process
  std::cout << "Need: {";
  for (std::size_t i = 0; i < n_resources_; ++i) {
    std::cout << (max_[process_id][i] - allocation_[process_id][i]);
    if (i < n_resources_ - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  return true;
}

bool BankersResourceManager::Release(std::size_t process_id,
                                   const std::vector<std::size_t>& release) {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);

  // Debug output
  std::cout << "Thread " << process_id << " releasing specific resources: {";
  for (std::size_t i = 0; i < release.size(); ++i) {
    std::cout << release[i];
    if (i < release.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  // Validate release size and process_id
  if (release.size() != n_resources_ || process_id >= allocation_.size())
    return false;

  // Check if process is trying to release more resources than it has
  for (std::size_t i = 0; i < n_resources_; ++i) {
    if (release[i] > allocation_[process_id][i])
      return false;  // Can't release more than what's allocated
  }

  // Release resources - decrease allocation and increase availability
  for (std::size_t i = 0; i < n_resources_; ++i) {
    allocation_[process_id][i] -= release[i];
    available_[i] += release[i];
  }

  // Show updated available resources
  std::cout << "Updated Available: {";
  for (std::size_t i = 0; i < available_.size(); ++i) {
    std::cout << available_[i];
    if (i < available_.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  return true;
}

bool BankersResourceManager::Release(std::size_t process_id) {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);

  // Debug output
  std::cout << "Thread " << process_id << " releasing all resources" << std::endl;

  // Validate process_id
  if (process_id >= allocation_.size())
    return false;

  // Release all resources held by the process
  for (std::size_t i = 0; i < n_resources_; ++i) {
    available_[i] += allocation_[process_id][i];  // Return resources to available pool
    allocation_[process_id][i] = 0;               // Clear process allocation
  }

  // Show updated available resources
  std::cout << "Updated Available: {";
  for (std::size_t i = 0; i < available_.size(); ++i) {
    std::cout << available_[i];
    if (i < available_.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  return true;
}

bool BankersResourceManager::IsSafeState() const {
  // This method implements the safety algorithm of Banker's Algorithm
  // It determines if a state is safe by checking if all processes can eventually complete
  
  const size_t n_processes = allocation_.size();
  
  // Track which processes can complete
  std::vector<bool> can_complete(n_processes, false);
  
  // Work array - starts with currently available resources
  // This simulates resource availability as processes complete
  auto available_work = available_;
  
  // Keep looking for processes that can complete with available resources
  bool found_completable_process;
  do {
    found_completable_process = false;
    
    for (size_t pid = 0; pid < n_processes; ++pid) {
      if (can_complete[pid]) 
        continue;
      
      bool has_enough_resources = true;
      for (size_t r = 0; r < n_resources_; ++r) {
        if (max_[pid][r] - allocation_[pid][r] > available_work[r]) {
          has_enough_resources = false;
          break;
        }
      }
      
      if (has_enough_resources) {
        for (size_t r = 0; r < n_resources_; ++r) {
          available_work[r] += allocation_[pid][r];
        }
        can_complete[pid] = true;
        found_completable_process = true;
      }
    }
  } while (found_completable_process);
  
  // System is safe if all processes can complete
  for (bool process_can_complete : can_complete) {
    if (!process_can_complete)
      return false;
  }
  return true;
}

std::string BankersResourceManager::GetStateString() const {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);
  
  std::stringstream ss;
  
  ss << "Available: ";
  for (std::size_t i = 0; i < available_.size(); ++i) {
    ss << available_[i];
    if (i < available_.size() - 1) ss << " ";
  }
  ss << "\n";
  
  // Show details for each process
  for (std::size_t i = 0; i < allocation_.size(); ++i) {
    ss << "Process " << i << ":\n";
    
    ss << "  Max: ";
    for (std::size_t j = 0; j < max_[i].size(); ++j) {
      ss << max_[i][j];
      if (j < max_[i].size() - 1) ss << " ";
    }
    
    ss << "\n  Allocation: ";
    for (std::size_t j = 0; j < allocation_[i].size(); ++j) {
      ss << allocation_[i][j];
      if (j < allocation_[i].size() - 1) ss << " ";
    }
    
    ss << "\n  Need: ";
    for (std::size_t j = 0; j < n_resources_; ++j) {
      ss << (max_[i][j] - allocation_[i][j]);
      if (j < n_resources_ - 1) ss << " ";
    }
    ss << "\n";
  }
  
  return ss.str();
}

std::vector<std::size_t> BankersResourceManager::GetAvailable() const {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);
  return available_;
}

std::vector<std::size_t> BankersResourceManager::GetAllocation(
    std::size_t process_id) const {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);
  
  if (process_id >= allocation_.size())
    return std::vector<std::size_t>();  // Return empty if invalid process
  
  return allocation_[process_id];
}

std::vector<std::size_t> BankersResourceManager::GetMax(
    std::size_t process_id) const {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);
  
  if (process_id >= max_.size())
    return std::vector<std::size_t>();  // Return empty if invalid process
  
  return max_[process_id];
}