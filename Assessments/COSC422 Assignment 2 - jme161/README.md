COSC422 Assessment 2
=============

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
2. Include a geometry shader - Computations.
3. The silhouette and crease edges of the model must be highlighted in the output.
3. Three different Pencil stroke textures - corresponding to three different shade levels.
4. Keystrokes described below


**Basic Keys:**
*  Toggle between two-tone rendering and pencil shading
   *  SpaceBar
* Arrow Keys - Rotate around model
  * Up
  * Down
  * Left
  * Right
* Zoom into model
  *  '+' or '=' or page up
* Zoom out of model
  *  '-' or page down 
*  Wireframe mode
   *  'f'
* Exit application
  * 'Esc'

&nbsp;

**Extra features - 5 Marks**
1. Vary the thicknesses of silhouette edges and crease edges.
2. Model selections
3. Mipmap - stroke density, 3 or 5 textures
4. Weighting between texture boundaries
5. Mesh Subdivision - Increase/Decrease mesh detail level

**Extra Keys:**

*  Thicknesses silhouette edges
   *  'q' - Increase
   *  'a' - Decrease
*  Thicknesses crease edges
   *  'w' - Increase
   *  's' - Decrease
*  Selecting models
   *  '1', '2' or '3'
*  Toggle between 3 to 5 textures
   *  'm'
*  Toggle texture weighting
   *  't'
* Mesh Subdivision
  * 'e' - Increase
  * 'd' - Decrease
  
&nbsp;

**Compiling & Running Program:**
* At the makefile level, Type - 'make && make program' in the terminal.

**Debugging in vscode:** 
* Press F5

