#define _GNU_SOURCE
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <fcntl.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

//#define PRINT_HACK

//helper function which translates fd to real paths
void fdpath(const int fd, char* fdname){
	// pid_t pid;
	// pid = getpid();
	char linkname[1024];
	//sprintf(linkname,"/proc/%d/%d",pid,fd);
	sprintf(linkname,"/proc/self/fd/%d",fd);
	int readlinkreturn = readlink(linkname,fdname,sizeof(fdname));
	if(readlinkreturn<0) strcpy(fdname, ".");
	else fdname[readlinkreturn]='\0';
}

//helper function which translates relative paths to real paths
void rpath(const char *path, char *rtpath){
	
	char realpathstr[1024];
	char *rp;

	rp=realpath(path,realpathstr);
	if(rp!=NULL){
		strcpy(rtpath,realpathstr);
	}else{
		strcpy(rtpath,path);
	}
}

void printhack(const char* fncname){
	#ifdef PRINT_HACK
	printf("This is function %s, HACKED!!\n",fncname);
	#endif
}

//safely print strings without unprintable characters with up to 32 bytes
void safestr(char *newstr, const char *oldstr){
    strcpy(newstr,oldstr);
    for (int i=0;i<strlen(oldstr);++i){
        

        newstr[i] = isprint(oldstr[i])?oldstr[i] : '.';
        if(i==31){
            newstr[32]='\0';
            break;
        }
    }
}


