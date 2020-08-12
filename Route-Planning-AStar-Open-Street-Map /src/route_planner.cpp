#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.

    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node = &m_Model.FindClosestNode(end_x, end_y);
}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
  return node->distance(*end_node);
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
  // Call FindNeighbors() on current_node to populate 
  // the current_node's neighbors vector.
  current_node->FindNeighbors();
  // For each neighbor in the current_node's neighbors
  for (auto neighbor: current_node->neighbors) {
    // Set the neighbors parent to the current_node.
    neighbor->parent = current_node;
    // Set the neighbor's g_value to the sum of the current_node's 
    // g_value plus the distance from the curent_node to the neighbor.
    neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
    // Set the neighbor's h_value using CalculateHValue
    neighbor->h_value = CalculateHValue(neighbor);
    // Push the neighbor to the back of the open_list.
    open_list.push_back(neighbor);
    // Mark the neighbor as visited.
    neighbor->visited = true;
  }
}

// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.


RouteModel::Node* RoutePlanner::NextNode() {
  std::sort(open_list.begin(), open_list.end(), [](const auto &a, const auto &b) {
    return a->h_value + a->g_value < b->h_value + b->g_value;
  });
  RouteModel::Node* next_node = open_list.front(); //sort arranges the open list in ascending order so we get the node with the lowest value in the beginning
  open_list.erase(open_list.begin());// node with the lowest f value is at the beginning. So, we remove that.
  return next_node;
}

// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
  distance = 0.f;
  std::vector<RouteModel::Node> path_found;
  RouteModel::Node parent;

  while (current_node->parent != nullptr) {
    path_found.push_back(*current_node);
    distance += current_node->distance(*(current_node->parent));
    current_node = current_node->parent;
  }
  // Still need to add start node
  path_found.push_back(*current_node);
  // Scale
  distance *= m_Model.MetricScale();
  return path_found;
}

// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
  // Set start_node->visited to be true.
  start_node->visited = true;
  // Push start_node to the back of open_list.
  open_list.push_back(start_node);
  // Create a pointer RouteModel::Node *current_node 
  // and initialize the pointer to nullptr.
  RouteModel::Node *current_node = nullptr;

  // while the open_list size is greater than 0:
  while (open_list.size() > 0) {
    // Set the current_node pointer to the results of calling NextNode.
    current_node = NextNode();
    // if the distance from current_node to the end_node is 0:
    if (current_node->distance(*end_node) == 0) {
      // Call ConstructFinalPath using current_node and 
      // set m_Model.path with the results.
      m_Model.path = ConstructFinalPath(current_node);
      // Return to exit the A* search.
      return;
    }
    // else call AddNeighbors with the current_node.
    else {
      AddNeighbors(current_node);
    }
  }
}