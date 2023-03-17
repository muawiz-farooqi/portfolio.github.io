// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments
#define NUM_BLOCKS 4226			// Number of blocks in a filesystem
#define BLOCK_SIZE 8192			// Size of a block in the filesystem
#define NUM_FILES 125			// Number of files in a filesystem
#define NUM_INODES 125			// Number of inodes in a filesystem
#define MAX_BLOCKS_PER_FILE 32	// Maximum number of blocks in a file
#define MAX_FILENAME 32			// Max character length of a file
#define MAX_FILE_SIZE 10240000	// Max size of a file that can be stored in the filesystem

// data_blocks array used to store filesystem in temporary memory
unsigned char data_blocks[NUM_BLOCKS][BLOCK_SIZE];
// name of filesystem in use
char filesystem[MAX_FILENAME] = "";
// free block map
u_int8_t * used_blocks;
// free inode map
u_int8_t * used_inodes;

// directory struct to store file information
struct directory_entry
{
	char name[MAX_FILENAME];
	int valid;
	int size;
	int inode_index;
	int read_only;
	int hidden;
};

// stores a file directory
struct directory_entry * directory_ptr;

// inode struc to track inode information for each file
struct inode
{
	time_t date;
	int valid;
	int blocks[MAX_BLOCKS_PER_FILE];
};

// points to an array of inodes
struct inode * inode_array_ptr[NUM_INODES];

// returns number of free blocks in a filesystem in bytes
int df()
{
	// if no file system open, display error message
	if (!strcmp(filesystem, ""))
	{
		printf("df: No file has been opened\n");
		return -1;
	}

	int count = 0;

	for (int i = 130; i < NUM_BLOCKS; i++)
	{
		// if free block found, increase free block count
		if (used_blocks[i] == 0)
		{
			count++;
		}
	}
	// return number of free bytes in an array
	return count * BLOCK_SIZE;
}

// return index of free block given an inode index
int findFreeInodeBlockEntry(int inode_index)
{
	int i ;
	int retval = -1;

	for (int i = 0; i < MAX_BLOCKS_PER_FILE; i++)
	{
		if (inode_array_ptr[inode_index]->blocks[i] == -1)
		{
			retval = i;
			break;
		}
	}

	return retval;
}

// initialize memory and values to filesystem
void init()
{
	// initialize memory to directory
	directory_ptr = (struct directory_entry*) &data_blocks[0];

	// initialize values to directory
	for (int i = 0; i < NUM_FILES; i++)
	{
		directory_ptr[i].valid = 0;
		directory_ptr[i].inode_index = -1;
		directory_ptr[i].size = -1;
		directory_ptr[i].read_only = 0;
		directory_ptr[i].hidden = 0;
	}

	// initialize memory for free inode map
	used_inodes = (u_int8_t *) &data_blocks[2];

	// initialize values for free inode map
	for (int i = 0; i < NUM_INODES; i++)
	{
		used_inodes[i] = 0;
	}

	// initialize memory for free block map
	used_blocks = (u_int8_t *) &data_blocks[3];

	// initialize values for free block map
	for (int i = 130; i < NUM_BLOCKS; i++)
	{
		used_blocks[i] = 0;
	}

	// initialize memory for inode blocks
	int inode_index = 0;
	for (int i = 5; i < 130; i++)
	{
		inode_array_ptr[inode_index++] = (struct inode*) &data_blocks[i];
	}

	// initialize values for inode blocks
	for (int i = 0; i < NUM_INODES; i++)
	{
		inode_array_ptr[i]->valid = 0;
		for (int j = 0; j < MAX_BLOCKS_PER_FILE; j++)
		{
			inode_array_ptr[i]->blocks[j] = -1;
		}
	}
}

// returns index of free directory
int findFreeDirectoryEntry()
{
	int retval = -1;
	for (int i = 0; i < NUM_FILES; i++)
	{
		if (directory_ptr[i].valid == 0)
		{
			retval = i;
			break;
		}
	}

	return retval;
}

// returns index of free inode in inode array
int findFreeInode()
{
	int retval = -1;

	for (int i = 0; i < NUM_INODES; i++)
	{
		if (inode_array_ptr[i]->valid == 0)
		{
			retval = i;
			break;
		}
	}

	return retval;
}

