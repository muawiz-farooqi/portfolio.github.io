# File System

Linux-use only

File system that can create a file image, add, delete, undelete files, and persist after closing the file system. Also includes test image (fs.image) and files.


mfs is a user space portable index-allocated file system. The program provides the user with ~33MB of drive space in a disk image.
Users have the ability to create the filesystem image, list the files currently in the file system, add files, remove files, and save the filesystem.
Files will persist in the file system when the program exits.
Detailed explanation of the file system is provided in the Guidelines pdf.

Use makefile to build code
```
make
```

Run mfs to open mfs file system
```
./mfs
```


Commands supported:


| **Command**            | **Usage**                                         | **Description**                                                                                 |
|------------------------|---------------------------------------------------|-------------------------------------------------------------------------------------------------|
| put                    | ```put <filename>```                              | Copy the local file to the filesystem image                                                     |
| get                    | ```get <filename>```                              | Retrieve the file from the filesystem image                                                     |
| get                    | ```get <filename> <newfilename>```                | Retrieve the file form the file system image and place it in the file named &lt;newfilename&gt; |
| del                    | ```del <filename>```                              | Delete the file                                                                                 |
| undel                  | ```undel <filename>```                            | Undelete the file if found in the directory                                                     |
| list                   | ```list [-h]```                                   | List the files in the file system image                                                         |
| df                     | ```df```                                          | Display the amount of disk space left in the file system                                        |
| open                   | ```open <file image name>```                      | Open a file system image                                                                        |
| close                  | ```close```                                       | Close the currently open file system                                                            |
| createfs               | ```createfs <disk image name>```                  | Create a new file system image                                                                  |
| savefs                 | ```savefs```                                      | Save the current file system image                                                              |
| attrib                 | ```attrib [+attribute] [-attribute] <filename>``` | Set or remove the attribute for the file                                                        |
| quit                   | ```quit```                                        | Quit the application                                                                            |


Valid Attributes:

| Label                             | Description                                                    |
|-----------------------------------|----------------------------------------------------------------|
| h                                 | Hidden. The file does not display in the directory listing     |
| r                                 | Read-Only. The file is marked read-only and can not be deleted |