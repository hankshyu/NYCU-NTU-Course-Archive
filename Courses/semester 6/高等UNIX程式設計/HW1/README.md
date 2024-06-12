# Implement a 'lsof'-like program

- 'lsof' is a tool to list open files, it can be used to list all the files opened by processes running in the system. 
- The output of your homework is required to follow the spec strictly. 


## Program Arguments

1. Your program should work without any arguments. 
2. In the meantime, your program has to handle the following arguments properly:

- ``c REGEX``: a regular expression (REGEX) filter for filtering command line. For example `` -c sh`` would match ``bash``, ``zsh``, and ``share``.

- ``-t TYPE``: a TYPE filter. Valid TYPE includes  ``REG``,  ``CHR``,  ``DIR``,  ``FIFO``,  ``SOCK``, and   ``unknown``. TYPEs other than the listed should be considered invalid. For invalid types, your program has to print out an error message ``Invalid TYPE option.`` in a single line and terminate your program.

- ``-f REGEX``: a regular expression (REGEX) filter for filtering filenames.

## Sample Output

```
$ ./hw1 | head -n 20
COMMAND                                 PID             USER            FD              TYPE              NODE        NAME
systemd                                 1               root            cwd             unknown                       /proc/1/cwd (Permission denied)
systemd                                 1               root            rtd             unknown                       /proc/1/root (Permission denied)
systemd                                 1               root            txt             unknown                       /proc/1/exe (Permission denied)
systemd                                 1               root            NOFD                                          /proc/1/fd (Permission denied)
kthreadd                                2               root            cwd             unknown                       /proc/2/cwd (Permission denied)
kthreadd                                2               root            rtd             unknown                       /proc/2/root (Permission denied)
kthreadd                                2               root            txt             unknown                       /proc/2/exe (Permission denied)
kthreadd                                2               root            NOFD                                          /proc/2/fd (Permission denied)
rcu_gp                                  3               root            cwd             unknown                       /proc/3/cwd (Permission denied)
rcu_gp                                  3               root            rtd             unknown                       /proc/3/root (Permission denied)
rcu_gp                                  3               root            txt             unknown                       /proc/3/exe (Permission denied)
rcu_gp                                  3               root            NOFD                                          /proc/3/fd (Permission denied)
rcu_par_gp                              4               root            cwd             unknown                       /proc/4/cwd (Permission denied)
rcu_par_gp                              4               root            rtd             unknown                       /proc/4/root (Permission denied)
rcu_par_gp                              4               root            txt             unknown                       /proc/4/exe (Permission denied)
rcu_par_gp                              4               root            NOFD                                          /proc/4/fd (Permission denied)
kworker/0:0H-events_highpri             6               root            cwd             unknown                       /proc/6/cwd (Permission denied)
kworker/0:0H-events_highpri             6               root            rtd             unknown                       /proc/6/root (Permission denied)
kworker/0:0H-events_highpri             6               root            txt             unknown                       /proc/6/exe (Permission denied)
...
```
## Details

Your program has to output the following fields (columns) for each file opened by a running process. Each line presents the information for a single file. The required fields include ``COMMAND``, ``PID``, ``USERM``, ``FD``, ``TYPE``, ``NODE``, and ``NAME``. The meaning of each field (column) is explained below:

- COMMAND:
    - The ***executable filename*** of a running process.
    - DO NOT show arguments.