// returns index of free block in memory
int findFreeBlock()
{
	int retval = -1;

	for (int i = 130; i < NUM_BLOCKS; i++)
	{
		if (used_blocks[i] == 0)
		{
			retval = i;
			break;
		}
	}

	return retval;
}

// creates a file system image file using user provided name
void createfs(char * disk_image_name)
{
	// print error if a filesystem is already open
	if (strcmp(filesystem, ""))
	{
		printf("createfs: Close %s before creating new file system\n", filesystem);
		return;
	}

	// print error if no name defined by user
	if (disk_image_name == NULL)
	{
		printf("createfs: File not found.\n");
	}
	else
	{
		// create new filesystem image
		FILE *fs_fp = fopen(disk_image_name, "w");
		// initialize data blocks array
		init();
		// initialize image file with initialized data blocks array
		for (int i = 0; i < NUM_BLOCKS; i++)
		{
			fwrite(data_blocks[i], BLOCK_SIZE, 1, fs_fp);
		}

		fclose(fs_fp);
	}
}

// open existing file system image passed in by user
void open(char * disk_image_name)
{
	// a file system is already open
	if (strcmp(filesystem, ""))
	{
		printf("open: Close %s before opening new file system\n", filesystem);
		return;
	}

	FILE *fs_fp = fopen(disk_image_name, "r");

	// file system not found
	if (fs_fp == NULL)
	{
		printf("open: File not found\n");
		return;
	}

	// store name of file system
	strncpy(filesystem, disk_image_name, strlen(disk_image_name));

	// copy filesystem into memory
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		fread(data_blocks[i], BLOCK_SIZE, 1, fs_fp);
	}

	fclose(fs_fp);
}

// write the filesystem to disk
void savefs()
{
	// a file ssytem is already open
	if (!strcmp(filesystem, ""))
	{
		printf("savefs: No file system has been opened\n");
		return;
	}

	FILE *fs_fp = fopen(filesystem, "w");

	// write filesystem to disk
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		fwrite(data_blocks[i], BLOCK_SIZE, 1, fs_fp);
	}

	fclose(fs_fp);
}

// close opened filesystem
void closefs()
{
	// empty name of filesystem open
	memset(filesystem, 0, MAX_FILENAME*sizeof(char));
	// clear filesystem from memory
	memset(data_blocks, 0, NUM_BLOCKS*BLOCK_SIZE*sizeof(unsigned char));
}

