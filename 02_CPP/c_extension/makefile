src = $(wildcard *.cpp)  #hello.c sub.c div1.c add.c
obj = $(patsubst %.cpp,%,$(src))

ALL:$(obj)

$(obj):%:%.cpp
	g++ $< -o $@


clean:
	-rm -rf $(obj) 

