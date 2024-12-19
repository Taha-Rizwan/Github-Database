# Github / Database with RedBlack & AVL


---

## Features

### Red-Black Tree
- A self-balancing binary search tree.
- Ensures logarithmic time complexity for search, insertion, and deletion.
- Each node includes:
  - `data`: Node content.
  - `left` and `right`: Child references (or file paths if stored externally).
  - `parent`: Parent reference.
  - `color`: Either RED or BLACK.
  - `lineNumbers`: A vector to store associated line numbers from a CSV file.
### AVL Tree
- A self-balancing binary search tree.
- Ensures logarithmic time complexity for search, insertion, and deletion.
- Each node includes:
  - `data`: Node content.
  - `left` and `right`: Child references (or file paths if stored externally).
  - `parent`: Parent reference.
  - `height`: Height of current node.
  - `lineNumbers`: A vector to store associated line numbers from a CSV file.

### File-Based Caching System
- Dynamically manages file I/O to minimize RAM usage.
- Keeps the most frequently accessed nodes in memory.
- Uses a combination of **LRU (Least Recently Used)** caching and **hash-based probing** to efficiently handle data.
- Implements:
  - Quadratic probing with secondary hashing for collision resolution.
  - Automatic eviction of old nodes when cache reaches capacity.
  - Dirty writing mechanism to only write modified nodes to files.
### Version Control
- Reverts to older versions of current branch using efficient data management
### Merging using Merkle Tree
- Uses merkle trees to handle data efficiently when transferring changes between branches
---

## How It Works

### CSV Integration
- Reads a specified column from a CSV file.
- Handles cells containing commas and spaces appropriately, preserving full column data.

### File Storage
- Each node is written to its own file in a structured format:
  ```
  data
  parentPath
  leftPath
  rightPath
  color
  lineNumber1,lineNumber2,...
  ```
- Nodes are lazily loaded into memory when needed.

### HashTable
- Utilizes a hash table for efficient node lookups.
- Customizable hash function optimized for performance based on cache size.
- Supports dynamic resizing and collision resolution.

---


## Example Usage

### CSV Integration
```cpp
#include "Menu.h"

int main() {
   Menu m;
    m.main();
    return 0;
}
```



## Performance Optimization

### Current Strategies
- **Quadratic Probing**: Improved collision resolution.
- **Dirty Writing**: Avoid redundant file writes for unmodified nodes.
- **LRU Cache Eviction**: Keeps only active nodes in memory.
- **Merkle tree when commiting**: Transfers only changes between branches.



## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/Taha-Rizwan/red-black-tree-cache.git
   ```

2. Build the project:
   ```bash
   g++ main.cpp -o main
   ```

3. Run the application:
   ```bash
   ./main
   ```

---

