### This program monitors a inbox directory 
- references as inbox
- image files
- image file is processed producing a result, in most cases another image. 
result is placed into an outbox directory

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