This project was done using SDL in a Fedora VM (WSL). 

It makes a surface and places circles on that surface. The circles have a veclocity and collide with one another (denoted by turning red on collision).
In order to detect thousands of collisions effeciently I used a a quadtree which allows to only check against neighoring circles for collisions rather than brute force checking the entire scene.

You can actaully see the difference in optimization as well as have some fun with the simulation. Controlls listed below.

This does use some helper classes originally designed by my professor Dr. Yihsang Liow. The copy right is in the code but a disclaimer here as well. These are particularly in the "Includes" and "Src" folders.

----------------------------
CONTROLS
----------------------------

- SPACE - Switch collision detection modes (brute force or quadtree)
- UP_ARROW --- Zoom in (you will see the quadtree visuals go away but it is still running unless toggled with above)
- DOWN_ARROW --- Zoom out
- TAB --- Pause
- LEFT_CLICK --- Push force applied at mouse location (can click and hold)
- RIGHT_CLICK --- Pull force applied at mouse lcoation (can cick and hold)
- MINUS --- Slows down time

--------------------------------------
NOT FULLY IMPLEMENTED
--------------------------------------
EQUALS --- "Speeds up" time. (currently broken)
W --- Pan up
A --- Pan left
S --- Pan down
D --- Pan right

--------------------------------------
HOW TO RUN
--------------------------------------
For anyone unsure of how this is being compiled. This project was compiled with g++ into the ./a.out

Run "./a.out" and you should be able to run the project.