// put file into filesystem
void put(char * filename)
{
	struct stat buf;

	// get file information
	int status = stat(filename, &buf);

	// file not found
	if (status == -1)
	{
		printf("put error: File not found\n");
		return;
	}

	int duplicate_dir_index = -1;
	// find name of file in filesystem
	for (int i = 0; i < NUM_FILES; i++)
	{
		if (!strcmp(directory_ptr[i].name, filename))
		{
			duplicate_dir_index = i;
			break;
		}
	}
	// file already in filesystem
	if (duplicate_dir_index != -1)
	{
		printf("df: File already present in filesystem.\n");
		return;
	}

	// check if filename too long
	if (strlen(filename) > MAX_FILENAME)
	{
		printf("put error: File name too long.\n");
		return;
	}

	// check if enough space in memory
	if (buf.st_size > df())
	{
		printf("put error: Not enough disk space.\n");
		return;
	}

	// check if file too big
	if (buf.st_size > MAX_FILE_SIZE)
	{
		printf("put error: File too large.[%d]\n", buf.st_size);
		return;
	}

	// find free directory index
	int dir_index = findFreeDirectoryEntry();
	// check if directory full
	if (dir_index == -1)
	{
		printf("put error: Not enough disk space.\n");
		return;
	}

	// set directory entry to in use
	directory_ptr[dir_index].valid = 1;

	// store filename in directory
	strncpy(directory_ptr[dir_index].name, filename, strlen(filename));

	// find free inode index
	int inode_index = findFreeInode();
	if (inode_index == -1)
	{
		printf("Error: No free inodes\n");
		return;
	}

	// store filesize, inode index in directory
	directory_ptr[dir_index].inode_index = inode_index;
	directory_ptr[dir_index].size = buf.st_size;

	// store date, valid in inode array
	inode_array_ptr[inode_index]->date = time(NULL);
	inode_array_ptr[inode_index]->valid = 1;

	// set inode to used in inode map
	used_inodes[inode_index] = 1;

	// Open the input file read-only 
	FILE *ifp = fopen (filename, "r");

	// set copy size to size of file
	int copy_size = buf.st_size;
	int offset = 0;

	while (copy_size >= BLOCK_SIZE)
	{
		// find free block index
		int block_index = findFreeBlock();

		if (block_index == -1)
		{
			// no block index found, should not occur
			printf("Error: Can't find free block [BUG!!]\n");
			fclose(ifp);
			return;
		}

		// update free block map		
		used_blocks[block_index] = 1;

		// find free inode block index
		int inode_block_entry = findFreeInodeBlockEntry(inode_index);
		if (inode_block_entry == -1)
		{
			// no inode block index found, should not occur
			printf("Error: [246]Can't find free inode block [BUG!!]\n");
			fclose(ifp);
			return;
		}
		// store block index in inode array
		inode_array_ptr[inode_index]->blocks[inode_block_entry] = block_index;

		// move file pointer to new read postion
		fseek(ifp, offset, SEEK_SET);

		// read data into memory, store num bytes read
		int bytes_read = fread(data_blocks[block_index], BLOCK_SIZE, 1, ifp);

		// error: nobytes read despite not end of file
		if(bytes_read == 0 && !feof(ifp))
		{
			printf("An error occured reading from the input file.\n");
			fclose(ifp);
			return;
		}

		// remove error
		clearerr(ifp);

		// decrease copy size by block size
		copy_size -= BLOCK_SIZE;
		// decrease offset by block size
		offset -= BLOCK_SIZE;
	}

	// if copy size is less than block size, greather than 0
	if (copy_size > 0)
	{
		// find free block
		int block_index = findFreeBlock();

		// error message
		if (block_index == -1)
		{
			printf("Error: Can't find free block [BUG!!]\n");
			fclose(ifp);
			return;
		}

		// update free block map
		used_blocks[block_index] = 1;

		// free free inode block
		int inode_block_entry = findFreeInodeBlockEntry(inode_index);
		if (inode_block_entry == -1)
		{
			printf("Error: [284]Can't find free inode block [BUG!!]\n");
			//Clean up directory and inodes
			fclose(ifp);
			return;
		}
		inode_array_ptr[inode_index]->blocks[inode_block_entry] = block_index; 

		// handle remainder
		fseek(ifp, offset, SEEK_SET);
		int bytes = fread(data_blocks[block_index], copy_size, 1, ifp);
	}

	fclose(ifp);
}

// get file from filesystem and store on disk using user-provided/original name
void get(char * filename, char * newfilename)
{
	FILE *ofp;

	// if no new name entered
	if (newfilename == NULL)
	{
		// current file is set to output file
		newfilename = filename;
	}
	
	// open output file
	ofp = fopen(newfilename, "w");	

	// find free directory index of corresponding filename
	int dir_index = -1;
	for (int i = 0; i < NUM_FILES; i++)
	{
		if (!strcmp(directory_ptr[i].name, filename) && directory_ptr[i].valid == 1)
		{
			dir_index = i;
			break;
		}
	}
	// filename not found
	if (dir_index == -1)
	{
		printf("get error: File not found.\n");
		fclose(ofp);
		return;
	}

	// Initialize our offsets and pointers just we did above when reading from the file.
	int inode_index = directory_ptr[dir_index].inode_index;
	int copy_size = directory_ptr[dir_index].size;
	int inode_block_entry = 0;
	int offset = 0;

	while(copy_size > 0)
	{ 
		// increment index while empty block encountered
		while(inode_array_ptr[inode_index]->blocks[inode_block_entry] == -1)
		{
			inode_block_entry++;
			// if block index is greater than max, print error
			if (inode_block_entry > MAX_BLOCKS_PER_FILE)
			{
				printf("get: No data blocks found for file %s\n", filename);
				fclose(ofp);
				return;
			}
		}

		// get block index from inode array
		int block_index = inode_array_ptr[inode_index]->blocks[inode_block_entry++];
		if (block_index == -1)
		{
			printf("Read into overflow memory [BUG]\n");
			fclose(ofp);
			return;
		}

		int num_bytes;

		// If the remaining number of bytes we need to copy is less than BLOCK_SIZE then
		// only copy the amount that remains. else set it as block size
		if( copy_size < BLOCK_SIZE )
		{
			num_bytes = copy_size;
		}
		else 
		{
			num_bytes = BLOCK_SIZE;
		}

		// Write num_bytes number of bytes from our data array into our output file.
		int bytes_written = fwrite(data_blocks[block_index], num_bytes, 1, ofp); 

		// error occured
		if(bytes_written == 0 && !feof(ofp))
		{
			printf("An error occured reading from the output file.\n");
			fclose(ofp);
			return;
		}

		clearerr(ofp);

		// Reduce the amount of bytes remaining to copy, increase the offset into the file
		// and increment the block_index to move us to the next data block.
		copy_size -= BLOCK_SIZE;
		offset += BLOCK_SIZE;

		// update file pointer location unless file completely copied
		if (copy_size > 0)
		{
			fseek(ofp, offset, SEEK_SET);
		}
	}

	// Close the output file, we're done. 
	fclose(ofp);
}

