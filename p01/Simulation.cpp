#include "Simulation.h"
#include "Includes.h"
#include <cstdlib> // For rand()
#include <ctime>   // For seeding random

Simulation::Simulation() : quadtree(0, Rect(0, 0, WIDTH, HEIGHT)), zoomFactor(1.0f), panX(0.0f), panY(0.0f), timeScale(1.0f)
{
    surface = Surface(WIDTH, HEIGHT);
    useQuadtree = true;
    int numOfCircles = 5000;

    if (!surface.get())
    {
        std::cerr << "Error: Failed to initialize surface.\n";
        exit(1);
    }

    std::srand(std::time(nullptr)); // Seed random generator

    for (int i = 0; i < numOfCircles; i++)
    {
        float x = rand() % WIDTH;
        float y = rand() % HEIGHT;
        float vx = (rand() % 10 - 5) / 2.0f;
        float vy = (rand() % 10 - 5) / 2.0f;
        int radius = 2;

        circles.emplace_back(std::make_unique<Circle>(x, y, vx, vy, radius));
    }
}

Simulation::~Simulation()
{
    cleanup();
}

void Simulation::addCircle()
{
    float centerX = WIDTH / 2.0f;
    float centerY = HEIGHT / 2.0f;
    float vx = (rand() % 20 - 5) / 2.0f;
    float vy = (rand() % 20 - 5) / 2.0f;
    int radius = 2;

    float maxOffset = 100.0f;

    // Generate random offsets within the range [-maxOffset, +maxOffset]
    float xOffset = static_cast<float>(rand() % static_cast<int>(maxOffset * 2)) - maxOffset;
    float yOffset = static_cast<float>(rand() % static_cast<int>(maxOffset * 2)) - maxOffset;

    // Calculate the new circle's position
    float x = centerX + xOffset;
    float y = centerY + yOffset;

    circles.emplace_back(std::make_unique<Circle>(x, y, vx, vy, radius));
    // std::cout << "Added a circle at center with velocity (" << vx << ", " << vy << "). Total circles: " << circles.size() << "\n";
}

void Simulation::removeCircle()
{
    if (!circles.empty())
    {
        int index = rand() % circles.size();
        circles.erase(circles.begin() + index);
        // std::cout << "Removed a circle. Total circles: " << circles.size() << "\n";
    }
    else
    {
        std::cout << "No circles to remove.\n";
    }
}

void Simulation::toggleCollisionMode()
{
    useQuadtree = !useQuadtree;
    if (useQuadtree)
    {
        std::cout << "Switched to Quadtree-based collision detection.\n";
    }
    else
    {
        std::cout << "Switched to brute-force collision detection.\n";
    }
}

void Simulation::update(float timeScale)
{
    static bool quadtreeDirty = true; // Track if the quadtree needs rebuilding

    if (useQuadtree)
    {
        if (quadtreeDirty)
        {
            // Clear and rebuild the quadtree
            quadtree.clear();

            // Insert all circles into the quadtree based on their current positions
            for (auto &circle : circles)
            {
                circle->setCollided(false); // Reset collision state
                quadtree.insert(circle.get());
            }
            quadtreeDirty = false; // Quadtree is now up to date
        }

        // Retrieve and check for collisions using the quadtree
        for (auto &circle : circles)
        {
            std::vector<Circle *> nearbyCircles;
            quadtree.retrieve(nearbyCircles, circle.get());

            for (Circle *nearbyCircle : nearbyCircles)
            {
                if (circle.get() != nearbyCircle && circle->collidesWith(*nearbyCircle))
                {
                    circle->resolveCollision(*nearbyCircle, timeScale); // Adjust collision resolution by timeScale
                    circle->setCollided(true);       // Mark the circle as collided
                    nearbyCircle->setCollided(true); // Mark the nearby circle as collided
                }
            }
        }
    }
    else
    {
        // Reset all circles' collision state
        for (auto &circle : circles)
        {
            circle->setCollided(false);
        }

        // Check for collisions without using the quadtree
        for (size_t i = 0; i < circles.size(); ++i)
        {
            for (size_t j = i + 1; j < circles.size(); ++j)
            {
                if (circles[i]->collidesWith(*circles[j]))
                {
                    circles[i]->resolveCollision(*circles[j], timeScale); // Adjust collision resolution by timeScale
                    circles[i]->setCollided(true); // Mark circle i as collided
                    circles[j]->setCollided(true); // Mark circle j as collided
                }
            }
        }
    }

    // Update positions for all circles with time scaling
    quadtreeDirty = false; // Assume quadtree doesn't need rebuilding
    for (auto &circle : circles)
    {
        float prevX = circle->getPrevX();
        float prevY = circle->getPrevY();
        circle->move(WIDTH, HEIGHT, timeScale); // Scale movement by timeScale

        // Check if the circle moved significantly
        if (std::abs(circle->getX() - prevX) > circle->getRadius() / 2.0f ||
            std::abs(circle->getY() - prevY) > circle->getRadius() / 2.0f)
        {
            quadtreeDirty = true; // Mark the quadtree as needing rebuilding
        }
    }
}

