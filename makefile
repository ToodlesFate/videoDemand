.PHONY:demo
demo:demo.cc
	g++ -o $@ $^ -std=c++11 -L/usr/lib64/mysql/ -lmysqlclient -ljsoncpp -lpthread

.PHONY:clean
clean:
	rm -rf demo