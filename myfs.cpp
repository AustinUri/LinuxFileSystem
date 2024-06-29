#include "myfs.h"
#include <string.h>
#include <iostream>
#include <math.h>
#include <sstream>

const char *MyFs::MYFS_MAGIC = "MYFS";

MyFs::MyFs(BlockDeviceSimulator *blkdevsim_):blkdevsim(blkdevsim_) {
	struct myfs_header header;
	blkdevsim->read(0, sizeof(header), (char *)&header);
	if (strncmp(header.magic, MYFS_MAGIC, sizeof(header.magic)) != 0 ||
	    (header.version != CURR_VERSION)) {
		std::cout << "Did not find myfs instance on blkdev" << std::endl;
		std::cout << "Creating..." << std::endl;
		format();
		std::cout << "Finished!" << std::endl;
	}
}

void MyFs::format() {

	// put the header in place
	struct myfs_header header;
	strncpy(header.magic, MYFS_MAGIC, sizeof(header.magic));
	header.version = CURR_VERSION;
	blkdevsim->write(0, sizeof(header), (const char*)&header);
    char buffer[BlockDeviceSimulator::DEVICE_SIZE-sizeof(header)] = {0};
    blkdevsim->write(sizeof(header)+1,BlockDeviceSimulator::DEVICE_SIZE-sizeof(header),buffer);
}


void MyFs::create_file(std::string path_str, bool directory){

    //check if file exsits already
    for(int i=0;i<this->list_dir(path_str).size();i++)
    {
        if(fileL[i].name == path_str)
        {
            throw std::invalid_argument("Exsits already");
        }
    }

    
    struct dir_list_entry file;
    file.file_size = 0;
    file.name = path_str;
    file.is_dir = directory;
    
    this->fileL.push_back(file);
    
}


std::string MyFs::get_content(std::string path_str) {
    char buffer[BlockDeviceSimulator::DEVICE_SIZE-sizeof(myfs_header)] = {};
    blkdevsim->read(sizeof(myfs_header)+1,BlockDeviceSimulator::DEVICE_SIZE-sizeof(myfs_header),buffer);
    std::string ans = std::string(buffer);
    return ans;
}



void MyFs::set_content(std::string path_str, std::string content) {
    
    char buffer[sizeof(content)+1] = {};
    strcpy(buffer, content.c_str()); 
    blkdevsim->write(sizeof(myfs_header)+1,sizeof(content),buffer);
}

MyFs::dir_list MyFs::list_dir(std::string path_str) {
	return this->fileL;
}

