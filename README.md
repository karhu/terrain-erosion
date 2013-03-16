Terrain Erosion
===========

![ScreenShot](https://raw.github.com/karhu/terrain-erosion/master/screenshot.png)

A realtime terrain erosion & fluid simulation running on the CPU. The simulation is implemented in C++ on the CPU and parallelized using OpenMP. Rendering is done using OpenGL. We use the eulerian or grid-based approach to fluid simulation to implement the shallow water equations and hydraulic erosion on a height field terrain.

Based on the following papers:
- Fast Hydraulic Erosion Simulation and Visualization on GPU [Mei et al., 2007]
- Interactive terrain modeling using hydraulic erosion [St'ava et al., 2008]
- Fast Hydraulic and Thermal Erosion on the GPU [Jákó et al, 2011]

## Controls:

| Keys       | Action                       |
| -----------|------------------------------|
| W/S        | move camera forward/backward |
| A/D        | move camera left/right       |
| R/F        | move camera up/down          |
| Q/E        | rotate camera left/right     |
| T/G        | rotate camera up/down        |
| O/P        | start/stop rain              |
| K/L        | start/stop flood             |
| arrow keys | move flood position          |

## Dependencies:

- GLFW
- GLEW
- OpenGL 3.2
- C++11
- boost::filesystem
- boost::system
- OpenMP
- TCLAP (redistributed)

## Compile Instructions

**QtCreator:**  
"TerrainFluid.pro" is a QtCreator project file.

**Linux Console:**  
qmake-qt4 -makefile -o Makefile TerrainFluid.pro  
make  
./TerrainFluid  


## MIT Licence:

Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)  
Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)  
Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)  

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
