### This program monitors a inbox directory for image files
- directory inbox
- image file is processed and result is placed into an outbox directory

Goals
- Simplicity. A small example code snippet should be enough to get the feel of the library and be ready to use its basic features.
- Extensibility. A user should be able to extend functionality of the library for collecting and storing information into logs.
- Performance. The library should have as little performance impact on the user's application as possible.

### Build
```shell
> gti clone  git@github.com:stjohnd777/mailbox.git 
> cd mailbox && mkdir build && cd build && cmake .. && make
```
## switches
| Switch       | Description                                                                    |
|--------------|--------------------------------------------------------------------------------|
| -i  <dir>    | set the directory to watch defaulted to /data/inbox                            |
| -o  <dir>    | set the directory to place result defaulted to /data/outbox is not part in chain |
| -e  <dir>    | set the error log dir  default /data/errbox                                    |
| -c  <dir>    | command directory default /data/cmd                                            |
| -c  <string> | string, comma seperated processing chain  ie -p  nuc,remap,threshold,fast      |

### shell
```shell
> build/mailbox -i /data/inbox -o /data/outbox -c /data/cmd 
```

## service
| name      | inbox                              |
|-----------|------------------------------------|
| remap     | remap                              |
| nuc       | NUC: normalized uniform correction |
| thresh    |                                    |
| fast      |                                    |
| sobel     |                                    |
| gauss     |                                    |
| median    |                                    |
| bilateral |                                    |
| disparity |                                    |


{ "service" : "nuc" , "inbox": "/data/nuc" , "outbox": "/data/nuced" }
/data/remap
/data/thresh
/data/fast
/data/sobel
/data/guass
/data/median
/data/bilateral
/data/disparity

### How Monitor File Events
- inotify mechanism for monitoring file events.
- https://www.linuxjournal.com/article/8478
- https://man7.org/linux/man-pages/man7/inotify.7.html
- The Linux Programming interface, Chapter 19 pg. 419

### Designed to implement an image processing chain
- fault tolerant 
- scalable
- persistent storage
- SOA

### Growth
- memory mapped file
- shared memory
- message oriented middleware

### Targeting Linux System
- generic posix 
- zcu102, zcu104

### Terms
- port
- node
- directories
  - inbox
  - outbox
  - err_box
  - log
  - cmd_box
-File
  - pulse

### State
- image file exists in inbox directory when process starts polling
  - process the images
- image file is moved into directory from another process
  - process image when copy is complete
- process creates file in inbox directory, writes to the file, then closes the file
  - process file when the file is close on write