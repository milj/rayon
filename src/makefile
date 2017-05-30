
rayon: main.o mgfview.o camera.o geometry.o dodatki.o saves.o scene.o raytrace.o \
	triangle_moller.o colours.o kdtree.o tone_mapping.o ciergb.o
	gcc -o rayon \
	main.o mgfview.o camera.o geometry.o dodatki.o saves.o scene.o raytrace.o \
	triangle_moller.o colours.o kdtree.o tone_mapping.o ciergb.o \
	../libmgf/libmgf.a \
	../radiance/color.o \
	../radiance/colrops.o \
	../radiance/resolu.o \
	../radiance/header.o \
	../radiance/bmalloc.o \
	../radiance/fputword.o \
	/usr/lib/libGL.so.1.0.4496 \
	/usr/lib/libglut.so.3.7

main.o: main.h main.c
	gcc -c -O3 main.c

mgfview.o: mgfview.h mgfview.c
	gcc -c -O3 mgfview.c

camera.o: camera.h camera.c
	gcc -c -O3 camera.c

geometry.o: geometry.h geometry.c
	gcc -c -O3 geometry.c

dodatki.o: dodatki.h dodatki.c
	gcc -c -O3 dodatki.c

saves.o: saves.h saves.c
	gcc -c -O3 saves.c

scene.o: scene.h scene.c
	gcc -c -O3 scene.c

raytrace.o: raytrace.h raytrace.c
	gcc -c -O3 raytrace.c

triangle_moller.o: triangle_moller.h triangle_moller.c
	gcc -c -O3 triangle_moller.c

colours.o: colours.h colours.c
	gcc -c -O3 colours.c

kdtree.o: kdtree.h kdtree.c
	gcc -c -O3 kdtree.c

tone_mapping.o: tone_mapping.h tone_mapping.c
	gcc -c -O3 tone_mapping.c

ciergb.o: ciergb.c
	gcc -c -O3 ciergb.c

clean:
	rm *.o
	rm *~