#include <sstream> // For std::ostringstream

void Simulation::render()
{
    surface.fill(BLACK);

    // Render all the circles with panning and scaling
    for (const auto &circle : circles)
    {
        int scaledX = static_cast<int>((circle->getX() - panX) * zoomFactor);
        int scaledY = static_cast<int>((circle->getY() - panY) * zoomFactor);
        int scaledRadius = static_cast<int>(circle->getRadius() * zoomFactor);

        // Skip invalid or out-of-bounds circles
        if (scaledX - scaledRadius < 0 || scaledX + scaledRadius >= WIDTH ||
            scaledY - scaledRadius < 0 || scaledY + scaledRadius >= HEIGHT ||
            scaledRadius <= 0)
        {
            continue;
        }

        surface.put_circle(scaledX, scaledY, scaledRadius, circle->getColor());
    }

    // Draw quadtree if enabled
    if (useQuadtree)
    {
        quadtree.draw(surface, zoomFactor, panX, panY); // Pass pan offsets to quadtree
    }

    // --- Display total number of balls ---
    std::string ballCountText = "Total Balls: " + std::to_string(circles.size());
    Font font("fonts/FreeMonoBold.ttf", 40); // Load font with size 40
    Color blue = {0, 255, 255};              // Blue color
    Image ballCountImage(font.render(ballCountText.c_str(), blue)); // Render text to an image

    // Position the ball count text
    Rect ballCountRect = ballCountImage.getRect();
    ballCountRect.x = WIDTH - ballCountRect.w - 10; // Right-align with 10px padding
    ballCountRect.y = 10;                          // 10px padding from the top

    surface.lock();
    surface.put_image(ballCountImage, ballCountRect);

    // --- Display time scale below ball count ---
    std::ostringstream timeScaleStream;
    timeScaleStream.precision(1);
    timeScaleStream << std::fixed << timeScale; // Format timeScale to 1 decimal place
    std::string timeScaleText = "Time Scale: " + timeScaleStream.str() + "x";
    Image timeScaleImage(font.render(timeScaleText.c_str(), blue)); // Render time scale text to an image

    // Position the time scale text
    Rect timeScaleRect = timeScaleImage.getRect();
    timeScaleRect.x = WIDTH - timeScaleRect.w - 10; // Align with ball count text
    timeScaleRect.y = ballCountRect.y + ballCountRect.h + 5; // Below ball count with 5px spacing

    surface.put_image(timeScaleImage, timeScaleRect);
    surface.unlock();

    surface.flip();
}

