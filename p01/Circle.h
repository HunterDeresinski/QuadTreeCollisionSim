#ifndef CIRCLE_H
#define CIRCLE_H

#include "Includes.h"

class Circle
{
private:
    float x, y;         // Circle's current position
    float prevX, prevY; // Circle's previous position
    float vx, vy;       // Circle's velocity
    int radius;         // Circle's radius
    Color color;        // Circle's current color
    bool collided;      // Tracks if the circle is currently in a collision

public:
    Circle(float x, float y, float vx, float vy, int radius);

    void move(int width, int height, float timeScale = 1.0f); // Updated to include timeScale

    bool collidesWith(const Circle &other) const; // Added const to match implementation
    void resolveCollision(Circle &other, float timeScale = 1.0f); // Updated to include timeScale
    void moveByForce(float fx, float fy);

    void setCollided(bool state);    // Set collision state and update color
    const Color &getColor() const;  // Get the current color

    void draw(Surface &surface);

    float getX() const;
    float getY() const;
    float getPrevX() const; // Get the previous X position
    float getPrevY() const; // Get the previous Y position
    float getRadius() const;
};

#endif