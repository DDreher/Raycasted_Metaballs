# GPU-based Raymarched Metaballs

C++/OpenGL implementation based on [GPU‐based Fast Ray Casting for a Large Number of Metaballs](https://www.researchgate.net/publication/220507233_GPU-based_Fast_Ray_Casting_for_a_Large_Number_of_Metaballs) by Kanamori et al. (2008)

<img src="https://github.com/DDreher/RaycastedMetaballs/blob/master/images/results.gif" width="300" />

## Overview

"In computer graphics, metaballs are organic-looking n-dimensional isosurfaces, characterised by their ability to meld together when in close proximity to create single, contiguous objects. This "blobby" appearance makes them versatile tools, often used to model organic objects and also to create base meshes for sculpting." ([Wiki](https://en.wikipedia.org/wiki/Metaballs))

For the purpose of rendering such metaballs a simple particle system was implemented.
Particles are simulated on CPU.
For each particle a billboard (i.e. a plane that always faces the camera) is generated in the geometry shader.
In the fragment shader, the implicit surfaces of the meta balls are calculated for each fragment of the billboards.

![image_of_procedure](https://github.com/DDreher/RaycastedMetaballs/blob/master/images/procedure.PNG)

A ray is marched from the camera in direction of the fragment.
The intersection of the ray and the implicit surface is calculated iteratively.
If no intersection is found, the fragment is discarded.
For the sake of efficiency, the intersection with the implicit surface is first calculated roughly and with big steps.
In case an intersection is found, a more accurate position is calculated with binary search.
For lighting, the normals of the isosurface are calculated by evaluating the density field gradients.

## How to build

Building has only been tested on Win x64.

1. Make sure to have OpenGL installed
2. Run `GenerateProjectFiles.bat`
3. Open the generated `RaycastedMetaBalls.sln`
4. Build and run in the desired configuration (debug / release)

## Dependencies

[OpenGL 3.3](https://www.opengl.org/)
[GLEW](http://glew.sourceforge.net/)     
[GLFW](https://www.glfw.org/)    
[GLM](https://glm.g-truc.net/0.9.9/index.html)

## Controls

If you're familiar with shooter controls, you're also familiar with the controls of this project :)

w - move forward    
a - strafe left    
s - move backward    
d - strafe right    
space - move upward    
ctrl - move downward    

1 - Enable point shader    
2 - Enable billboard shader    
3 - Enable meta ball shader    

y - freeze particle simulation    
x - enable slow motion / continue particle simulation if frozen    
c - disable slow motion / continue particle simulation if frozen    
escape - close application    
