#include "route_model.h"
#include <iostream>

RouteModel::RouteModel(const std::vector<std::byte> &xml) : Model(xml) {
    // Create RouteModel nodes.
    int counter = 0;
    for (Model::Node node : this->Nodes()) {
        m_Nodes.emplace_back(Node(counter, this, node));
        counter++;
    }
    CreateNodeToRoadHashmap();
}


void RouteModel::CreateNodeToRoadHashmap() {
    for (const Model::Road &road : Roads()) {
        if (road.type != Model::Road::Type::Footway) {
            for (int node_idx : Ways()[road.way].nodes) { //each road has the index of the way it belongs to. The road.way provides an index 
            // which we use in the vector of ways that contains nodes. These nodes are integer indices.
                if (node_to_road.find(node_idx) == node_to_road.end()) {
                    node_to_road[node_idx] = std::vector<const Model::Road *> ();
                }
                node_to_road[node_idx].push_back(&road);
            }
        }
    }
}


RouteModel::Node *RouteModel::Node::FindNeighbor(std::vector<int> node_indices) {
    Node *closest_node = nullptr;
    Node node;

    for (int node_index : node_indices) {
        node = parent_model->SNodes()[node_index]; // gets the node object from parent model SNodes. Parent model is a pointer to the route model
        if (this->distance(node) != 0 && !node.visited) {
            // below, we check if closest node has been initialized to null pointer. We check if the distance to the current node 
            // in the for loop is lesser than the distance to the closest node we found so far. 
            if (closest_node == nullptr || this->distance(node) < this->distance(*closest_node)) {
                closest_node = &parent_model->SNodes()[node_index];
            }
        }
    }
    return closest_node;
}
/*
Each road that the current node is a part of can be thought of as a possible direction to travel, much like the up, down,
left, right directions from the first A* search project. This means that you will want to find the closest neighbor from each 
road that the current node this belongs to. 
You can get each road that the current node belongs to using the node_to_road hash table as follows:
parent_model->node_to_road[this->index]
Once you have a road, you can get a vector containing all other node indices on that road with a similar construction to 
what you have seen before:
parent_model->Ways()[road->way].nodes
You can then use the FindNeighbor method with that vector of node indices to find a pointer to the closest node.
*/
void RouteModel::Node::FindNeighbors() {
    for (auto & road : parent_model->node_to_road[this->index]) {
        RouteModel::Node *new_neighbor = this->FindNeighbor(parent_model->Ways()[road->way].nodes); // returns a pointer to the closest node on the road
        if (new_neighbor) {
            this->neighbors.emplace_back(new_neighbor);
        }
    }
}


RouteModel::Node &RouteModel::FindClosestNode(float x, float y) {
    Node input; //Add the input given by the user to the node list
    input.x = x;
    input.y = y;

    float min_dist = std::numeric_limits<float>::max();
    float dist;
    int closest_idx;
    // Nodes may be on isolated  or inacessible or faraway paths. We will search for nodes that are on the road and are not footway
    for (const Model::Road &road : Roads()) {
        if (road.type != Model::Road::Type::Footway) {
            for (int node_idx : Ways()[road.way].nodes) { // get all of the node indices that are on the road
                dist = input.distance(SNodes()[node_idx]);
                if (dist < min_dist) {
                    closest_idx = node_idx;
                    min_dist = dist;
                }
            }
        }
    }

    return SNodes()[closest_idx];
}

