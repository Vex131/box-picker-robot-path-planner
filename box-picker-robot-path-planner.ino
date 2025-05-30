// Define the dimensions of the environment grid
#define ROWS 11       // Number of rows in the grid
#define COLS 11       // Number of columns in the grid

// Define a structure to represent a coordinate point
struct Point {
  int row, col;
};

// Define movement deltas for Up, Right, Down, Left directions
const int dr[4] = {-1, 0, 1, 0}; // Row change for each direction
const int dc[4] = {0, 1, 0, -1}; // Column change for each direction

// Direction names for human-readable output
const char* directionNames[4] = {"UP", "RIGHT", "DOWN", "LEFT"};

// 2D grid representing the environment
// 1 = walkable path; 0 = obstacle or wall
int grid[ROWS][COLS] = {
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

// Visited array for BFS search
bool visited[ROWS][COLS];

// Parent array to reconstruct path from BFS
Point parent[ROWS][COLS];

// Check if a given cell (r, c) is inside the grid and walkable
bool isValid(int r, int c) {
  return r >= 0 && r < ROWS && c >= 0 && c < COLS && grid[r][c] == 1;
}

// Initial facing direction of the robot: RIGHT (index 1)
int currentFacingDir = 1;

// Breadth-First Search to find a path from 'start' to 'end'
bool bfs(Point start, Point end) {
  // Clear previous search data
  memset(visited, 0, sizeof(visited));
  memset(parent, -1, sizeof(parent)); // Initialize all parent positions to -1

  // Create a queue for BFS
  Point queue[ROWS * COLS];
  int front = 0, back = 0;

  // Add the start point to the queue
  queue[back++] = start;
  visited[start.row][start.col] = true;

  // Loop through queue until it's empty or we find the end
  while (front < back) {
    Point curr = queue[front++];

    // If we reached the destination
    if (curr.row == end.row && curr.col == end.col)
      return true;

    // Check all 4 directions
    for (int i = 0; i < 4; ++i) {
      int nr = curr.row + dr[i];
      int nc = curr.col + dc[i];

      // If the next cell is valid and not visited
      if (isValid(nr, nc) && !visited[nr][nc]) {
        visited[nr][nc] = true;
        parent[nr][nc] = curr;       // Store where we came from
        queue[back++] = {nr, nc};    // Add next cell to the queue
      }
    }
  }

  // Path not found
  return false;
}

// Reconstructs the path from BFS and prints movement commands
bool moveAndPrint(Point &current, Point target) {
  if (!bfs(current, target)) {
    Serial.println("No path found.\n");
    return false;
  }

  // Reconstruct path by following parent links backward
  Point path[ROWS * COLS];
  int length = 0;
  Point temp = target;

  while (!(temp.row == -1 && temp.col == -1)) {
    path[length++] = temp;
    temp = parent[temp.row][temp.col];
  }

  // Move along the path from current to target
  for (int i = length - 2; i >= 0; i--) {
    Point next = path[i];

    // Calculate direction to move
    int dRow = next.row - current.row;
    int dCol = next.col - current.col;

    // Determine which direction (UP, RIGHT, DOWN, LEFT) we are moving
    int newDir = -1;
    for (int j = 0; j < 4; ++j) {
      if (dr[j] == dRow && dc[j] == dCol) {
        newDir = j;
        break;
      }
    }

    const char* dirName = directionNames[newDir];

    // Determine robot action based on current vs new direction
    const char* action = "";
    if (newDir == currentFacingDir)
      action = "Forward";
    else if ((currentFacingDir + 1) % 4 == newDir)
      action = "Turn Right";
    else if ((currentFacingDir + 3) % 4 == newDir)
      action = "Turn Left";
    else
      action = "Reverse";

    // Determine side grid values (left, center, right)
    int leftDir = (newDir + 3) % 4;
    int rightDir = (newDir + 1) % 4;

    int leftRow = next.row + dr[leftDir];
    int leftCol = next.col + dc[leftDir];
    int rightRow = next.row + dr[rightDir];
    int rightCol = next.col + dc[rightDir];

    int leftVal = -1, centerVal = -1, rightVal = -1;

    if (leftRow >= 0 && leftRow < ROWS && leftCol >= 0 && leftCol < COLS)
      leftVal = grid[leftRow][leftCol];
    if (next.row >= 0 && next.row < ROWS && next.col >= 0 && next.col < COLS)
      centerVal = grid[next.row][next.col];
    if (rightRow >= 0 && rightRow < ROWS && rightCol >= 0 && rightCol < COLS)
      rightVal = grid[rightRow][rightCol];

    // Print path step details
    Serial.print("(");
    Serial.print(current.row); Serial.print(", ");
    Serial.print(current.col); Serial.print(") > (");
    Serial.print(next.row); Serial.print(", ");
    Serial.print(next.col); Serial.print(")");

    Serial.print("| Grid: (");
    Serial.print(leftVal); Serial.print(", ");
    Serial.print(centerVal); Serial.print(", ");
    Serial.print(rightVal); Serial.print(")");

    Serial.print("| Facing: ");
    Serial.print(dirName);

    Serial.print("| Act: ");
    Serial.println(action);

    // Update position and facing
    current = next;
    currentFacingDir = newDir;

    delay(200); // Add a delay for visualization or simulation pacing
  }

  return true;
}

// Find a walkable adjacent tile near the drop zone
bool findDropAdjacent(Point dropPoint, Point &dropTarget) {
  for (int i = 0; i < 4; ++i) {
    int nr = dropPoint.row + dr[i];
    int nc = dropPoint.col + dc[i];
    if (isValid(nr, nc)) {
      dropTarget = {nr, nc};
      return true;
    }
  }
  return false;
}

// Arduino setup function: runs once at startup
void setup() {
  Serial.begin(115200);     // Start serial communication
  delay(1000);              // Wait for serial to initialize
  Serial.println("=== Robot Path Planning ===");

  Point start = {9, 0};     // Initial starting position of robot
  Point dropZone = {5, 5};  // Central drop zone location

  // Locations of all boxes to pick up
  Point boxes[] = {
    {3, 3},
    {3, 6},
    {5, 7},
    {9, 9}
  };

  // Current position tracker
  Point current = start;

  Serial.print("Starting at (");
  Serial.print(current.row); Serial.print(", ");
  Serial.print(current.col); Serial.println(")");

  // Process each box
  for (int i = 0; i < 4; i++) {
    Serial.print("\n>>> Moving to Box ");
    Serial.println(i + 1);
    Serial.flush();

    // Go to box location
    bool picked = moveAndPrint(current, boxes[i]);

    if (picked) {
      Serial.print("Box ");
      Serial.print(i + 1);
      Serial.println(" picked");

      // Find a place to drop the box
      Point dropTarget;
      if (findDropAdjacent(dropZone, dropTarget)) {
        Serial.print(">>> Moving to Drop Zone with Box ");
        Serial.println(i + 1);
        Serial.flush();

        bool dropped = moveAndPrint(current, dropTarget);

        if (dropped) {
          Serial.print("Box ");
          Serial.print(i + 1);
          Serial.println(" dropped");
        } else {
          Serial.print("Could not reach drop position for Box ");
          Serial.println(i + 1);
        }
      } else {
        Serial.println("No valid drop position found around the container.");
      }
    }
  }

  Serial.println("\n=== All Tasks Complete ===");
}


void loop() {
  // Idle
}
