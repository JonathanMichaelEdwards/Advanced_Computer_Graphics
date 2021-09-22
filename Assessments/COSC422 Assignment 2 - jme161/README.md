COSC422 Assessment 2
----------

Computer Graphics (2021);  University of Canterbury

**DESCRIPTION:**
* In this assignment, you will develop methods for non-photorealistic rendering of three- dimensional mesh models. 
* The OpenMesh library will be used for loading mesh objects and for creating the definitions of triangle adjacency primitives (GL_TRIANGLES_ADJACENCY). 
* The triangle adjacency primitives are used for the computation of silhouette and crease edges.

&nbsp;

Non-Photorealistic Rendering (NPR):
---------
* Rendering methods used:
  - Two-tone shading -- uses only two shades of material colour.
  - Pencil shading -- uses a set of pencil stroke textures.
  
&nbsp;

**Basic NPR - 12 Marks**
1. Use the OpenMesh library.
2. Include a geometry shader - Highlighting edges.
   *  The silhouette and crease edges of the model must be highlighted in the output.
3. 3 different Pencil stroke textures - corresponding to three different shade levels.
4. Toggle between "Two-tone
 
&nbsp;

**Keys:**
* Arrow Keys - Moving around the Scene
  * Up
  * Down
  * Left
  * Right
* Other Keys
  *  '+' or '=' Move down
  *  '-' Move up
  *  '1' Height Map 1
  *  '2' Height Map 2
  *  SpaceBar - Toggles between wireframe and textured view
  *  'c' Toggle cracking
  *  'q' Inc. water level
  *  'a' Dec. water level
  *  'w' Inc. snow level
  *  's' Dec. snow level
  *  'f' Toggle fog
  *  'd' dec. fog density
  *  'e' Inc. fog density
  *  'l' Toggle lighting shade
  *  'b' Toggles sky color
  
&nbsp;

**Run Program:**
* On the Makefile Level - Type the command 'make && make program' in the command prompt