// deletes a file in the filesystem using user-provided filename
void del(char * filename)
{
	// find filename in directory
	int dir_index = -1;
	for (int i = 0; i < NUM_FILES; i++)
	{
		if (!strcmp(directory_ptr[i].name, filename))
		{
			dir_index = i;
			break;
		}
	}
	// file not found
	if (dir_index == -1)
	{
		printf("del error: File not found.\n");
		return;
	}
	// file read-only: cannot delete
	if (directory_ptr[dir_index].read_only == 1)
	{
		printf("del: file cannot be deleted (read only)\n");
		return;
	}

	// file set to invalid in directory
	directory_ptr[dir_index].valid = 0;

	int inode_index = directory_ptr[dir_index].inode_index;
	// inode index set to invalid in inode array
	inode_array_ptr[inode_index]->valid = 0;

	// set all corresponding blocks in free block map to free
	for (int i = 0; i < MAX_BLOCKS_PER_FILE; i++)
	{
		if (inode_array_ptr[inode_index]->blocks[i] != -1)
		{
			used_blocks[inode_array_ptr[inode_index]->blocks[i]] = 0;
		}
	}
}

// undeletes a file in the filesystem using user-provided filename
void undel(char * filename)
{
	// find filename in directory
	int dir_index = -1;
	for (int i = 0; i < NUM_FILES; i++)
	{
		if (!strcmp(directory_ptr[i].name, filename))
		{
			dir_index = i;
			break;
		}
	}
	// file not found
	if (dir_index == -1)
	{
		printf("del error: File not found.\n");
		return;
	}
	// file set to vaild in directory
	directory_ptr[dir_index].valid = 1;

	int inode_index = directory_ptr[dir_index].inode_index;
	// inode index set to valid in inode array
	inode_array_ptr[inode_index]->valid = 1;

	// set all corresponding blocks in free block map to used
	for (int i = 0; i < MAX_BLOCKS_PER_FILE; i++)
	{
		if (inode_array_ptr[inode_index]->blocks[i] != -1)
		{
			used_blocks[inode_array_ptr[inode_index]->blocks[i]] = 1;
		}
	}
}

// lists files and file information for all files in the filesystem
void list()
{
	// no filesystem open
	if (!strcmp(filesystem, ""))
	{
		printf("list: No filesystem has been opened\n");
		return;
	}

	// tm struct to output custom time information
	struct tm * tm_info;
	// store custom file time
	char filetime[32];

	for (int i = 0; i < NUM_FILES; i++)
	{
		// if non-hidden valid directory found
		if (directory_ptr[i].valid == 1 && directory_ptr[i].hidden == 0)
		{
			int inode_index = directory_ptr[i].inode_index;

			// store time information from date in directory
			tm_info = localtime(&inode_array_ptr[inode_index]->date);

			// store custom time in filetime using tm_info
			strftime(filetime, 32, "%b %d %R", tm_info);
			// print file size, date, name
			printf("%-6d %s %s\n",
				directory_ptr[i].size, filetime, directory_ptr[i].name);
		}
	}
}

