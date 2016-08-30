defaultmake: risch.cpp rischtest.cpp
	g++ -o rischtest risch.cpp rischtest.cpp -std=c++17 -Wall

clean:
	rm -f *~ *.o *.stackdump
