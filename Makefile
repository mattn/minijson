all : main

main : main.cxx minijson.h
	g++ -I. -o main main.cxx
