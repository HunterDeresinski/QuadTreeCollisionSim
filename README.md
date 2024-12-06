**Project Overview:** 
---
This project demonstrates a physics-based simulation utilizing the Simple DirectMedia Layer (SDL) library, implemented within a Fedora virtual machine (VM) environment under Windows Subsystem for Linux (WSL). The primary objective of the project is to simulate dynamic object interactions on a graphical surface, emphasizing efficiency in collision detection for large-scale scenarios.

**Key Features**
---
**Surface and Dynamic Objects:**
The simulation creates a graphical surface populated with multiple circles, each assigned an initial velocity vector. These circles move independently and interact dynamically within the environment.

**Collision Detection with Visual Feedback:**
When two circles collide, the collision is visually represented by the circles turning red. This provides a clear, intuitive indication of interactions within the simulation.

**Optimized Collision Handling via Quadtree: **
To enhance performance, the project employs a quadtree algorithm for spatial partitioning. This significantly reduces computational overhead by restricting collision checks to nearby objects rather than performing exhaustive, brute-force comparisons across all circles in the scene. This optimization is particularly effective for handling thousands of simultaneous objects, making the simulation scalable and efficient.

**Interactive and Visual Appeal:**
Users can observe the effects of optimization in real-time and engage with the simulation using the provided controls, enhancing both educational value and user enjoyment.

**Technical Contributions**
This project builds on foundational helper classes originally developed by Dr. Yihsang Liow. These classes, included in the project's "Includes" and "Src" folders, streamline the implementation of essential functionalities. Full attribution and copyright information are provided in the code files, and this document also acknowledges their usage as a disclaimer.

**Controls and User Interaction**
A set of interactive controls enables users to experiment with the simulation, offering insights into the behavior of optimized collision detection systems.

This project not only showcases advanced programming techniques but also serves as an engaging platform for exploring the practical application of algorithms in computer graphics and physics-based simulations.

----------------------------
**CONTROLS**
----------------------------

- SPACE - Switch collision detection modes (brute force or quadtree)
- UP_ARROW --- Zoom in (you will see the quadtree visuals go away but it is still running unless toggled with above)
- DOWN_ARROW --- Zoom out
- TAB --- Pause
- LEFT_CLICK --- Push force applied at mouse location (can click and hold)
- RIGHT_CLICK --- Pull force applied at mouse lcoation (can cick and hold)
- MINUS --- Slows down time

--------------------------------------
**NOT FULLY IMPLEMENTED**
--------------------------------------
- EQUALS --- "Speeds up" time. (currently broken)
- W --- Pan up
- A --- Pan left
- S --- Pan down
- D --- Pan right
--------------------------------------
HOW TO RUN
--------------------------------------
For anyone unsure of how this is being compiled. This project was compiled with g++ into the ./a.out

Run "./a.out" and you should be able to run the project.
