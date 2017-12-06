all: main.exe

vector3.o: vector3.h vector3.cpp
	g++ -c vector3.cpp -O2

color.o: color.h color.cpp
	g++ -c color.cpp -O2

bmp.o: bmp.h bmp.cpp
	g++ -c bmp.cpp -O2

primitive.o: primitive.h primitive.cpp
	g++ -c primitive.cpp -O2

light.o: light.h light.cpp
	g++ -c light.cpp -O2

camera.o: camera.h camera.cpp
	g++ -c camera.cpp -O2

scene.o: scene.h scene.cpp
	g++ -c scene.cpp -O2

raytracer.o: raytracer.h raytracer.cpp
	g++ -c raytracer.cpp -O2

photonmap.o: photonmap.h photonmap.cpp
	g++ -c photonmap.cpp -O2

photontracer.o: photontracer.h photontracer.cpp
	g++ -c photontracer.cpp -O2

main.exe:   main.cpp vector3.o color.o bmp.o primitive.o light.o camera.o scene.o raytracer.o photonmap.o photontracer.o
	g++ main.cpp vector3.o color.o bmp.o primitive.o light.o camera.o scene.o raytracer.o photonmap.o photontracer.o -o main -O2

clear:
	rm *.bak *.o
