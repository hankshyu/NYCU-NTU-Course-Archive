#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <regex.h>
#include <assert.h>
struct pidinfo
{
    char user[1024];
    pid_t pid;
    char cmdline[1024];
    char path[1024];
    ssize_t pathlenstore;
};

struct fileinfo{
    char command [1024];
    int pid;
    char user [1024];
    char fd [1024];
    char type [1024];
    unsigned long node;
    char name [1024];
    int show;
};

struct fileinfo *ansarr [115200];
int ansarrnum;
void printans();

int isNumber(char s[]);
char *strremove(char *, const char *);
void printrow1();
void printrown(struct fileinfo*);

void show3info(pid_t);
struct fileinfo* handlespecialfiles(char [],struct pidinfo *);
void handlemapfiles(struct pidinfo*);
void handlefdfiles(struct pidinfo*);

int meetregex(char *,char*);
int main(int argc, char **argv)
{
    int copt =0, topt=0, fopt=0;
    char creg [1024];
    char treg [1024];
    char freg [1024];

    int option_val = 0;
    while ((option_val = getopt(argc, argv, "c:t:f:")) != -1)
    {
        switch (option_val)
        {
        case 'c':
            copt=1;
            strcpy(creg,optarg);
            break;
        case 't':
            topt=1;
            strcpy(treg,optarg);
            break;
        case 'f':
            fopt=1;
            strcpy(freg,optarg);
            break;
        default:
            fprintf(stderr, "Invalid arguments.\n");
            exit(EXIT_FAILURE);
        }
    }


    DIR *procdir;
    struct dirent *dir;
    char baseprocstr[] = "/proc/";
    procdir = opendir(baseprocstr);

    if (procdir)
    {
        while ((dir = readdir(procdir)) != NULL)
        {
            if (isNumber(dir->d_name))
            {

                
                show3info(atoi(dir->d_name));
            }
        }
        closedir(procdir);
    }
    for (int i = 0; i < ansarrnum; i++)ansarr[i]->show=1;

    //do -c
    if(copt){
        for (int i = 0; i < ansarrnum; i++)
        {
            if(!meetregex(ansarr[i]->command,creg)) ansarr[i]->show=0;
        }
        
    }


    //do -t
    if(topt){
        if(!strcmp(treg,"REG")){
            for (int i = 0; i < ansarrnum; i++)
                if(strcmp(ansarr[i]->type,"REG"))   ansarr[i]->show=0;
        }
        else if(!strcmp(treg,"CHR")){
            for (int i = 0; i < ansarrnum; i++)
                if(strcmp(ansarr[i]->type,"CHR"))   ansarr[i]->show=0;
        }
        else if(!strcmp(treg,"DIR")){
            for (int i = 0; i < ansarrnum; i++)
                if(strcmp(ansarr[i]->type,"DIR"))   ansarr[i]->show=0;            
        }
        else if(!strcmp(treg,"FIFO")){
            for (int i = 0; i < ansarrnum; i++)
                if(strcmp(ansarr[i]->type,"FIFO"))   ansarr[i]->show=0;                 
        }
        else if(!strcmp(treg,"SOCK")){
            for (int i = 0; i < ansarrnum; i++)
                if(strcmp(ansarr[i]->type,"SOCK"))   ansarr[i]->show=0;                 
        }
        else if(!strcmp(treg,"unknown")){
            for (int i = 0; i < ansarrnum; i++)
                if(strcmp(ansarr[i]->type,"unknown"))   ansarr[i]->show=0;                 
        }else{
            fprintf(stderr, "Invalid TYPE option.\n");
            exit(EXIT_FAILURE);
        }

    }


    //do -f
        if(fopt){
        for (int i = 0; i < ansarrnum; i++)
        {
            if(!meetregex(ansarr[i]->name,freg)) ansarr[i]->show=0;
        }
        
    }

    printans();


    // printf("hello this is start!\n");
    
    // char a [1024]="zsh";
    // char b [1024]="bash";
    // char c [1024]="/proc/fish";
    // char d [1024]="/proc/meemyfeehs";

    // char r[1024];
    // strcpy(r,"*");
    // printf("%s, %s, %s, %s, %s\n",a,b,c,d ,r);
    // printf("%d\n",meetregex(a,r));
    // printf("%d\n",meetregex(b,r));
    // printf("%d\n",meetregex(c,r));
    // printf("%d\n",meetregex(d,r));


}
int isNumber(char s[])
{
    for (int i = 0; s[i] != '\0'; ++i)
    {
        if (isdigit(s[i]) == 0)
            return 0;
    }
    return 1;
}

char *strremove(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}

void printrow1()
{

    fprintf(stdout,"%-12s %5s %10s %5s %9s %-12s %s\n",
            "COMMAND","PID","USER","FD","TYPE","NODE","NAME");
}


