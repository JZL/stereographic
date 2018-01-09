# What is this?

This is a C program which generates the projection of a 2D image onto an icosahedron. It was inspired by Henry Segerman's amazing work with [(inverse) stereographic projection](https://www.youtube.com/watch?v=lbUOScpu0ws) which uses the projection of a 2D image onto/from a sphere. However, Professor Segerman uses 3D printing to generate physical versions of these inverse projections which I do not have access to (and don't want to pay for shapeways/etc).

So, I use an icosahedron as the 3D shape which can be made using papercraft and common printers. It can be made with plain paper and an XActo knife but, for fine details, this becomes extremely time intensive, innaccurate, and fragile. Therefore, transparecies are perfect and have the added benefit of allowing full color projection. Some of the most interesting results I have found use Google Earth satellite imagery as the 'base image' to form a miniature projectable 'globe' 

# How to use/How does it work?

1. `makeImg.sh BASE_IMAGE`
    - This generates the base image of the correct (square) size using ImageMagick
        - Modify the `size` parameter to change the quality of the output image (warning, exponential increase in time)

2. `./stereo`
    - The main program which converts the input image into the individual faces of the icosahedron
    - Algorithm used:
      1. Using `libpng,` the input image is read in
      2. For each face of the icosahedron and for each pixel on the input image:
          - *Didier Badouel's Ray Polygon Intersection Algorithm* is used to find the intersection (if any) between the pixel's location and the virtual light's location (center top of the icosahedron)
          - If there is an intersection in 3-space, need to project onto the 2-space of face's plan
              - Uses linear algebra change-of-basis (which requires a custom-written left inverse and projection function)
                -  **Note:** for fun/learning, no external linear algebra libraries are used, all functions were custom written
          - Each face is run in parallel with `pthreads` for an added (embarassingly parallel) speed boost
    - This generates a separate png file for each face of the iscosahedron with the projected image in `out/` directory

3. `makeIcosahedronNet.sh`
  - Using ImageMagick, this combines the faces into a single net i.e. (for an undisclosed location)
     ![Example Output Net](https://github.com/JZL/stereographic/blob/master/net.jpg)

# Future Work/Extensions

 - Right now, this can only generate icosahedrons because they are convienent and relatively strong. However, this system was designed to be used with any convex solid (even irregular ones). In order to make a different solid, the coordnates of each face must be placed into `coords.c`. For example, `icosahedronCoord.py` generates the icosahedron coordinates. Note that the program projects the image from a virtual (0,0) plane so if the canonical form of the vertices for the solid do not have a flat bottom plane, the whole solid must be rotated. Otherwise, the resulting 3D form will be unstable. (Example rotation code to get a chosen plane to be flat is in `icosahedronCoord.py` along with `pyplot` visualizations). The assembly of the net (i.e. `makeIcosahedronNet.sh` will also need to be modified)