- PID:
Process id of a running process.
Only need to handle opened files in process level (check ```/proc/[pid]```. No need to handle opened files at thread level (that would be in ```/proc/[pid]/task/[tid]```).

- USER:
The username who runs the process.
Please show ```username``` instead of UID.

- FD: The file descriptor. The value shown in the ```FD``` field can be one of the following cases.
    - ```cwd```: The current working directory, which can be read from ```/proc/[pid]/cwd```.
    - ```rtd```: root directory, which can be read from ```/proc/[pid]/root```.
    - ```txt```: program file of this process, can be read from ```/proc/[pid]/exe```.
    - ```mem```: memory mapping information, which can be read from ```/proc/[pid]/maps```.
        - If ```/proc/<pid>/maps``` is not accessible, you don't need to show any information about mapped files.
        - A memory-mapped file may have multiple segments or be mapped multiple times. You only need to output the first one for duplicated files, i.e., files having the same i-node or filename.
        - You don't need to handle memory segments that do not associate with a file. For example, [heap] or anonymously mapped memory segments. Those memory segments should have an i-node number of zero.
    - ```DEL```: indicate a memory-mapped file has been deleted. You should show this value if there is a (deleted) mark right after the filename in memory maps.
    - ```[0-9]+[rwu]```: file descriptor and opened mode.
        - The numbers show the file descriptor number of the opened file.
        - The mode "r" means the file is opened for reading.
        - The mode "w" means the file is opened for writing.
        - The mode "u" means the file is opened for reading and writing.
    - ```NOFD```: if ```/proc/[pid]/fd``` is not accessible. In this case, the values for ```TYPE``` and ```NODE``` fields can be left empty.

- TYPE: The type of the opened file. The value shown in TYPE can be one of the following cases.
    - ```DIR```: a directory. ```cwd``` and ```rtd``` are also classified as this type.
    - ```REG```: a regular file
    - ```CHR```: a character special file, for example
    ```crw-rw-rw- 1 root root 1, 3 Mar 17 17:31 /dev/null```
    - ```FIFO```: a pipe, for example
        - A link to a pipe, e.g.,
        ```lr-x------ 1 root root 64 Mar 17 19:55 5 -> 'pipe:[138394]'```
        - A file with p type (FIFO)
        ```prw------- 1 root root 0 Mar 17 19:54 /run/systemd/inhibit/11.ref```
    - ```SOCK```: a socket, for example
    ```lrwx------ 1 root root 64 Mar 17 19:55 1 -> 'socket:[136975]'```
    -```unknown```: Any other unlisted types. Alternatively, if a file has been deleted or is not accessible (e.g., permission denied), this column can show unknown.

- NODE:
    - The i-node number of the file
    - It can be blank or empty if and only if ```/proc/[pid]/fd``` is not accessible.

- NAME:
    - Show the opened filename if it is a typical file or directory.
    - Show ```pipe:[node number]``` if it is a symbolic file to a pipe, e.g.,
    ```l-wx------ 1 ta ta 64 三 8 02:11 91 -> 'pipe:[2669735]'```
    - Show ```socket:[node number]``` if it is a symbolic file to a socket, e.g.,
    ```lrwx------ 1 ta ta 64 三 8 02:11 51 -> 'socket:[2669792]'```
    - Append  ```(Permission denied)``` if the access to ```/proc/pid/fd``` or ```/proc/pid/(cwd|root|exe)``` is failed due to permission denied.
    - If the filename you read from /proc file system contains a  ```(deleted)```, please remove it from the filename before you print it out.

## Additional Notes on REGEX
If you plan to test REGEX feature with the ```lsof``` package that comes with Linux distributions, you should run it with the option ```-c /REGEX/```.

For students who programming with C++, consider working with ```regex_search()``` instead of ```regex_match()```. Please work with ```regcomp()``` and ```regexec()``` for students who are programming with C to implement this feature.

## More Outputs
Your program has to order the output lines by performing a numeric sort against process ID (PIDs). We will test your program with and without root permission. If an operation does not have sufficient permission to perform, you have to print out ```Permission denied``` message.

### Run the command without root permission
```
$ ./hw1| head -n 20
COMMAND         PID		USER		FD		TYPE		NODE		NAME
systemd         1		root		cwd		unknown				    /proc/1/cwd (Permission denied)
systemd         1		root		rtd		unknown				    /proc/1/root (Permission denied)
systemd         1		root		txt		unknown				    /proc/1/exe (Permission denied)
systemd         1		root		NOFD						    /proc/1/fd (Permission denied)
kthreadd        2		root		cwd		unknown				    /proc/2/cwd (Permission denied)
kthreadd        2		root		rtd		unknown				    /proc/2/root (Permission denied)
kthreadd        2		root		txt		unknown				    /proc/2/exe (Permission denied)
kthreadd        2		root		NOFD						    /proc/2/fd (Permission denied)
kworker/0:0H    4		root		cwd		unknown				    /proc/4/cwd (Permission denied)
kworker/0:0H    4		root		rtd		unknown				    /proc/4/root (Permission denied)
kworker/0:0H    4		root		txt		unknown				    /proc/4/exe (Permission denied)
kworker/0:0H    4		root		NOFD						    /proc/4/fd (Permission denied)
mm_percpu_wq    6		root		cwd		unknown				    /proc/6/cwd (Permission denied)
mm_percpu_wq    6		root		rtd		unknown				    /proc/6/root (Permission denied)
mm_percpu_wq    6		root		txt		unknown				    /proc/6/exe (Permission denied)
mm_percpu_wq    6		root		NOFD						    /proc/6/fd (Permission denied)
ksoftirqd/0     7		root		cwd		unknown				    /proc/7/cwd (Permission denied)
ksoftirqd/0     7		root		rtd		unknown				    /proc/7/root (Permission denied)
ksoftirqd/0     7		root		txt		unknown				    /proc/7/exe (Permission denied)
``` 

### Run the command with root permission
```
$ sudo ./hw1| head -n 20
COMMAND		PID		USER		FD		TYPE		NODE		NAME
systemd		1		root		cwd		DIR		    2		    /
systemd		1		root		rtd		DIR		    2		    /
systemd		1		root		txt		REG		    23734219	/lib/systemd/systemd
systemd		1		root		DEL		REG		    23724387	/lib/x86_64-linux-gnu/libm-2.27.so
systemd		1		root		mem		REG		    23724289	/lib/x86_64-linux-gnu/libudev.so.1.6.9
systemd		1		root		mem		REG		    23724380	/lib/x86_64-linux-gnu/libgpg-error.so.0.22.0
systemd		1		root		mem		REG		    23724264	/lib/x86_64-linux-gnu/libjson-c.so.3.0.1
systemd		1		root		mem		REG		    24775614	/usr/lib/x86_64-linux-gnu/libargon2.so.0
systemd		1		root		mem		REG		    23724307	/lib/x86_64-linux-gnu/libdevmapper.so.1.02.1
systemd		1		root		mem		REG		    23725490	/lib/x86_64-linux-gnu/libattr.so.1.1.0
systemd		1		root		mem		REG		    23725195	/lib/x86_64-linux-gnu/libcap-ng.so.0.0.0
systemd		1		root		mem		REG		    23724192	/lib/x86_64-linux-gnu/libuuid.so.1.3.0
systemd		1		root		DEL		REG		    23724382	/lib/x86_64-linux-gnu/libdl-2.27.so
systemd		1		root		mem		REG		    23724639	/lib/x86_64-linux-gnu/libpcre.so.3.13.3
systemd		1		root		DEL		REG		    23724459	/lib/x86_64-linux-gnu/libpthread-2.27.so
systemd		1		root		mem		REG		    24772961	/usr/lib/x86_64-linux-gnu/liblz4.so.1.7.1
systemd		1		root		mem		REG		    23725142	/lib/x86_64-linux-gnu/liblzma.so.5.2.2
systemd		1		root		mem		REG		    23724150	/lib/x86_64-linux-gnu/libidn.so.11.6.16
systemd		1		root		mem		REG		    24772626	/usr/lib/x86_64-linux-gnu/libip4tc.so.0.1.0
```