void printrown(struct fileinfo* info){
        if(info->show==0)return;
        if((!strcmp(info->type,"unknown"))||(!strcmp(info->fd,"NOFD"))){
            fprintf(stdout,"%-12s %5d %10s %5s %9s %12s %s\n",
                info->command,
                info->pid,
                info->user,
                info->fd,
                info->type,
                "",
                info->name
                );
        }else
            fprintf(stdout,"%-12s %5d %10s %5s %9s %-12ld %s\n",
                info->command,
                info->pid,
                info->user,
                info->fd,
                info->type,
                info->node,
                info->name
                );

}

void show3info(pid_t pid)
{

    struct pidinfo info;
    struct stat pidstat;
    struct passwd *pw;

    sprintf(info.path, "/proc/%d/", pid); // now /proc/8....
    info.pid = pid;
    info.pathlenstore = strlen(info.path);

    // retrieve the uid by stat and translate the uid using passwd file
    if (!stat(info.path, &pidstat))
    {
        pw = getpwuid(pidstat.st_uid);
        if (pw)
            strcpy(info.user, pw->pw_name);
        else
            sprintf(info.user, "%d", (int)pidstat.st_uid);
    }
    else
    {
        strcpy(info.user, "unknown");
    }

    strcat(info.path, "cmdline");
    int fd = open(info.path, O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr, "error at opening cmdline\n");
        return;
    }
    char cmdline[1024];
    int numread = read(fd, cmdline, sizeof(cmdline) - 1);
    if (numread < 0)
    {
        fprintf(stderr, "error at reading cmdline\n");
        return;
    }
    cmdline[numread] = '\0';

    char *token = strtok(cmdline, " ");
    strcpy(info.cmdline, basename(token));
    info.path[info.pathlenstore]='\0';

    ansarr[ansarrnum++]=handlespecialfiles("cwd", &info);
    ansarr[ansarrnum++]=handlespecialfiles("root", &info);
    ansarr[ansarrnum++]=handlespecialfiles("exe", &info);



    handlemapfiles(&info);
    handlefdfiles(&info);

    // fprintf(stdout, "Final print\t");
    // fprintf(stdout, "Pid:\t %d\n", info.pid);
    // fprintf(stdout, "User:\t %s\n", info.user);
    // fprintf(stdout, "Cmdline:\t %s\n", info.cmdline);
    // fprintf(stdout, "Path:\t %s\n", info.path);
    // fprintf(stdout, "pathlenstore:\t %ld\n", info.pathlenstore);
    // fprintf(stdout, "Final end\n");

}
void printans(){
    printrow1();
    for (int i = 0; i < ansarrnum; i++)
    {
        printrown(ansarr[i]);
    }
    
}

struct fileinfo* handlespecialfiles(char basename [], struct pidinfo *info){

    struct fileinfo *answer=malloc(sizeof(struct fileinfo));
    
    strcpy(answer->command,info->cmdline);
    answer->pid=info->pid;
    strcpy(answer->user,info->user);
    if(!strcmp(basename,"cwd")) strcpy(answer->fd,"cwd");
    else if(!strcmp(basename,"root")) strcpy(answer->fd,"rtd");
    else if(!strcmp(basename,"exe")) strcpy(answer->fd,"txt");
    else strcpy(answer->fd, "!ERROR!");

    strcat(info->path,basename);
    ssize_t linksize;
    struct stat tmpstat;

    char linkdest[1024];
    linksize=readlink(info->path,linkdest,1023);
    if(linksize>0) {
        linkdest[linksize]='\0';
        strcpy(answer->name,linkdest);
    }else{
        sprintf(answer->name,"%s %s",info->path,"(Permission denied)");
    }
    
    // printf("this is link %s -> %s, size: %ld\n",info->path,linkdest,linksize);
    
    
    if (!stat(info->path, &tmpstat))
    {
        if((tmpstat.st_mode & S_IFMT)==S_IFDIR)strcpy(answer->type,"DIR");
        else if((tmpstat.st_mode & S_IFMT)==S_IFREG) strcpy(answer->type,"REG");
        else if((tmpstat.st_mode & S_IFMT)==S_IFCHR) strcpy(answer->type,"CHR");
        else if((tmpstat.st_mode & S_IFMT)==S_IFIFO) strcpy(answer->type,"FIFO");
        else if((tmpstat.st_mode & S_IFMT)==S_IFSOCK) strcpy(answer->type,"SOCK");
        else goto UNKNOWNMODE;
    
        answer->node=tmpstat.st_ino;
    }
    else
    {
       UNKNOWNMODE:
       strcpy(answer->type,"unknown");

    }

    info->path[info->pathlenstore]='\0';
    return answer;
}