int chmod(const char*path, mode_t mode){
	
	const char* funcname="chmod";
	printhack(funcname);
	typedef int (*origin_chmod_type) (const char*,mode_t);
	int answer;

	origin_chmod_type orig_chmod;
	orig_chmod = (origin_chmod_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_chmod(path,mode);

	char realpathstr[1024];
	// memset(realpathstr,0,sizeof(realpathstr));

	char saferealpathstr[1024];
	// memset(saferealpathstr,0,sizeof(saferealpathstr));
	
	rpath(path,realpathstr);
	safestr(saferealpathstr,realpathstr);
	fprintf(stderr,"[logger] %s(\"%s\", %o) = %d\n",funcname,saferealpathstr,mode,answer);

	return answer;
}

int chown(const char *path, uid_t owner, gid_t group){

	const char* funcname="chown";
	printhack(funcname);
	typedef int (*origin_fnc_type) (const char*,uid_t,gid_t);
	int answer;   

	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(path,owner,group);

	char realpathstr[1024];
	// memset(realpathstr,0,sizeof(realpathstr));
	rpath(path,realpathstr);
	char saferealpathstr[1024];
	// memset(saferealpathstr,0,sizeof(saferealpathstr));
	safestr(saferealpathstr,realpathstr);
	fprintf(stderr,"[logger] %s(\"%s\", %u, %u) = %d\n",funcname,saferealpathstr,owner,group,answer);

	return answer;

}

int close(int fd){

	const char* funcname="close";
	printhack(funcname);
	typedef int (*origin_fnc_type) (int);
	int answer;

	char fdname [1024];
	// memset(fdname,0,sizeof(fdname));
	char safefdname [1024];
	// memset(safefdname,0,sizeof(safefdname));
	fdpath(fd,fdname);
	safestr(safefdname,fdname);


	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(fd);

	fprintf(stderr,"[logger] %s(\"%s\") = %d\n",funcname,safefdname,answer);

}

int creat(const char*pathname, mode_t mode){
	
	const char* funcname="creat";
	printhack(funcname);
	int answer;
	typedef int (*origin_fnc_type) (const char*, mode_t);

	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(pathname, mode);

	char realpathstr[1024];
	// memset(realpathstr,0,sizeof(realpathstr));
	char saferealpathstr[1024];
	// memset(saferealpathstr,0,sizeof(saferealpathstr));
	rpath(pathname,realpathstr);
	safestr(saferealpathstr,realpathstr);
	fprintf(stderr,"[logger] %s(\"%s\", %o) = %d\n",funcname,saferealpathstr,mode,answer);

	return answer;
}


int fclose(FILE *stream){
	const char* funcname="fclose";
	printhack(funcname);
	int answer;
	typedef int (*origin_fnc_type) (FILE*);


	char *fdname =(char*)calloc(1024,sizeof(char));
	char proclnk [1024];
	int tmpfd=fileno(stream);

	sprintf(proclnk, "/proc/self/fd/%d", tmpfd);
    ssize_t r = readlink(proclnk, fdname, 1024);
	if(r<0)strcpy(fdname,".");

	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(stream);

	char safefdname [1024];
	// memset(safefdname,0,sizeof(safefdname));

	safestr(safefdname,fdname);
	fprintf(stderr,"[logger] %s(\"%s\") = %d\n",funcname,safefdname,answer);
	free(fdname);
	return answer;


}

FILE *fopen(const char*pathname,const char*mode){
	
	const char* funcname="fopen";
	printhack(funcname);
	
	typedef FILE* (*origin_open_f_type) (const char*,const char*);//define the original funciton pointer
	FILE *answer;

	//the original function 	
	origin_open_f_type orig_open;
	orig_open = (origin_open_f_type)dlsym(RTLD_NEXT,funcname);
	answer=orig_open(pathname,mode);

	char realpathstr[1024];
	// memset(realpath,0,sizeof(realpath));
	char saferealpathstr[1024];
	// memset(saferealpathstr,0,sizeof(saferealpathstr));
	rpath(pathname,realpathstr);
	safestr(saferealpathstr,realpathstr);
	fprintf(stderr,"[logger] %s(\"%s\", \"%s\") = %p\n",funcname,saferealpathstr,mode,answer);

	return answer;

}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream){
	const char* funcname="fread";
	printhack(funcname);
	int answer;
	typedef size_t (*origin_fnc_type) (void *, size_t, size_t, FILE *);

	//char fdname [1024];
	char *fdname =(char*)calloc(1024,sizeof(char));
	char proclnk [1024];
	memset(proclnk,0,sizeof(proclnk));
	int tmpfd=fileno(stream);

	sprintf(proclnk, "/proc/self/fd/%d", tmpfd);
    ssize_t r = readlink(proclnk, fdname, 1024);
	if(r<0)strcpy(fdname,".");


	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(ptr,size,nmemb,stream);

	char safeptr [1024];
	memset(safeptr,0,sizeof(safeptr));
	char safefdname [1024];
	memset(safefdname,0,sizeof(safefdname));
	safestr(safeptr,(char*)ptr);
	safestr(safefdname,fdname);
	fprintf(stderr,"[logger] %s(\"%s\", %ld, %ld, \"%s\") = %d\n",funcname,safeptr,size,nmemb,safefdname,answer);

	free(fdname);
	return answer;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
	
	const char* funcname="fwrite";
	printhack(funcname);
	int answer;
	typedef size_t (*origin_fnc_type) (const void *, size_t, size_t, FILE *);

	char *fdname =(char*)calloc(1024,sizeof(char));
	char proclnk [1024];
	int tmpfd=fileno(stream);

	sprintf(proclnk, "/proc/self/fd/%d", tmpfd);
    ssize_t r = readlink(proclnk, fdname, 1024);
	if(r<0)strcpy(fdname,".");


	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(ptr,size,nmemb,stream);

	char safeptr [1024];
	// memset(safeptr,0,sizeof(safeptr));
	char safefdname [1024];
	// memset(safefdname,0,sizeof(safefdname));
	safestr(safeptr,(char*)ptr);
	safestr(safefdname,fdname);

	fprintf(stderr,"[logger] %s(\"%s\", %ld, %ld, \"%s\") = %d\n",funcname,safeptr,size,nmemb,safefdname,answer);
	free(fdname);
	return answer;
}

