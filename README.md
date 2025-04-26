# CS416 Programming Assignments

This directory contains the starter code for several of the CS416 assignments.

## Getting Started

Refer to Canvas for the pre-requisites for building these assignments. At a minimum you will need CMake and a C++ compiler that supports C++17. CMake will test for other prerequisites.

## Building

Once you have the dependencies installed, you will need to create the build files with CMake. For simplicity we will use an "in-source" build. 

On the *ada* cluster run the following command from within the top-level directory.

```
cmake .
```

You should only need to do this once per clone of the skeleton (or if you pull updates the to the skeleton). You should not need to modify any of the build files themselves.

Once you have run the above command to create the build files, you can build the individual assignments as described in README files in the assignment directories.