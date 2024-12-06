#include "Circle.h"
#include <cmath>

// Constructor
Circle::Circle(float x, float y, float vx, float vy, int radius)
    : x(x), y(y), vx(vx), vy(vy), radius(radius), color(GREEN), collided(false), prevX(x), prevY(y)
{
    this->x = (x < radius) ? radius : (x > WIDTH - radius) ? WIDTH - radius : x;
    this->y = (y < radius) ? radius : (y > HEIGHT - radius) ? HEIGHT - radius : y;
}

// Movement
void Circle::move(int width, int height, float timeScale)
{
    prevX = x; // Store the current position as the previous position
    prevY = y;

    x += vx * timeScale;
    y += vy * timeScale;

    if (x - radius < 0)
    {
        x = radius;
        vx = -vx;
    }
    else if (x + radius > width)
    {
        x = width - radius;
        vx = -vx;
    }

    if (y - radius < 0)
    {
        y = radius;
        vy = -vy;
    }
    else if (y + radius > height)
    {
        y = height - radius;
        vy = -vy;
    }
}

// Collision resolution
void Circle::resolveCollision(Circle &other, float timeScale)
{
    float dx = other.x - x;
    float dy = other.y - y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance == 0)
    {
        dx = 1.0f;
        dy = 0.0f;
        distance = 1.0f;
    }

    float nx = dx / distance; // Normalized collision axis
    float ny = dy / distance;

    float dotProduct = (vx * nx + vy * ny);               // Velocity along the collision axis
    float otherDotProduct = (other.vx * nx + other.vy * ny);

    // Adjust velocities based on collision
    vx -= dotProduct * nx * timeScale;
    vy -= dotProduct * ny * timeScale;
    other.vx -= otherDotProduct * nx * timeScale;
    other.vy -= otherDotProduct * ny * timeScale;

    vx += otherDotProduct * nx * timeScale;
    vy += otherDotProduct * ny * timeScale;
    other.vx += dotProduct * nx * timeScale;
    other.vy += dotProduct * ny * timeScale;

    // Apply decay factor to velocities
    const float DECAY_FACTOR = 0.92f; // Reduce velocity by 2% per collision
    vx *= DECAY_FACTOR;
    vy *= DECAY_FACTOR;
    other.vx *= DECAY_FACTOR;
    other.vy *= DECAY_FACTOR;

    // Resolve overlap to prevent sticking
    float overlap = (radius + other.radius - distance) / 2;
    x -= overlap * nx * timeScale;
    y -= overlap * ny * timeScale;
    other.x += overlap * nx * timeScale;
    other.y += overlap * ny * timeScale;
}

// Collision detection
bool Circle::collidesWith(const Circle &other) const
{
    float dx = x - other.x;
    float dy = y - other.y;
    float distanceSquared = dx * dx + dy * dy;
    float radiusSum = radius + other.radius;
    return distanceSquared < radiusSum * radiusSum;
}

// Set collision state and update color
void Circle::setCollided(bool state)
{
    collided = state;
    color = collided ? RED : GREEN;
}

void Circle::moveByForce(float fx, float fy)
{
    vx += fx;
    vy += fy;
}

// Get the current color
const Color &Circle::getColor() const
{
    return color;
}

// Draw the circle
void Circle::draw(Surface &surface)
{
    if (radius <= 0 || x - radius < 0 || x + radius >= WIDTH || y - radius < 0 || y + radius >= HEIGHT)
        return; // Skip invalid or out-of-bounds circles

    surface.put_circle(x, y, radius, color);
}

// Getters
float Circle::getX() const { return x; }
float Circle::getY() const { return y; }
float Circle::getPrevX() const { return prevX; }
float Circle::getPrevY() const { return prevY; }
float Circle::getRadius() const { return static_cast<float>(radius); }