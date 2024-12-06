#ifndef SIMULATION_H
#define SIMULATION_H

#include "Quadtree.h"
#include "Surface.h"
#include "Circle.h"
#include <vector>
#include <memory>

class Simulation
{
public:
    Simulation();
    ~Simulation();

    void run();
    void toggleCollisionMode();
    void cleanup();

private:
    void update(float timeScale); // Updated to include timeScale as a parameter
    void render();
    void addCircle();    // Declaration for addCircle
    void removeCircle(); // Declaration for removeCircle
    
    void pushCircles(int mouseX, int mouseY);
    void pullCircles(int mouseX, int mouseY);

    Quadtree quadtree;
    Surface surface;
    std::vector<std::unique_ptr<Circle>> circles;
    float zoomFactor;
    bool useQuadtree;
    
    float panX = 0.0f; // Horizontal pan offset
    float panY = 0.0f; // Vertical pan offset
    const float panSpeed = 10.0f; // Speed of panning
    float timeScale = 1.0f; // Time scaling factor for simulation speed
};

#endif