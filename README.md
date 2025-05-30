# box-picker-robot-path-planner
This Arduino sketch simulates a simple warehouse-style grid where a robot must collect boxes from defined locations and drop them off at a central location. It uses Breadth-First Search (BFS) to find the shortest path to each target sequentially.

⚠️ This code focuses only on pathfinding logic and does not contain hardware control or motor commands. It's intended as the planning layer for an Arduino-based robot.

### Features
- Simulates a grid-based environment using a 2D array
- Implements Breadth-First Search (BFS) for efficient path planning
- Plans path from the robot to each box, then to the drop zone
- Avoids obstacles in the grid (represented by `0`s)


### Example Map Layout
The map is defined as a 2D character array in the code

```cpp
int grid[V][V] = {
{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
};
```

### Legend:
  - 0 – Wall or obstacle
  - 1 – Walkable cell

### Key Coordinates
  - Starting Position: grid[9][0]
  - Drop Zone: grid[5][5]
  - Box Locations:
    - grid[3][3]
    - grid[3][6]
    - grid[5][7]
    - grid[9][9]

### How It Works:
  - The robot starts at grid[9][0]
  - It calculates the shortest path to the nearest box
  - Then it calculates a path from the box to the drop zone
  - Repeats for all boxes in order

