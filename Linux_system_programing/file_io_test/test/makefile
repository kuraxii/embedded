src = $(wildcard *.c)  #hello.c sub.c div1.c add.c
obj = $(patsubst %.c,%.out,$(src))

ALL:$(obj)

$(obj):%.out:%.c
	gcc $< -o $@ -g




clean:
	-rm -rf $(obj) 
