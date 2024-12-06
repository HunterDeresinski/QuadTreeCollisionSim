#include "Quadtree.h"
#include <iostream>
#include <memory>
#include <array>
#include <algorithm>

Quadtree::Quadtree(int level, Rect bounds) : level(level), bounds(bounds) {}

Quadtree::~Quadtree()
{
    clear();
}

void Quadtree::clear()
{
    for (auto &node : nodes)
    {
        if (node)
        {
            node->clear();
            node.reset(); // Ensure the unique_ptr is reset
        }
    }
    nodes.clear();
    objects.clear();
}

void Quadtree::split()
{
    if (!nodes.empty())
        return; // Already split

    int subWidth = bounds.w / 2;
    int subHeight = bounds.h / 2;

    if (subWidth <= 0 || subHeight <= 0)
        return;

    int x = bounds.x;
    int y = bounds.y;

    Rect quadrantRects[4] = {
        {x, y, subWidth, subHeight},                       // Top-left
        {x + subWidth, y, subWidth, subHeight},            // Top-right
        {x, y + subHeight, subWidth, subHeight},           // Bottom-left
        {x + subWidth, y + subHeight, subWidth, subHeight} // Bottom-right
    };

    nodes.resize(4);
    for (int i = 0; i < 4; ++i)
    {
        nodes[i] = std::make_unique<Quadtree>(level + 1, quadrantRects[i]);
    }

    for (auto *object : objects)
    {
        int index = getIndex(object);
        if (index != -1)
        {
            nodes[index]->insert(object);
        }
    }

    // Remove objects moved to children
    objects.erase(std::remove_if(objects.begin(), objects.end(),
                                 [this](Circle *obj)
                                 {
                                     for (const auto &node : nodes)
                                     {
                                         if (node && node->getIndex(obj) != -1)
                                             return true;
                                     }
                                     return false;
                                 }),
                  objects.end());
}

void Quadtree::merge()
{
    if (nodes.empty())
        return;

    const int mergeThreshold = 8; // Same threshold for merging
    int totalObjects = objects.size();

    // Check if children can be merged
    bool canMerge = true;
    for (const auto &node : nodes)
    {
        if (node)
        {
            totalObjects += node->objects.size();
            if (!node->nodes.empty())
            {
                canMerge = false; // Prevent merging if any child is further split
            }
        }
    }

    if (canMerge && totalObjects <= mergeThreshold)
    {
        for (auto &node : nodes)
        {
            if (node)
            {
                objects.insert(objects.end(), node->objects.begin(), node->objects.end());
                node.reset(); // Delete child node
            }
        }
        nodes.clear();
    }
}

int Quadtree::getIndex(Circle *circle)
{
    // Dynamically calculate midpoints for each node
    double verticalMidpoint = bounds.x + bounds.w / 2.0;
    double horizontalMidpoint = bounds.y + bounds.h / 2.0;

    bool fitsInTop = circle->getY() + circle->getRadius() < horizontalMidpoint;
    bool fitsInBottom = circle->getY() - circle->getRadius() >= horizontalMidpoint;

    bool fitsInLeft = circle->getX() + circle->getRadius() < verticalMidpoint;
    bool fitsInRight = circle->getX() - circle->getRadius() >= verticalMidpoint;

    // If the object spans multiple quadrants, return -1
    if ((fitsInTop && fitsInBottom) || (fitsInLeft && fitsInRight))
        return -1;

    if (fitsInTop)
    {
        if (fitsInLeft)
            return 0; // Top-left
        if (fitsInRight)
            return 1; // Top-right
    }
    else if (fitsInBottom)
    {
        if (fitsInLeft)
            return 2; // Bottom-left
        if (fitsInRight)
            return 3; // Bottom-right
    }

    return -1; // Overlapping multiple quadrants or undefined case
}

void Quadtree::insert(Circle *circle)
{
    if (!nodes.empty())
    {
        int index = getIndex(circle);
        if (index != -1 && index < static_cast<int>(nodes.size()) && nodes[index])
        {
            nodes[index]->insert(circle);
            return;
        }
    }

    objects.push_back(circle);

    const int splitThreshold = 4; // Split if objects exceed this threshold
    const int maxLevel = 100;      // Limit quadtree depth to prevent excessive splitting

    if (level >= maxLevel)
    {
        return;
    }

    if (objects.size() > splitThreshold && nodes.empty())
    {
        split();
    }
    else if (!nodes.empty())
    {
        merge(); // Attempt to merge if objects fall below the merge threshold
    }
}

void Quadtree::retrieve(std::vector<Circle *> &returnObjects, Circle *circle)
{
    // Add all objects in the current node
    returnObjects.insert(returnObjects.end(), objects.begin(), objects.end());

    // Ensure the nodes vector is not empty
    if (!nodes.empty())
    {
        int index = getIndex(circle);

        if (index != -1)
        {
            // Retrieve from the specific child node if it exists
            if (index < static_cast<int>(nodes.size()) && nodes[index])
            {
                nodes[index]->retrieve(returnObjects, circle);
            }
        }
        else
        {
            // If the object overlaps multiple quadrants, retrieve from all children
            for (const auto &node : nodes)
            {
                if (node)
                { // Check if the node is not null
                    node->retrieve(returnObjects, circle);
                }
            }
        }
    }
}

void Quadtree::draw(Surface &surface, float zoomFactor, float panX, float panY)
{
    int x1 = static_cast<int>((bounds.x - panX) * zoomFactor);
    int y1 = static_cast<int>((bounds.y - panY) * zoomFactor);
    int x2 = static_cast<int>(((bounds.x + bounds.w) - panX) * zoomFactor);
    int y2 = static_cast<int>(((bounds.y + bounds.h) - panY) * zoomFactor);

    // Skip drawing if the bounds are outside the visible area or invalid
    if (x1 < 0 || y1 < 0 || x2 > WIDTH || y2 > HEIGHT || x1 >= x2 || y1 >= y2)
        return;

    // Draw the boundary of the quadtree node
    surface.put_line(x1, y1, x2, y1, WHITE); // Top
    surface.put_line(x1, y1, x1, y2, WHITE); // Left
    surface.put_line(x1, y2, x2, y2, WHITE); // Bottom
    surface.put_line(x2, y1, x2, y2, WHITE); // Right

    // Recursively draw child nodes
    for (const auto &node : nodes)
    {
        if (node)
        {
            node->draw(surface, zoomFactor, panX, panY);
        }
    }
}