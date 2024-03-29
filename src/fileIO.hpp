/*
 * fileIO.hpp
 *
 *  Created on: Dec 24, 2012
 *      Author: satya
 */

#ifndef FILEIO_HPP_
#define FILEIO_HPP_


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include"globals.hpp"



using namespace std;


//singleton class
class bptFileIO
{


		 static bool bptFileIOInstanceFlag;
		 static bptFileIO *indexFileIO;
		 int nextPageNumber;
		 int fd;

		bptFileIO()
		 	{

				fd = open("index",O_CREAT | O_RDWR , S_IRUSR|S_IWUSR,777);
		 		char buffer[sizeof(int)];
		 		lseek(fd,0,SEEK_SET);
				if(read(fd,buffer,sizeof(int)) == 0)
				{
					nextPageNumber = 0;
				}
				else
				{
		 			memcpy(&nextPageNumber,buffer,sizeof(int));
				}

		 	}


public:
	static bptFileIO* getBptFileIOInstance();
	char* getPage(long int pageNumber,char priority);
	void writePage(long int pageNumber,char* pageData);
	void updatePageNumber();


	 //getters and setters

	int getNextPageNumber()
	{
		nextPageNumber = nextPageNumber + 1;
		return (nextPageNumber);
	}


	void setNextPageNumber(int nextPageNumber )
	{
		this->nextPageNumber = nextPageNumber;
	}

	~bptFileIO()
	{



		close(fd);
		delete indexFileIO;

	}


};


bool bptFileIO::bptFileIOInstanceFlag = false;
bptFileIO* bptFileIO::indexFileIO = NULL;

bptFileIO* bptFileIO::getBptFileIOInstance()
{
	if(indexFileIO == NULL)
	{
		indexFileIO = new bptFileIO();
		bptFileIOInstanceFlag = true;
		return indexFileIO;
	}
	else return indexFileIO;
}



char* bptFileIO::getPage(long int pageNumber,char priority)
{

	if(pageNumber == -1)
	{	cout<<"page cannot be retrived with the given pageNumber"<<endl;
		return NULL;
	}


	long int offset;
	char* buffer = new char[PAGESIZE];
	offset = pageNumber*PAGESIZE;
	lseek(fd,offset,SEEK_SET);
	read(fd, buffer, PAGESIZE);
	return buffer;
}

void bptFileIO::writePage(long int pageNumber,char* pageData)
{


	long int offset;

	if(pageNumber == -1)
		cout<<"page cannot be written with the given pageNumber"<<endl;
	offset = pageNumber*PAGESIZE;
    lseek(fd,offset,SEEK_SET);
	write(fd, pageData, PAGESIZE);
	//free(pageData);

	return ;
}

void bptFileIO::updatePageNumber()
{
			char buffer[sizeof(int)];
			memcpy(buffer,&nextPageNumber,sizeof(int));
			lseek(fd,0,SEEK_SET);
			write(fd,buffer,sizeof(int));

}


#endif /* FILEIO_HPP_ */

