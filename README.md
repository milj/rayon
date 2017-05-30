# rayon
This my "photorealistic" raytracer implemented back in 2004. This was a student assignment done as part of Realistic Computer Graphics university course.

I have uploaded it as-is for the first commit. There are some Polish sentences in the comments, a few source files have Polish names and the readme.txt is in Polish :) I have never done any professional development in C and I'm aware the code must be far from perfect.

The code depends on libmgf and radiance sources + some additional files, which are not included here. Cleaning it up and making it compile again will be fun. I even tried and fighted for a few minutes, but then the `undefined reference to symbol 'exp@@GLIBC_2.2.5'` error knocked me over.

Example image rendered by this raytracer, the only one I have found:

![Bathroom](https://raw.githubusercontent.com/milj/rayon/master/pics/bathroom.png)

Apparently the raytracing time was very short, therefore so many pixels are still black.

