src = $(wildcard *.c)  #hello.c sub.c div1.c add.c
obj = $(patsubst %.c,%.out,$(src))

ALL:$(obj)

$(obj):%.out:%.c
	gcc $< -o $(HOME)/desktop/embedded/bin/$@


clean:
	-rm -rf $(obj) 
