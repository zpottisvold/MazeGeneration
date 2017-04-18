/*
 * Wilson's algorithm
 * 1. Select a random node and add it to the maze path.
 * 2. Select another random node that is not part of the maze path
 *    and perform a random walk until you encounter a node that is part of the maze path.
 * 3. Add the nodes from the random walk to the maze path.
 * 4. Repeat 2 and 3 until all nodes are part of the maze path.
 *
 * (The random walk performed is a loop-erased random walk.)
 */

#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using std::vector;
using std::string;
using std::cout;
using std::endl;

#ifdef _WIN32
  #include <windows.h>
  #define CLEARSCREEN system("cls")
  #define SLEEP Sleep(100) // milliseconds
#else
  #include <unistd.h>
  #define CLEARSCREEN system("clear")
  #define SLEEP usleep(100 * 1000) // microseconds
#endif


struct step {
  int x, y, dir;

  step(int a, int b, int c)
    : x(a)
    , y(b)
    , dir(c) { }
};

vector<vector<int>> grid;
vector<step> currentPath;
vector<std::pair<int, int>> remainingCells;
int currentx, currenty;

int movX(int d) {
  if (d == 1) {
    // East
    return 1;
  }
  if (d == 2) {
    // West
    return -1;
  }
  return 0;
}

// Y-axis is reversed.
int movY(int d) {
  if (d == 3) {
    // South
    return 1;
  }
  if (d == 4) {
    // North
    return -1;
  }
  return 0;
}

bool inCurrentPath(int new_x, int new_y) {
  for (step s : currentPath) {
    if (s.x == new_x && s.y == new_y) {
      return true;
    }
  }
  return false;
}

void addStepToCurrentPath(int new_x, int new_y, int dir) {
  if (!currentPath.empty()) {
    step s = currentPath.back();
    if (s.x == new_x && s.y == new_y) {
      currentPath.pop_back();
    }
  }
  currentPath.push_back(step(new_x, new_y, dir));
}

void eraseWalk(int new_x, int new_y) {
  while (true) {
    step s = currentPath.back();
    if (s.x == new_x && s.y == new_y) {
      break;
    }
    currentPath.pop_back();
  }
}

bool isPointInGrid(int x, int y) {
  return x < grid[0].size() && x >= 0 && y >= 0 && y < grid.size();
}

void printGrid(bool printcurrentpath) {
  CLEARSCREEN;
  string line = "+";
  for (int i = 0; i < grid[0].size(); ++i){
    line += "-+";
  }

  cout << line << "\n";

  for (int i = 0; i < grid.size(); i++) {
    string inBetween = "+";
    cout << "|";
    for (int j = 0; j < grid[i].size(); j++) {
      bool skip = false;
      if (printcurrentpath) {
        if (i == currenty && j == currentx){
          cout << "O";
          skip = true;
        }
        else if (inCurrentPath(j, i)) {
          cout << ".";
          skip = true;
        }
      }

      if (!skip)
        if (grid[i][j] != 0) {
          cout << " ";
        }
        else {
          cout << "#";
        }

      if (j != grid[i].size() - 1 && (grid[i][j] == 1 || grid[i][j + 1] == 2))
        cout << " ";
      else
        cout << "|";


      if (i != grid.size() - 1 && (grid[i][j] == 3 || grid[i + 1][j] == 4))
        inBetween += " +";
      else
        inBetween += "-+";
    }

    cout << "\n";
    if (i != grid.size() - 1) {
      cout << inBetween;
      cout << "\n";
    }
  }

  cout << line << "\n";

  cout.flush();
  // SLEEP;
}

// The loop-erased random walk.
void walk() {
  int index = rand() % remainingCells.size();
  currentx = remainingCells[index].first;
  currenty = remainingCells[index].second;


  while (true) {
    int direction = rand() % 4 + 1;
    int newX = currentx + movX(direction);
    int newY = currenty + movY(direction);
    if (!isPointInGrid(newX, newY)) {
      continue;
    }
    if (grid[newY][newX] != 0) {
      addStepToCurrentPath(currentx, currenty, direction);
      currentx = newX;
      currenty = newY;
      printGrid(true);
      break;
    }

    if (inCurrentPath(newX, newY)) {
      eraseWalk(newX, newY);
      currentx = newX;
      currenty = newY;
    }
    else {
      addStepToCurrentPath(currentx, currenty, direction);
      currentx = newX;
      currenty = newY;
      printGrid(true);
    }
  }
}

void removeFromRemainingCells(int x, int y) {
  auto it = remainingCells.begin();
  for (; it != remainingCells.end(); ++it) {
    if ((*it).first == x && (*it).second == y) {
      break;
    }
  }
  remainingCells.erase(it);
}

int main() {
  srand(time(nullptr));

  int sizeX = 20;
  int sizeY = 20;

  grid.resize(sizeY);
  for (auto& row : grid) {
    row.resize(sizeX, 0);
  }
  for (int i = 0; i < sizeY; i++) {
    for (int j = 0; j < sizeX; j++) {
      remainingCells.push_back(std::pair<int, int>(j, i));
    }
  }

  int x = rand() % sizeX;
  int y = rand() % sizeY;

  grid[y][x] = -1; // The start node. -1 == part of maze but no direction.
  removeFromRemainingCells(x, y);

  while (!remainingCells.empty()) {
    walk();
    for (step s : currentPath) {
      grid[s.y][s.x] = s.dir;
      removeFromRemainingCells(s.x, s.y);
      printGrid(false);
    }

    currentPath.clear();
  }
  printGrid(false);
  cout << endl;

  return 0;
}
