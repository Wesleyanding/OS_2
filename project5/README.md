# Project 5

## Building

Command line

`make` to build. Creates an executable called `reservations`.

## Files

`block.c` : The code for reading and writing blocks
`block.h` : Shows example usage for block.c
`ctest.h` : Shows examples for test creation
`image.c` : The code for opening and closing the file system image
`image.h` : Shows example usage for the image.c
`libvvsfs.a : library of object files
`testfs.c` : The main code to launch the project and run the tests.

## Data
`result` - an integer for the result of functions

## Functions
* `CTEST_VERBOSE()` : Sets the verbosity of the testing framework
* `image_open()` : Function to open the image
* `CTEST_ASSERT` : Function to assert that the result functioned properly
* `CTEST_RESULTS()` : Function to print the results of the testing framework
* `CTEST_EXIT()` : Function to exit the testing framework. 

## Notes
ctest.h was provided. Used in class notes to build functions for image and block. 