void Simulation::run()
{
    Event event;
    bool running = true;
    bool paused = false;
    bool pushing = false;
    bool pulling = false;
    int mouseX = 0, mouseY = 0;

    while (running)
    {
        while (event.poll())
        {
            if (event.type() == QUIT)
            {
                running = false;
            }
            else if (event.type() == KEYDOWN)
            {
                if (event.keypressed(UPARROW)) // Zoom in
                {
                    float oldZoom = zoomFactor;
                    zoomFactor = std::min(zoomFactor + 0.1f, 3.0f);

                    // Adjust pan to center zoom on screen
                    float zoomRatio = zoomFactor / oldZoom;
                    panX += (WIDTH / 2.0f) * (1 - zoomRatio);
                    panY += (HEIGHT / 2.0f) * (1 - zoomRatio);

                    // Clamp to valid bounds
                    float maxPanX = WIDTH * (zoomFactor - 1.0f);
                    float maxPanY = HEIGHT * (zoomFactor - 1.0f);
                    panX = std::max(0.0f, std::min(panX, maxPanX));
                    panY = std::max(0.0f, std::min(panY, maxPanY));
                }
                else if (event.keypressed(DOWNARROW)) // Zoom out
                {
                    float oldZoom = zoomFactor;
                    zoomFactor = std::max(zoomFactor - 0.1f, 0.5f);

                    // Adjust pan to center zoom on screen
                    float zoomRatio = zoomFactor / oldZoom;
                    panX += (WIDTH / 2.0f) * (1 - zoomRatio);
                    panY += (HEIGHT / 2.0f) * (1 - zoomRatio);

                    // Clamp to valid bounds
                    float maxPanX = WIDTH * (zoomFactor - 1.0f);
                    float maxPanY = HEIGHT * (zoomFactor - 1.0f);
                    panX = std::max(0.0f, std::min(panX, maxPanX));
                    panY = std::max(0.0f, std::min(panY, maxPanY));
                }
                else if (event.keypressed(SDLK_PLUS) || event.keypressed(SDLK_EQUALS)) // Speed up time
                {
                    timeScale = std::min(timeScale + 0.1f, 3.0f); // Limit to max 3.0x speed
                    //std::cout << "Time scale increased to: " << timeScale << "x\n";
                }
                else if (event.keypressed(SDLK_MINUS)) // Slow down time
                {
                    timeScale = std::max(timeScale - 0.1f, 0.1f); // Limit to min 0.1x speed
                    //std::cout << "Time scale decreased to: " << timeScale << "x\n";
                }
                else if (event.keypressed(RIGHTARROW))
                {
                    for (int i = 0; i < 100; ++i)
                        addCircle();
                }
                else if (event.keypressed(LEFTARROW))
                {
                    for (int i = 0; i < 100; ++i)
                        removeCircle();
                }
                else if (event.keypressed(TAB))
                {
                    paused = !paused;
                }
                else if (event.keypressed(SPACE))
                {
                    toggleCollisionMode();
                }
                else if (event.keypressed(SDLK_ESCAPE))
                {
                    running = false;
                }
            }
            else if (event.type() == MOUSEBUTTONDOWN)
            {
                if (event.button()->button == SDL_BUTTON_LEFT)
                    pushing = true;
                else if (event.button()->button == SDL_BUTTON_RIGHT)
                    pulling = true;
            }
            else if (event.type() == MOUSEBUTTONUP)
            {
                if (event.button()->button == SDL_BUTTON_LEFT)
                    pushing = false;
                else if (event.button()->button == SDL_BUTTON_RIGHT)
                    pulling = false;
            }
            else if (event.type() == MOUSEMOTION)
            {
                mouseX = event.motion()->x;
                mouseY = event.motion()->y;
            }
        }

        // Check for held keys and apply movement
        Uint8 *keys = SDL_GetKeyState(NULL);
        if (keys[SDLK_w])
            panY = std::max(panY - panSpeed, 0.0f);
        if (keys[SDLK_s])
            panY = std::min(panY + panSpeed, HEIGHT / zoomFactor - HEIGHT);
        if (keys[SDLK_a])
            panX = std::max(panX - panSpeed, 0.0f);
        if (keys[SDLK_d])
            panX = std::min(panX + panSpeed, WIDTH / zoomFactor - WIDTH);

        if (!paused)
        {
            if (pushing)
                pushCircles(mouseX, mouseY);
            if (pulling)
                pullCircles(mouseX, mouseY);

            update(timeScale);
        }

        render();
        SDL_Delay(16); // Approx. 60 FPS
    }
}

void Simulation::pushCircles(int mouseX, int mouseY)
{
    const float pushRadius = 100.0f; // Area of influence
    const float pushForce = 10.0f;

    float worldMouseX = mouseX / zoomFactor + panX;
    float worldMouseY = mouseY / zoomFactor + panY;

    for (auto &circle : circles)
    {
        float dx = circle->getX() - worldMouseX;
        float dy = circle->getY() - worldMouseY;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < pushRadius && distance > 0)
        {
            float fx = (dx / distance) * pushForce;
            float fy = (dy / distance) * pushForce;
            circle->moveByForce(fx, fy);
        }
    }
}

void Simulation::pullCircles(int mouseX, int mouseY)
{
    const float pullRadius = 150.0f; // Area of influence
    const float pullForce = 10.0f;

    // Adjust mouse coordinates to world space
    float worldMouseX = mouseX / zoomFactor + panX;
    float worldMouseY = mouseY / zoomFactor + panY;

    for (auto &circle : circles)
    {
        float dx = worldMouseX - circle->getX();
        float dy = worldMouseY - circle->getY();
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance < pullRadius && distance > 0)
        {
            float fx = (dx / distance) * pullForce;
            float fy = (dy / distance) * pullForce;
            circle->moveByForce(fx, fy);
        }
    }
}

void Simulation::cleanup()
{
    surface.free();
    quadtree.clear();
    circles.clear();
    SDL_Quit();
}
