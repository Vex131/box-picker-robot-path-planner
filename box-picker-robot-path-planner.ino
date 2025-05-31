#define ROWS 11       // Number of rows in the grid (environment)
#define COLS 11       // Number of columns in the grid

// Structure to represent a coordinate point in the grid
struct Point {
  int row, col;
};

// Direction arrays for movement: up, right, down, left
const int dr[4] = {-1, 0, 1, 0}; // Row delta
const int dc[4] = {0, 1, 0, -1}; // Column delta
const char* directionNames[4] = {"UP", "RIGHT", "DOWN", "LEFT"};

// 2D grid representing the map/environment
// 1 = walkable cell, 0 = obstacle or wall
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

// Boolean array to keep track of visited cells during BFS
bool visited[ROWS][COLS];

// Stores the parent of each cell to reconstruct the path
Point parent[ROWS][COLS];

// Checks if a cell (r, c) is within bounds and is walkable
bool isValid(int r, int c) {
  return r >= 0 && r < ROWS && c >= 0 && c < COLS && grid[r][c] == 1;
}

int currentFacingDir = 1; // 0=UP, 1=RIGHT, 2=DOWN, 3=LEFT (starts facing RIGHT)

// Breadth-First Search to find a path from start to end
// Returns true if a path is found and fills in the 'parent' array
bool bfs(Point start, Point end) {
  memset(visited, 0, sizeof(visited));     // Clear visited cells
  memset(parent, -1, sizeof(parent));      // Reset parent links

  Point queue[ROWS * COLS]; // BFS queue
  int front = 0, back = 0;
  queue[back++] = start;                    // Enqueue start position
  visited[start.row][start.col] = true;    // Mark as visited

  while (front < back) {
    Point curr = queue[front++];

    if (curr.row == end.row && curr.col == end.col)
      return true;

    for (int i = 0; i < 4; ++i) {
      int nr = curr.row + dr[i];
      int nc = curr.col + dc[i];

      if (isValid(nr, nc) && !visited[nr][nc]) {
        visited[nr][nc] = true;
        parent[nr][nc] = curr;
        queue[back++] = {nr, nc};
      }
    }
  }

  return false; // No path found
}

// Moves from the current position to the target and prints the steps
bool moveAndPrint(Point &current, Point target) {
  if (!bfs(current, target)) {
    Serial.println("No path found.\n");
    return false;
  }

  // Reconstruct path
  Point path[ROWS * COLS];
  int length = 0;
  Point temp = target;

  while (!(temp.row == -1 && temp.col == -1)) {
    path[length++] = temp;
    temp = parent[temp.row][temp.col];
  }

  // Move forward along path
  for (int i = length - 2; i >= 0; i--) {
    Point next = path[i];

    // when arriving at the box position
    // The box is the target, which is path[0], so when i == 0, next == target
    if (i == 0) {
      //Serial.println("Approching Target");
      Serial.flush();
    }

    // Calculate direction
    int dRow = next.row - current.row;
    int dCol = next.col - current.col;

    int newDir = -1;
    for (int j = 0; j < 4; ++j) {
      if (dr[j] == dRow && dc[j] == dCol) {
        newDir = j;
        break;
      }
    }

    const char* dirName = directionNames[newDir];

    // Determine action based on facing direction
    const char* action = "";
    if (newDir == currentFacingDir) {
      action = "Forward";
    } else if ((currentFacingDir + 1) % 4 == newDir) {
      action = "Turn Right";
    } else if ((currentFacingDir + 3) % 4 == newDir) {
      action = "Turn Left";
    } else {
      action = "Reverse";
    }

    // Calculate left and right directions relative to newDir
    int leftDir = (newDir + 3) % 4;
    int rightDir = (newDir + 1) % 4;

    // Calculate grid positions for left, center, right
    int leftRow = next.row + dr[leftDir];
    int leftCol = next.col + dc[leftDir];
    int rightRow = next.row + dr[rightDir];
    int rightCol = next.col + dc[rightDir];

    // Read values safely with boundary checks
    int leftVal = -1, centerVal = -1, rightVal = -1;

    if (leftRow >= 0 && leftRow < ROWS && leftCol >= 0 && leftCol < COLS) {
      leftVal = grid[leftRow][leftCol];
    }

    if (next.row >= 0 && next.row < ROWS && next.col >= 0 && next.col < COLS) {
      centerVal = grid[next.row][next.col];
    }

    if (rightRow >= 0 && rightRow < ROWS && rightCol >= 0 && rightCol < COLS) {
      rightVal = grid[rightRow][rightCol];
    }

    // Print all info in the updated format
    Serial.print("(");
    Serial.print(current.row);
    Serial.print(", ");
    Serial.print(current.col);
    Serial.print(") > (");
    Serial.print(next.row);
    Serial.print(", ");
    Serial.print(next.col);
    Serial.print(")");

    Serial.print("| Grid: (");
    Serial.print(leftVal);
    Serial.print(", ");
    Serial.print(centerVal);
    Serial.print(", ");
    Serial.print(rightVal);
    Serial.print(")");

    Serial.print("| Facing: ");
    Serial.print(dirName);

    Serial.print("| Act: ");
    Serial.println(action);

    current = next;
    currentFacingDir = newDir;

    delay(200);
  }

  return true;
}

// Finds a walkable cell adjacent to the drop point
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

// Arduino setup
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Robot Path Planning ===");

  Point start = {9, 0};
  Point dropZone = {5, 5};

  Point boxes[] = {
    {3, 3},
    {3, 6},
    {5, 7},
    {9, 9}
  };

  Point current = start;
  Serial.print("Starting at (");
  Serial.print(current.row); Serial.print(", ");
  Serial.print(current.col); Serial.println(")");

  for (int i = 0; i < 4; i++) {
    Serial.print("\n>>> Moving to Box ");
    Serial.println(i + 1);
    Serial.flush();
    //delay(10);

    bool picked = moveAndPrint(current, boxes[i]);

    if (picked) {
      Serial.print("Box ");
      Serial.print(i + 1);
      Serial.println(" picked");

      Point dropTarget;
      if (findDropAdjacent(dropZone, dropTarget)) {
        Serial.print(">>> Moving to Drop Zone with Box ");
        Serial.println(i + 1);
        Serial.flush();
        //delay(10);

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
  // Nothing to do
}
