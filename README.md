# This exercise uses threads, mutexes, and condition variables

## ✏️ Request:
Write a program that creates 3 threads with the following functions:

- **Thread 1**: Enters student information from the keyboard, including name, date of birth, and hometown.  
- **Thread 2**: Each time data is entered, this thread writes the information to the `thongtinsinhvien.txt` file (each student's information is written on one line).  
- **Thread 3**: Reads the data from the `thongtinsinhvien.txt` file and prints it to the terminal. After that, it signals Thread 1 to continue inputting.

## 📌 Additional Requirements:
- A global variable `total` is used to calculate the total number of students.
- The program can be rerun: if `thongtinsinhvien.txt` already exists before the program starts, it will read the file and compute the current total number of students before creating the threads.

## 👉 How to run?
  - 'make all'
  - './main'
  - using 'gdb ./main' to debug
