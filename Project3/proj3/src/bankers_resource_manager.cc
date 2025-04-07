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
  // std::cout << "New process registered with max demand: {";
  // for (std::size_t i = 0; i < max_demand.size(); ++i) {
  //   std::cout << max_demand[i];
  //   if (i < max_demand.size() - 1) std::cout << " ";
  // }
  // std::cout << "}" << std::endl;
}

bool BankersResourceManager::Request(std::size_t process_id, const std::vector<std::size_t>& request) {
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

  // Display current state
  DisplayProcessState(process_id);
  
  // Steps 1 & 2: Validation checks
  if (!IsRequestValid(process_id, request))
    return false;

  // Step 3: Save state and tentatively allocate resources
  auto saved_state = std::make_pair(available_, allocation_[process_id]);
  
  // Update state tentatively
  for (std::size_t i = 0; i < n_resources_; ++i) {
    available_[i] -= request[i];
    allocation_[process_id][i] += request[i];
  }

  // Step 4: Check if system remains in a safe state
  std::vector<size_t> safe_sequence;
  bool is_safe = FindSafeSequence(safe_sequence);

  if (!is_safe) {
    // Step 5a: If not safe, restore previous state
    available_ = saved_state.first;
    allocation_[process_id] = saved_state.second;
    std::cout << "   Unsafe state. Request denied." << std::endl;
    return false;
  }

  // Step 5b: If safe, keep the allocation and show execution order
  std::cout << "   Safe. Request allocated. Order: {";
  for (std::size_t i = 0; i < safe_sequence.size(); ++i) {
    std::cout << "P" << safe_sequence[i];
    if (i < safe_sequence.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  // Show updated state
  DisplayProcessState(process_id);
  
  return true;
}

bool BankersResourceManager::Release(std::size_t process_id, const std::vector<std::size_t>& release) {
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
  std::cout << "   Updated Available: {";
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

  // Validate process_id first
  if (process_id >= allocation_.size())
    return false;
    
  // Save current allocation before releasing (to show what was released)
  auto released_resources = allocation_[process_id];

  // Debug output
  std::cout << "Thread " << process_id << " releasing all resources: {";
  for (std::size_t i = 0; i < n_resources_; ++i) {
    std::cout << released_resources[i];
    if (i < n_resources_ - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  // Release all resources held by the process
  for (std::size_t i = 0; i < n_resources_; ++i) {
    available_[i] += allocation_[process_id][i];  // Return resources to available pool
    allocation_[process_id][i] = 0;               // Clear process allocation
  }

  // Show updated available resources
  std::cout << "   Updated Available: {";
  for (std::size_t i = 0; i < available_.size(); ++i) {
    std::cout << available_[i];
    if (i < available_.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;

  return true;
}

bool BankersResourceManager::IsRequestValid(std::size_t process_id, const std::vector<std::size_t>& request) {
  for (std::size_t i = 0; i < n_resources_; ++i) {
    // Check if request exceeds the process's remaining need
    if (request[i] > max_[process_id][i] - allocation_[process_id][i]) {
      std::cout << "   Request exceeds max need. Request denied." << std::endl;
      return false;
    }
    
    // Check if enough resources are currently available
    if (request[i] > available_[i]) {
      std::cout << "   Not available. Request denied." << std::endl;
      return false;
    }
  }
  return true;
}

bool BankersResourceManager::CanProcessComplete(std::size_t pid, const std::vector<std::size_t>& available_work) const {
  for (std::size_t r = 0; r < n_resources_; ++r) {
    if (max_[pid][r] - allocation_[pid][r] > available_work[r]) {
      return false;
    }
  }
  return true;
}

bool BankersResourceManager::FindSafeSequence(std::vector<size_t>& safe_sequence) {
  const size_t n_processes = allocation_.size();
  std::vector<bool> can_complete(n_processes, false);
  safe_sequence.clear();
  
  auto available_work = available_;
  
  // Find a safe execution sequence
  bool found_completable;
  do {
    found_completable = false;
    
    for (size_t pid = 0; pid < n_processes; ++pid) {
      if (can_complete[pid]) 
        continue;
      
      if (CanProcessComplete(pid, available_work)) {
        // Process can complete - simulate resource release
        for (size_t r = 0; r < n_resources_; ++r) {
          available_work[r] += allocation_[pid][r];
        }
        can_complete[pid] = true;
        found_completable = true;
        safe_sequence.push_back(pid);
      }
    }
  } while (found_completable);
  
  // Check if all processes can complete
  for (bool process_complete : can_complete) {
    if (!process_complete) {
      return false;
    }
  }
  return true;
}

void BankersResourceManager::DisplayProcessState(std::size_t process_id) {
  // Display the current need (Max - Allocation) for this process
  std::cout << "   Need: {";
  for (std::size_t i = 0; i < n_resources_; ++i) {
    std::cout << (max_[process_id][i] - allocation_[process_id][i]);
    if (i < n_resources_ - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;
  
  // Show currently available resources
  std::cout << "   Available: {";
  for (std::size_t i = 0; i < available_.size(); ++i) {
    std::cout << available_[i];
    if (i < available_.size() - 1) std::cout << " ";
  }
  std::cout << "}" << std::endl;
}

bool BankersResourceManager::IsSafeState() const {
  // Use the FindSafeSequence method but discard the sequence
  std::vector<size_t> dummy_sequence;
  return const_cast<BankersResourceManager*>(this)->FindSafeSequence(dummy_sequence);
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

std::vector<std::size_t> BankersResourceManager::GetAllocation(std::size_t process_id) const {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);
  
  if (process_id >= allocation_.size())
    return std::vector<std::size_t>();  // Return empty if invalid process
  
  return allocation_[process_id];
}

std::vector<std::size_t> BankersResourceManager::GetMax(std::size_t process_id) const {
  // Create a mutex guard for thread safety
  ThreadMutexGuard guard(mutex_);
  
  if (process_id >= max_.size())
    return std::vector<std::size_t>();  // Return empty if invalid process
  
  return max_[process_id];
}