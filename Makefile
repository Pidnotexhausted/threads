.PHONY : all clean

all:
	gcc -o main main.c -g -pthread

clean: 
	rm -rf main thongtinsinhvien.txt
