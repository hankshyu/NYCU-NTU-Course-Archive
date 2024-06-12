# Advanced Programming in the UNIX Environment

A graduate course on UNIX programming. More comments on the course could be seen [here](https://github.com/hankshyu/NYCU-Course/blob/main/Courses/semester%206/高等UNIX程式設計.md)

## Course Objective
- UNIX programming tools
- Library calls and system calls
- Implement (console-based) tools and applications
- Low-level programming in the UNIX environment

## Textbook
> W. Richard Stevens and Stephen A. Rago, “Advanced Programming in the UNIX Environment,” 2nd ed. or 3rd ed, Addison Wesley
#### Topics

Chap. | Topic |Slides 
:--------:|:----- |:---
1 |Fundamental tools and shell programming| [slide01][sl01]
2 |Files and directories| [slide04][sl04] 
3 |File I/O and standard I/O|[slide03+05][sl03+05]
4 |System data files and information|[slide04][sl04] [slide06][sl06]
5 |Process environment|[slide07][sl07]
6 |Process control|[slide08][sl08]
7 |Signals|[slide10][sl10]
8 |Assembly language integration|[slide10.5][sl10.5]
9 |ptrace and applications|[slide10.6][sl10.6]
10 |Threads|[slide11][sl11] [slide12][sl12]
11 |Inter-process communication|[slide15][sl15]
12 |Advanced IPC (Unix domain sockets)|[slide17][sl17]





## Labs 
Lab| Spec(PDF) |Brief description |Keywords
:---:|:-----:|:-----|:---
[Lab1][l1]|[⚙️][s1]|A 'lsof'-like program |[FILE I/O][sl04] [Files and directories][sl04] [System data files][sl06]
[Lab2][l2]|[⚙️][s2]|Logger program that can show file-access-related activities of an arbitrary binary|[Process environment][sl07] [Process control][sl08]
[Lab3][l3]|[⚙️][s3]|Extend a mini C library to support signal relevant system calls in x86 Assembly| [x86 Assembly][sl10] [Process control][sl08]
[Lab4][l4]|[⚙️][s4]|Scriptable Instruction Level Debugger|[Signals][sl10] [ptrace][sl12]

[sl01]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/01-ov%2Btools.pdf
[sl04]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/04-file%2Bdir.pdf
[sl03+05]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/03%2B05-file%2Bstdio.pdf

[sl04]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/04-file%2Bdir.pdf
[sl06]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/06-sysinfo.pdf

[sl07]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/07-procenv.pdf
[sl08]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/08-procctrl.pdf
[sl10]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/10-signals.pdf

[sl10.5]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/10.5-assembly.pdf
[sl10.6]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/10.6-ptrace.pdf

[sl11]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/11-threads.pdf

[sl12]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/12-threadctrl.pdf

[sl15]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/15-classipc.pdf

[sl17]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/Slides/17-advipc.pdf
  
[s1]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/HW1/unix_hw1.pdf
[s2]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/HW2/unix_hw2.pdf
[s3]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/HW3/unix_hw3.pdf
[s4]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/blob/main/HW4/unix_hw4.pdf


[l1]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/tree/main/HW1
[l2]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/tree/main/HW2
[l3]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/tree/main/HW3
[l4]:https://github.com/hankshyu/Advanced-Programming-in-the-UNIX-Environment/tree/main/HW4
