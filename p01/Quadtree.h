#ifndef QUADTREE_H
#define QUADTREE_H

#include "Includes.h"
#include "Circle.h"
#include <vector>
#include <memory> // For smart pointers

class Quadtree
{
private:
    int level;
    Rect bounds;
    std::vector<Circle *> objects;
    std::vector<std::unique_ptr<Quadtree>> nodes; // Use smart pointers

    void split(); // Splits the node into four child nodes
    void merge(); // Merges child nodes back into the parent node
    int getIndex(Circle *circle); // Determines which child node the circle belongs to

public:
    Quadtree(int level, Rect bounds);
    ~Quadtree();

    void clear(); // Clears the current node and its children
    void insert(Circle *circle); // Inserts a circle into the quadtree
    void retrieve(std::vector<Circle *> &returnObjects, Circle *circle); // Retrieves all circles that might collide with the given circle
    void draw(Surface &surface, float zoomFactor, float panX = 0.0f, float panY = 0.0f); // Draw Quadtree strcuture
};

#endif // QUADTREE_H