void handlemapfiles(struct pidinfo *info){

    strcat(info->path,"maps");
    FILE *fmaps;
    fmaps = fopen(info->path,"r");

    long inode=-1 ,lastinode =-3;
    char line [1024];
    char pathname [1024];

    if(!fmaps){
        struct fileinfo* answer=malloc(sizeof(struct fileinfo));
        strcpy(answer->command,info->cmdline);
        answer->pid=info->pid;
        strcpy(answer->user,info->user);
        strcpy(answer->fd,"NOFD");
        sprintf(answer->name,"%s (Permission denied)",info->path);
        ansarr[ansarrnum++]=answer;

        return;
    }else{
        //start scanning each map files
        
        while(fgets(line,sizeof(line)-1,fmaps)){
            int tmp=sscanf(line,"%*s %*s %*s %*s %ld %s",&inode,pathname);
            if(inode ==0 || (lastinode == inode)|| tmp!=2) continue;

            struct fileinfo* answer=malloc(sizeof(struct fileinfo));
            strcpy(answer->command,info->cmdline);
            answer->pid=info->pid;
            strcpy(answer->user,info->user);
            answer->node=inode;
            strcpy(answer->type,"REG");

            if(strstr(pathname,"deleted")){
                strcpy(answer->fd,"DEL");
                strcpy(answer->name,strremove(pathname,"deleted"));

            }else{
                strcpy(answer->fd,"mem");
                strcpy(answer->name,pathname);
            }

            ansarr[ansarrnum++]=answer;
            lastinode = inode;
        }

    }
    info->path[info->pathlenstore]='\0';
}

void handlefdfiles(struct pidinfo* info){
    info->path[info->pathlenstore]='\0';
    char* fd_path = "fd/";
    int tmpparentlenstore=info->pathlenstore;
    strcat(info->path,fd_path);
    info->pathlenstore+=strlen(fd_path);
    DIR *dir = opendir(info->path);
    

    if(!dir){
        struct fileinfo* answer=malloc(sizeof(struct fileinfo));
        strcpy(answer->command,info->cmdline);
        answer->pid=info->pid;
        strcpy(answer->user,info->user);
        strcpy(answer->fd,"NOFD");
        char tmp [1024];
        strcpy(tmp,info->path);
        tmp[strlen(tmp)-1]='\0';
        sprintf(answer->name,"%s (Permission denied)",tmp);
        ansarr[ansarrnum++]=answer;
        info->path[tmpparentlenstore]='\0';
        info->pathlenstore=tmpparentlenstore;

        return;

    }

    struct dirent *dirent;
    while ((dirent=readdir(dir)))
    {
        if(!strcmp(dirent->d_name,".")||!(strcmp(dirent->d_name,".."))) continue;
        char dirname [2048] ;
        snprintf(dirname,sizeof(dirname),"%s%s",info->path,dirent->d_name);
        struct stat tmpstat;
        int rtstat=stat(dirname,&tmpstat);
        if(rtstat!=0)continue;

        struct fileinfo* answer=malloc(sizeof(struct fileinfo));
        strcpy(answer->command,info->cmdline);
        answer->pid=info->pid;
        strcpy(answer->user,info->user);

        if((tmpstat.st_mode&S_IRUSR) && (tmpstat.st_mode&S_IWUSR))sprintf(answer->fd,"%s%s",dirent->d_name,"u");
        else if(tmpstat.st_mode&S_IRUSR) sprintf(answer->fd,"%s%s",dirent->d_name,"r");
        else if(tmpstat.st_mode&S_IWUSR) sprintf(answer->fd,"%s%s",dirent->d_name,"w");
        else {
          free(answer);
          continue;  
        }

        if((tmpstat.st_mode&S_IFMT)==S_IFSOCK)strcpy(answer->type,"SOCK");
        else if((tmpstat.st_mode&S_IFMT)==S_IFLNK)strcpy(answer->type,"LNK");
        else if((tmpstat.st_mode&S_IFMT)==S_IFDIR)strcpy(answer->type,"DIR");
        else if((tmpstat.st_mode&S_IFMT)==S_IFREG)strcpy(answer->type,"REG");
        else if((tmpstat.st_mode&S_IFMT)==S_IFCHR)strcpy(answer->type,"CHR");
        else if((tmpstat.st_mode&S_IFMT)==S_IFIFO)strcpy(answer->type,"FIFO");
        else strcpy(answer->type,"unknown");

        answer->node=tmpstat.st_ino;
        
        char linkdest[1024];
        int linksize=readlink(dirname,linkdest,1023);
        if(linksize>0) {
            linkdest[linksize]='\0';
            strcpy(answer->name,linkdest);
        }else{
            sprintf(answer->name,"%s %s",info->path,"(Permission denied)");
        }
    
        ansarr[ansarrnum++]=answer;

    }
    closedir(dir);
    info->path[tmpparentlenstore]='\0';
    info->pathlenstore=tmpparentlenstore;
}


int meetregex(char * target,char* pattern){
    regex_t preg;
    int success = regcomp(&preg,pattern,0);
    assert(!success);

    regmatch_t matchptr[1];
    const size_t nmatch=1;
    int status = regexec(&preg,target,nmatch,matchptr,0);
    regfree(&preg);
    return !(status == REG_NOMATCH);
}
