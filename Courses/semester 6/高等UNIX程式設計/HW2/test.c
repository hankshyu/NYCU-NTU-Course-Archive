#include <stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

//#define PRINT_HT
void etest(){
	
	#ifdef PRINT_HT
	for(int i=0;i<=50;++i)
		printf("--");
	printf("\n");
	#endif
}

void stest(const char* tname){
	#ifdef PRINT_HT
	etest();
	printf("This is the test of function: %s\n",tname);
	#endif
}

const char* filename1= "temp1.tx";
const char* filename2= "temp2.tx";

int main(int argc, char *argv[]){

	stest("creat");
	int creatreturn=creat(filename1,0600);
	if(creatreturn<0) printf("create fail!\t");
	else printf("create success!\t");
	printf("create return value: %d\n",creatreturn);

	etest();

	stest("chmod");
	int chmodreturn = chmod(filename1,0666);
	if(chmodreturn<0) printf("chmod fail!\t");
	else printf("create success!\t");
	printf(",returns value: %d\n",chmodreturn);
	etest();

	stest("chown");
	int chownreturn = chown(filename1,345,123);
	if(chownreturn<0) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",chownreturn);
	etest();

	stest("rename");
	int renamereturn = rename(filename1,filename2);
	if(renamereturn<0) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",renamereturn);
	etest();

	
	stest("open");
	int fd = open(filename2,1101,0666);
	if(fd<0) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",fd);
	etest();

	stest("write");
	int writereturn = write(fd,"cccc.",5);
	if(writereturn<0) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",writereturn);
	etest();

	stest("close");
	int closereturn = close(fd);
	if(closereturn<0) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",closereturn);
	etest();

	stest("open");
	int fd2 = open(filename2,000,0000);
	if(fd2<0) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",fd2);
	etest();

	stest("read");
	char readbuff[1024];
	int readreturn = read(fd2,readbuff,5);
	if(readreturn<0) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",readreturn);
	printf("Read value:%s\n",readbuff);
	etest();

	stest("close");
	closereturn = close(fd2);
	if(closereturn<0) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",closereturn);
	etest();

	stest("tmpfile");
	FILE *tmpfilereturn = tmpfile();
	if(tmpfilereturn==NULL) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %p\n",tmpfilereturn);

	etest();

	stest("fwrite");
	char fwritestr[1024] = "Slava Ukraini!";
	size_t fwritereturn = fwrite(fwritestr,sizeof(char),sizeof(fwritestr),tmpfilereturn);
	if(fwritereturn<=0)printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %lu\n",fwritereturn);
	etest();


	stest("fclose");
	int fclosereturn = fclose(tmpfilereturn);
	if(fclosereturn!=0)printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",fclosereturn);
	etest();

	stest("fopen");
	FILE *fopenreturn = fopen(filename2,"r");
	if(fopenreturn==NULL)printf("Fail!\n");
	else printf("Success!\n");
	etest();

	stest("fread");
	char freadstr [1024];
	size_t freadreturn = fread(freadstr,sizeof(char),sizeof(freadstr),fopenreturn);
	if(freadreturn<0)printf("Fail!\t");
	else printf("Success!\t");
	printf("Read String: %s, Return value: %d\n",freadstr,fclosereturn);
	etest();

	stest("fclose");
	int fclosereturn2 = fclose(fopenreturn);
	if(fclosereturn2!=0)printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",fclosereturn2);
	etest();

	stest("remove");
	int removereturn = remove(filename2);
	if(removereturn <0) printf("Fail!\t");
	else printf("Success!\t");
	printf("Return value: %d\n",removereturn);
	etest();
}
