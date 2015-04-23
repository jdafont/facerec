# facerec
Facial recognition for Linear Algebra (SP15) honors project.  Based on Turk and Pentland paper

Using Eigen lib to detect the faces of local students at UL using eigen vectors and eigenvalues

Eigen is used as it does not require the install of LAPACK, BLAS, or ATLAS.  This means that it will be more easily installed.  

An open face database will be used, provided by Yale at vision.ucsd.edu/datasetsAll

Image loading will be done using SDL2, to install this please use:

sudo apt-get install libsdl2-dev libsdl2-image-dev

This software is currently limited to 8-bit per pixel GIFs
