all:
	g++ -o main main.cpp -L/usr/X11R6/lib -lm -lpthread -lX11