// adds or removes an attribute from a file in the filesystem
void attrib(char * attrib_change, char * filename)
{
	// find filename in directory
	int dir_index = -1;
	for (int i = 0; i < NUM_FILES; i++)
	{
		if (!strcmp(directory_ptr[i].name, filename))
		{
			dir_index = i;
			break;
		}
	}
	// file not in directory
	if (dir_index == -1)
	{
		printf("attrib: File not found.\n");
		return;
	}
	// check attribute format
	if (strlen(attrib_change) != 2)
	{
		printf("attrib: invalid attribute\n");
		return;
	}

	// store + or - sign
	char sign = attrib_change[0];
	// store attribute
	char attrib = attrib_change[1];

	switch (attrib)
	{
		case 'r':
			// set to read-only
			if (sign == '+')
			{
				directory_ptr[dir_index].read_only = 1;
			}
			// remove read-only
			else if (sign == '-')
			{
				directory_ptr[dir_index].read_only = 0;
			}
			// invalid sign
			else
			{
				printf("attrib: invalid attribute\n");
				return;
			}
			break;

		case 'h':
			// set to hidden
			if (sign == '+')
			{
				directory_ptr[dir_index].hidden = 1;
			}
			// remove hidden
			else if (sign == '-')
			{
				directory_ptr[dir_index].hidden = 0;
			}
			// invalid sign
			else
			{
				printf("attrib: invalid attribute\n");
				return;
			}
			break;

		default:
			// invalid attribute
			printf("attrib: invalid attribute\n");
			return;
	}
}

int main()
{
	// allocate memory for string read in
	char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

	while( 1 )
	{
		// Print out the mfs prompt
		printf ("mfs> ");

		// Read the command from the commandline.  The
		// maximum command that will be read is MAX_COMMAND_SIZE
		// This while command will wait here until the user
		// inputs something since fgets returns NULL when there
		// is no input
		while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

		/* Parse input */
		char *token[MAX_NUM_ARGUMENTS];

		int   token_count = 0;                                 
		               
		// Pointer to point to the token
		// parsed by strsep
		char *arg_ptr;                                         
		               
		char *working_str  = strdup( cmd_str );                

		// we are going to move the working_str pointer so
		// keep track of its original value so we can deallocate
		// the correct amount at the end
		char *working_root = working_str;

		// Tokenize the input stringswith whitespace used as the delimiter
		while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
		(token_count<MAX_NUM_ARGUMENTS))
		{
			token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
			if( strlen( token[token_count] ) == 0 )
			{
				token[token_count] = NULL;
			}
			token_count++;
		}

		

		if (token[0] == NULL)
		{
			// skip/ignore empty line
		}
		else if (!strcmp("put", token[0]))
		{
			// put file into filesystem
			put(token[1]);
		}
		else if (!strcmp("get", token[0]))
		{
			// get file from filesystem and store on disk
			get(token[1], token[2]);
		}
		else if (!strcmp("del", token[0]))
		{
			// delete file from filesystem
			del(token[1]);
		}
		else if (!strcmp("undel", token[0]))
		{
			// undelete file from filesystem
			undel(token[1]);
		}
		else if (!strcmp("list", token[0]))
		{
			// list files in filesystem
			list();
		}
		else if (!strcmp("df", token[0]))
		{
			// find number of free bytes in filesystem
			int size = df();
			// print only if valid size returned (no error)
			if (size != -1)
			{
				printf("%d bytes free.\n", size);
			}
		}
		else if (!strcmp("open", token[0]))
		{
			// initialize data blocks array
			init();
			// open filesystem and store in memory
			open(token[1]);
		}
		else if (!strcmp("close", token[0]))
		{
			// close opened filesystem
			closefs();
		}
		else if (!strcmp("createfs", token[0]))
		{
			// create new filesystem
			createfs(token[1]);
		}
		else if (!strcmp("savefs", token[0]))
		{
			// save filesystem to disk
			savefs();
		}
		else if (!strcmp("attrib", token[0]))
		{
			// add or remove an attribute from file in filesystem
			attrib(token[1], token[2]);
		}
		else if (!strcmp("quit", token[0]))
		{
			// quit program
			return 1;
		}
		else
		{
			// invalid command
			printf("%s: invalid command.\n", token[0]);
		}

		// free dynamically allocated memory
		free( working_root );

	}
	return 0;
}