int open(const char *pathname, int flags, mode_t mode){
	const char* funcname="open";
	printhack(funcname);
	int answer;
	typedef int (*origin_fnc_type) (const char*, int, mode_t);

	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(pathname, flags, mode);

	char realpathstr[1024];
	// memset(realpathstr,0,sizeof(realpathstr));
	char saferealpathstr[1024];
	// memset(saferealpathstr,0,sizeof(saferealpathstr));

	rpath(pathname,realpathstr);
	safestr(saferealpathstr,realpathstr);
	fprintf(stderr,"[logger] %s(\"%s\", %d, %03o) = %d\n",funcname,saferealpathstr,flags,mode,answer);

	return answer;
}

ssize_t read(int fd, void *buf, size_t count){
	
	const char* funcname="read";
	printhack(funcname);
	int answer;
	typedef int (*origin_fnc_type) (int , void*, size_t);


	char fdname[1024];
	// memset(fdname,0,sizeof(fdname));
	fdpath(fd,fdname);

	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(fd,buf,count);

	char  safefdname [1024];
	// memset(safefdname,0,sizeof(safefdname));
	char  safebuf [1024];
	// memset(safebuf,0,sizeof(safebuf));

	safestr(safebuf,(char*)buf);
	safestr(safefdname,fdname);
	

	fprintf(stderr,"[logger] %s(\"%s\", \"%s\", %ld) = %d\n",funcname,safefdname,safebuf,count,answer);
	return answer;
}

int remove(const char *pathname){
	const char* funcname="remove";
	printhack(funcname);
	int answer;
	typedef int (*origin_fnc_type) (const char*);

	char *realpathstr=(char*)calloc(1024,sizeof(char));

	rpath(pathname,realpathstr);

	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(pathname);

	char saferealpathname [1024];

	safestr(saferealpathname,realpathstr);
	fprintf(stderr,"[logger] %s(\"%s\") = %d\n",funcname,saferealpathname,answer);

	free(realpathstr);
	return answer;
}

int rename(const char *old, const char *new){
	const char* funcname="rename";
	printhack(funcname);
	int answer;
	typedef int (*origin_fnc_type) (const char*, const char*);

	char realpatholdstr[1024];
	// memset(realpatholdstr,0,sizeof(realpatholdstr));
	char realpathnewstr[1024];
	// memset(realpathnewstr,0,sizeof(realpathnewstr));
	rpath(old,realpatholdstr);
	rpath(new,realpathnewstr);



	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(old, new);

	char saferealpatholdstr[1024];
	// memset(saferealpatholdstr,0,sizeof(saferealpatholdstr));
	char saferealpathnewstr[1024];
	// memset(saferealpathnewstr,0,sizeof(saferealpathnewstr));

	safestr(saferealpathnewstr,realpathnewstr);
	safestr(saferealpatholdstr,realpatholdstr);


	fprintf(stderr,"[logger] %s(\"%s\", \"%s\") = %d\n",funcname,saferealpatholdstr,saferealpathnewstr,answer);

	return answer;
}

FILE *tmpfile(void){
	
	const char* funcname="tmpfile";
	printhack(funcname);
	
	typedef FILE* (*origin_fnc_type) (void);//define the original funciton pointer
	FILE *answer;

	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc();

	fprintf(stderr,"[logger] %s() = %p\n",funcname,answer);
	return answer;

}

ssize_t write(int fd, const void *buf, size_t count){
	
	const char* funcname="write";
	printhack(funcname);
	int answer;
	typedef int (*origin_fnc_type) (int , const void*, size_t);


	char fdname[1024];
	// memset(fdname,0,sizeof(fdname));
	fdpath(fd,fdname);

	origin_fnc_type orig_fnc;
	orig_fnc = (origin_fnc_type)dlsym(RTLD_NEXT,funcname);
	answer = orig_fnc(fd,buf,count);

	char safefdname [1024];
	// memset(safefdname,0,sizeof(safefdname));
	char safebuf [1024];
	// memset(safebuf,0,sizeof(safebuf));

	safestr(safefdname,fdname);
	safestr(safebuf,(char*)buf);

	fprintf(stderr,"[logger] %s(\"%s\", \"%s\", %ld) = %d\n",funcname,safefdname,safebuf,count,answer);
	return answer;
}

