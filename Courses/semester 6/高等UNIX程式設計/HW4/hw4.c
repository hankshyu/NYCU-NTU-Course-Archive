#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <capstone/capstone.h>
#include <sys/user.h>
#include <ctype.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>



void errquit(const char *);
void printprompt();
void gdbhelp();
void gdbexit();
void gdblist();
void gdbvmmap(pid_t);
void gdbdump(const unsigned long long dump_addr,  unsigned char * const ptr);
void gdbdisasm(char *programstr,unsigned long long disasm_addr);
void disasm_instr(unsigned long long start_addr, unsigned long long limit_addr, unsigned char *CODE);
void disasm_single_instr(unsigned long long start_addr, unsigned char *CODE, char *output);
int readtextsection(const char *, unsigned long *, unsigned long *);
void wrongstateinput(char *wrong_input);


typedef struct bpoint{
    unsigned long long n_addr;
    unsigned char n_chr;
    char n_msg[1024];

}bpoint;

bpoint *bpoint_arr[256];
int bpoint_arr_size;

void init_bpoint_arr();
void insert_bpoint_arr(unsigned long long bp_addr,unsigned char bp_orig_char, const char* bp_msg);
void remove_bpoint_arr(int idx);
int seek_bpoint_arr(unsigned long long search_addr);
void print_bpont_arr_clean();


char errmsg [2048];
unsigned long sh_addr, sh_size;
FILE *file;
int main(int argc, char *argv[]){
	setvbuf(stdout,NULL,_IONBF,0);//turn off buffering...
	init_bpoint_arr();
	int option, sopt = 0, programopt = 0;
	char soptstr [1024], programstr [1024];	

	while((option = getopt(argc, argv, "s:"))!= -1){
		switch(option){
			case 's':
				sopt =1;
				strcpy(soptstr,optarg);
				break;
			default:
				break;	
		}
	}
	for(int i = optind; i < argc; i++){
		programopt =1;
		strcpy(programstr,argv[i]);
		break;
	}
	if(sopt){
		// dup2
		file = fopen(soptstr,"r");
		//fd = open(soptstr,O_RDONLY);
		//dup2(fd,STDIN_FILENO);

	}else{
		file = stdin;
	}

	// printf("======================================\n");
	// printf("Done processing arguemnts\n");
	// printf("%d || %s\n",sopt, soptstr);
	// printf("%d || %s\n",programopt, programstr);
	// printf("======================================\n");


	/* this is the NO_LOAD state
	
	accept commands:
	-> exit/q -> help/h -> list/l */

	if(!programopt){
		
		while(1){
			char no_load_full_input [2048];
			memset(no_load_full_input,0,sizeof(no_load_full_input));
			char no_load_input [1024];
			char *strtoktmp0;
			// printf("N_LOAD");
			
			if(file == stdin)printprompt();
			if(!fgets(no_load_full_input, sizeof(no_load_full_input)-1,file)){
				gdbexit();
			}
			
			no_load_full_input[strlen(no_load_full_input)-1] = 0;
			strtoktmp0=strtok(no_load_full_input," ");
			strcpy(no_load_input,strtoktmp0);

			//fscanf(file,"%s",no_load_input);
			if((!strcmp("help",no_load_input)) || (!strcmp("h",no_load_input))){
				gdbhelp();
				continue;
			}else if((!strcmp("exit",no_load_input)) || (!strcmp("q",no_load_input))){
				gdbexit();
			}else if((!strcmp("list",no_load_input)) || (!strcmp("l",no_load_input))){
				gdblist();
				continue;
			}else if(!strcmp("load",no_load_input)){
				strtoktmp0=strtok(NULL," ");

				//fscanf(file,"%s",no_load_input);
				strcpy(programstr,strtoktmp0);
				readtextsection(programstr, &sh_addr, &sh_size);
				// printf("addr: %lx || size: %lx\n",sh_addr,sh_size);
				printf("** program '%s' loaded. entry point 0x%llx\n",programstr,(unsigned long long)sh_addr);
				break;
			}else{
				wrongstateinput(no_load_input);
			}
		}
	}else{
		readtextsection(programstr, &sh_addr, &sh_size);
		// printf("addr: %lx || size: %lx\n",sh_addr,sh_size);
		printf("** program '%s' loaded. entry point 0x%llx\n",programstr,(unsigned long long)sh_addr);
	}
	
	/* this is the LOAD state (could not go back, programstr is already determined)
	
	accept commands:
	-> exit/q	-> help/h	-> list/l 
	-> run/r	->start	*/
	while(1){
		char load_full_input[2048];
		char load_input[1024];
		char *strtoktmp1;

		while(1){
			// printf("LOAD");

			if(file == stdin)printprompt();
			if(!fgets(load_full_input, sizeof(load_full_input),file)){
				gdbexit();
			}

			load_full_input[strlen(load_full_input)-1] = 0;
			strtoktmp1=strtok(load_full_input," ");
			strcpy(load_input,strtoktmp1);

			
			//fscanf(file,"%s",load_input);
			if((!strcmp("help",load_input)) || (!strcmp("h",load_input))){
				gdbhelp();
				continue;
			}else if((!strcmp("exit",load_input)) || (!strcmp("q",load_input))){
				gdbexit();
			}else if((!strcmp("list",load_input)) || (!strcmp("l",load_input))){
				gdblist();
				continue;
			}else if((!strcmp("run",load_input)) || (!strcmp("r",load_input))){
				break;
			}else if((!strcmp("start",load_input))){
				break;
			}else{
				wrongstateinput(load_input);
			}
		}

		/* this is the RUNNING state

		accept commands:
		-> exit/q	-> help/h	-> list/l 
		-> run/r (with reminder)
		->break/b	->cont/c	->delete	->disasm/d	->dump/x
		->get/g		->getregs	->vmmap/m	->set/s		->si	*/
		pid_t child;
		if((child = fork())<0) errquit("fork");
		if(child == 0){ 	/* Child */
			setvbuf(stdout,NULL,_IONBF,0);//turn off buffering...
			if(ptrace(PTRACE_TRACEME, 0, 0, 0)<0) errquit("ptrace");
			char *const argvar[]={programstr,NULL};//there will be not arguments
			// char environ [1024];
			// strcpy(environ,getenv("PATH"));
			// strcat(environ,":.");
			// setenv("PATH",environ,1);
			execvp(programstr,argvar);
			errquit("execvp");
		}else{ 				/* Parent process */

		int status;
		struct user_regs_struct regs;
		if(waitpid(child, &status, 0)<0) errquit("waitpid");
		assert(WIFSTOPPED(status));
		ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);

		// we have to put all the break points back to the program
		for (int i = 0; i < bpoint_arr_size; i++){
			unsigned long long break_addr = bpoint_arr[i]->n_addr;
			long ret;
			unsigned char *ptr = (unsigned char *) &ret;
			ret = ptrace(PTRACE_PEEKTEXT, child, break_addr, 0);

			ptr[0]=0xcc;
			unsigned long *lcode = (unsigned long*)ptr;
			ptrace(PTRACE_POKETEXT, child, break_addr, *lcode);
		}

		if((!strcmp("run",load_input)) || (!strcmp("r",load_input))){
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("getregs1@run0");
				unsigned long long oldaddr = regs.rip;
				int bpoint_addr = seek_bpoint_arr(oldaddr);
				if(bpoint_addr != -1){
					long ret;
					unsigned char *ptr = (unsigned char *) &ret;
					ret = ptrace(PTRACE_PEEKTEXT, child, oldaddr,0);
					ptrace(PTRACE_POKETEXT, child, oldaddr, (ret & 0xffffffffffffff00 | bpoint_arr[bpoint_addr]->n_chr));
					ptrace(PTRACE_SINGLESTEP, child, 0, 0);
					if(waitpid(child, &status, 0)<0) errquit("waitpid");
					ptrace(PTRACE_POKETEXT, child, oldaddr, ret);
					
				}
				
				ptrace(PTRACE_CONT, child, 0, 0);
				if(waitpid(child, &status, 0)<0) errquit("waitpid");
				if (WIFEXITED(status)){

					const int es = WEXITSTATUS(status);
					if(!es)
						printf("** child process %d terminated normally (code %d)\n",child,es);
					else 
						printf("** child process %d terminated with exit code %d\n",child ,es);
					
					break;

				}
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("getregs2@cont");
				int next_bpoint_addr = seek_bpoint_arr(regs.rip-1);
				if(next_bpoint_addr != -1 ){
					printf("** breakpoint @\t%s\n",bpoint_arr[next_bpoint_addr]->n_msg);
				}
				regs.rip--;
				ptrace(PTRACE_SETREGS, child, 0, &regs);
		}else if((!strcmp("start",load_input))){
			printf("** pid %d\n",child);
		}

		char run_full_input[2048];
		char run_input[1024];
		char *strtoktmp;

		while(WIFSTOPPED(status)){


			//fscanf(stdin,"%s",run_input);
			if(file == stdin)printprompt();

			if(!fgets(run_full_input, sizeof(run_full_input)-1,file)){
				// printf("detected fgets null!\n");
				gdbexit();
			}


			run_full_input[strlen(run_full_input)-1] = 0;

			strtoktmp=strtok(run_full_input," ");
			strcpy(run_input,strtoktmp);

			

			if((!strcmp("help",run_input)) || (!strcmp("h",run_input))){
				gdbhelp();
				
			}else if((!strcmp("exit",run_input)) || (!strcmp("q",run_input))){
				gdbexit();

			}else if((!strcmp("list",run_input)) || (!strcmp("l",run_input))){
				gdblist();
				
			}else if((!strcmp("vmmap",run_input)) || (!strcmp("m",run_input))){
				gdbvmmap(child);

			}else if((!strcmp("getregs",run_input))){
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("ptrace@getregs");
	
				printf("RAX %llx\tRBX %llx\tRCX %llx\tRDX %llx\n",regs.rax,regs.rbx,regs.rcx,regs.rdx);
				printf("R8 %llx\tR9 %llx\tR10 %llx\tR11 %llx\n",regs.r8,regs.r9,regs.r10,regs.r11);
				printf("R12 %llx\tR13 %llx\tR14 %llx\tR15 %llx\n",regs.r12,regs.r13,regs.r14,regs.r15);
				printf("RDI %llx\tRSI %llx\tRBP %llx\tRSP %llx\n",regs.rdi,regs.rsi,regs.rbp,regs.rsp);
				printf("RIP %llx\tFLAGS %016llx\n",regs.rip,regs.eflags);
	
			}else if((!strcmp("get",run_input)) || (!strcmp("g",run_input))){

				char run_regs[1024];
				strtoktmp=strtok(NULL," ");
				strcpy(run_regs,strtoktmp);
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("ptrace@getregs");
				if(!strcmp("rax", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.rax,regs.rax);
				else if(!strcmp("rbx", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.rbx,regs.rbx);
				else if(!strcmp("rcx", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.rcx,regs.rcx);
				else if(!strcmp("rdx", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.rdx,regs.rdx);
				else if(!strcmp("r8", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.r8,regs.r8);
				else if(!strcmp("r9", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.r9,regs.r9);
				else if(!strcmp("r10", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.r10,regs.r10);
				else if(!strcmp("r11", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.r11,regs.r11);
				else if(!strcmp("r12", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.r12,regs.r12);
				else if(!strcmp("r13", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.r13,regs.r13);
				else if(!strcmp("r14", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.r14,regs.r14);
				else if(!strcmp("r15", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.r15,regs.r15);
				else if(!strcmp("rdi", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.rdi,regs.rdi);
				else if(!strcmp("rsi", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.rsi,regs.rsi);
				else if(!strcmp("rbp", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.rbp,regs.rbp);
				else if(!strcmp("rsp", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.rsp,regs.rsp);
				else if(!strcmp("rip", run_regs)) printf("%s = %llu (0x%llx)\n",run_regs,regs.rip,regs.rip);	
				else if(!strcmp("eflags", run_regs)) printf("%s = %llu (0x%016llx)\n",run_regs,regs.eflags,regs.eflags);
				else{
					sprintf(errmsg,"** RUNNING state@get incorrect register '%s'\n",run_regs);
					errquit(errmsg);
				}
				
			}else if((!strcmp("set",run_input)) || (!strcmp("s",run_input))){
				char run_regs[1024];
				unsigned long long value_old; 
				unsigned long long value_target; 
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("ptrace@getregs");

				strtoktmp=strtok(NULL," ");
				strcpy(run_regs,strtoktmp);
				strtoktmp=strtok(NULL," ");
				value_target=strtoul(strtoktmp,0,0);

				if(!strcmp("rax", run_regs))		value_old=regs.rax, regs.rax=value_target;
				else if(!strcmp("rbx", run_regs)) 	value_old=regs.rbx, regs.rbx=value_target;
				else if(!strcmp("rcx", run_regs)) 	value_old=regs.rcx, regs.rcx=value_target;
				else if(!strcmp("rdx", run_regs)) 	value_old=regs.rdx, regs.rdx=value_target;
				else if(!strcmp("r8", run_regs)) 	value_old=regs.r8 , regs.r8 =value_target;
				else if(!strcmp("r9", run_regs)) 	value_old=regs.r9 , regs.r9 =value_target;
				else if(!strcmp("r10", run_regs)) 	value_old=regs.r10, regs.r10=value_target;
				else if(!strcmp("r11", run_regs)) 	value_old=regs.r11, regs.r11=value_target;
				else if(!strcmp("r12", run_regs)) 	value_old=regs.r12, regs.r12=value_target;
				else if(!strcmp("r13", run_regs)) 	value_old=regs.r13, regs.r13=value_target;
				else if(!strcmp("r14", run_regs)) 	value_old=regs.r14, regs.r14=value_target;
				else if(!strcmp("r15", run_regs)) 	value_old=regs.r15, regs.r15=value_target;
				else if(!strcmp("rdi", run_regs)) 	value_old=regs.rdi, regs.rdi=value_target;
				else if(!strcmp("rsi", run_regs)) 	value_old=regs.rsi, regs.rsi=value_target;
				else if(!strcmp("rbp", run_regs)) 	value_old=regs.rbp, regs.rbp=value_target;
				else if(!strcmp("rsp", run_regs)) 	value_old=regs.rsp, regs.rsp=value_target;
				else if(!strcmp("rip", run_regs)) 	value_old=regs.rip, regs.rip=value_target;
				else if(!strcmp("eflags", run_regs))value_old=regs.eflags, regs.eflags=value_target;
				else{
					sprintf(errmsg,"** RUNNING state@set incorrect register '%s'\n",run_regs);
					errquit(errmsg);
				}

				//printf("** %s = %llu (0x%llx) ==> %llu (0x%llx)\n",run_regs,value_old,value_old,value_target,value_target);
				ptrace(PTRACE_SETREGS, child, 0, &regs);


			}else if((!strcmp("dump",run_input)) || (!strcmp("x",run_input))){
				
				unsigned long long dump_addr;
				strtoktmp=strtok(NULL," ");
				if(!strtoktmp){
					printf("** no addr is given.\n");
					continue;
				}
				dump_addr = strtoul(strtoktmp,0,0);
				
				long ret, ret2;
				unsigned char *ptr = (unsigned char *) &ret;
				
				for(int i=0; i<5; ++i){

				ret = ptrace(PTRACE_PEEKTEXT, child, dump_addr, 0);
				ret2 = ptrace(PTRACE_PEEKTEXT, child, dump_addr+8, 0);
				memcpy(ptr+8, (unsigned char *)&ret2, 8);
				gdbdump(dump_addr,ptr);
				dump_addr+=16;
				}

			
			}else if((!strcmp("disasm",run_input)) || (!strcmp("d",run_input))){
				unsigned long long disasm_addr;
				strtoktmp=strtok(NULL," ");
				if(!strtoktmp){
					printf("** no addr is given.\n");
					continue;
				}

				disasm_addr = strtoul(strtoktmp,0,0);
				gdbdisasm(programstr,disasm_addr);

			}else if((!strcmp("break",run_input)) || (!strcmp("b",run_input))){
				unsigned long long break_addr;
				unsigned char break_char;
				char break_msg[1024];

				strtoktmp=strtok(NULL," ");
				if(!strtoktmp){
					printf("** no addr is given.\n");
					continue;
				}
				break_addr = strtoul(strtoktmp,0,0);
				if(break_addr < sh_addr || break_addr >= (sh_addr+sh_size)){
					printf("** the breaking point address is out of the range of the text segment\n");
					continue;
				}
				if(seek_bpoint_arr(break_addr) != -1){
					printf("** the breaking point already exist!\n");
					continue;
				}

				long ret, ret2;
				unsigned char *ptr = (unsigned char *) &ret;
				ret = ptrace(PTRACE_PEEKTEXT, child, break_addr, 0);
				ret2 = ptrace(PTRACE_PEEKTEXT, child, break_addr+8, 0);
				memcpy(ptr+8, (unsigned char *)&ret2, 8);

				disasm_single_instr(break_addr, ptr, break_msg);
				insert_bpoint_arr(break_addr, *ptr, break_msg);

				ptr[0]=0xcc;//insert the breaking point
				unsigned long *lcode = (unsigned long*)ptr;
				ptrace(PTRACE_POKETEXT, child, break_addr, *lcode);

			}else if((!strcmp("delete",run_input))){
				int deleteidx;
	

				strtoktmp=strtok(NULL," ");
				if(!strtoktmp){
					printf("** no index is given.\n");
					continue;
				}
				
				deleteidx = atoi(strtoktmp);

				if(deleteidx >= bpoint_arr_size){
					printf("** The index has no corresponding break point!\n");
					continue;
				}
				
				long ret, ret2;
				unsigned char *ptr = (unsigned char *) &ret;
				ret = ptrace(PTRACE_PEEKTEXT, child, bpoint_arr[deleteidx]->n_addr, 0);

				ptr[0]=bpoint_arr[deleteidx]->n_chr;//insert the breaking point
				unsigned long *lcode = (unsigned long*)ptr;
				ptrace(PTRACE_POKETEXT, child, bpoint_arr[deleteidx]->n_addr, *lcode);

				remove_bpoint_arr(deleteidx);
				//remove breaking points
			}else if((!strcmp("run",run_input)) || (!strcmp("r",run_input))){
				printf("** program %s is already running\n",programstr);
				//the below is copied directly from "continue"
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("getregs1@run");
				unsigned long long oldaddr = regs.rip;
				int bpoint_addr = seek_bpoint_arr(oldaddr);
				if(bpoint_addr != -1){
					long ret;
					unsigned char *ptr = (unsigned char *) &ret;
					ret = ptrace(PTRACE_PEEKTEXT, child, oldaddr,0);
					ptrace(PTRACE_POKETEXT, child, oldaddr, (ret & 0xffffffffffffff00 | bpoint_arr[bpoint_addr]->n_chr));
					ptrace(PTRACE_SINGLESTEP, child, 0, 0);
					if(waitpid(child, &status, 0)<0) errquit("waitpid");
					ptrace(PTRACE_POKETEXT, child, oldaddr, ret);
					
				}
				
				ptrace(PTRACE_CONT, child, 0, 0);
				if(waitpid(child, &status, 0)<0) errquit("waitpid");
				if (WIFEXITED(status)){
					// printf("The program has exited!!@ cont\n");
					break;
				}
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("getregs2@cont");
				int next_bpoint_addr = seek_bpoint_arr(regs.rip-1);
				if(next_bpoint_addr != -1 ){
					printf("** breakpoint @\t%s\n",bpoint_arr[next_bpoint_addr]->n_msg);
				}
				regs.rip--;
				ptrace(PTRACE_SETREGS, child, 0, &regs);

			}else if((!strcmp("si",run_input))){
				
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("getregs1@si");
				unsigned long long oldaddr = regs.rip;
				int bpoint_addr = seek_bpoint_arr(oldaddr);
				if(bpoint_addr != -1){
					long ret;
					unsigned char *ptr = (unsigned char *) &ret;
					ret = ptrace(PTRACE_PEEKTEXT, child, oldaddr,0);
					ptrace(PTRACE_POKETEXT, child, oldaddr, (ret & 0xffffffffffffff00 | bpoint_arr[bpoint_addr]->n_chr));
					ptrace(PTRACE_SINGLESTEP, child, 0, 0);
					if(waitpid(child, &status, 0)<0) errquit("waitpid");
					ptrace(PTRACE_POKETEXT, child, oldaddr, ret);
					
				}else{
					ptrace(PTRACE_SINGLESTEP, child, 0, 0);
					if(waitpid(child, &status, 0)<0) errquit("waitpid");

				}
				if (WIFEXITED(status)){
					printf("The program has exited!!@ si\n");
					break;
				}
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("getregs2@si");
				int next_bpoint_addr = seek_bpoint_arr(regs.rip);
				if(next_bpoint_addr != -1 ){
					printf("** breakpoint @\t%s\n",bpoint_arr[next_bpoint_addr]->n_msg);
				}

			}else if((!strcmp("cont",run_input)) || (!strcmp("c",run_input))){
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("getregs1@cont");
				unsigned long long oldaddr = regs.rip;
				int bpoint_addr = seek_bpoint_arr(oldaddr);
				if(bpoint_addr != -1){
					long ret;
					unsigned char *ptr = (unsigned char *) &ret;
					ret = ptrace(PTRACE_PEEKTEXT, child, oldaddr,0);
					ptrace(PTRACE_POKETEXT, child, oldaddr, (ret & 0xffffffffffffff00 | bpoint_arr[bpoint_addr]->n_chr));
					ptrace(PTRACE_SINGLESTEP, child, 0, 0);
					if(waitpid(child, &status, 0)<0) errquit("waitpid");
					ptrace(PTRACE_POKETEXT, child, oldaddr, ret);
					
				}
				
				ptrace(PTRACE_CONT, child, 0, 0);
				if(waitpid(child, &status, 0)<0) errquit("waitpid");
				if (WIFEXITED(status)){
					// printf("The program has exited!!@ cont\n");
					break;
				}
				if(ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("getregs2@cont");
				int next_bpoint_addr = seek_bpoint_arr(regs.rip-1);
				if(next_bpoint_addr != -1 ){
					printf("** breakpoint @\t%s\n",bpoint_arr[next_bpoint_addr]->n_msg);
				}
				regs.rip--;
				ptrace(PTRACE_SETREGS, child, 0, &regs);

			}else{
				wrongstateinput(run_input);
			}

		}
		
		if (WIFEXITED(status)){
			const int es = WEXITSTATUS(status);
			if(!es)
				printf("** child process %d terminated normally (code %d)\n",child,es);
				
			else 
				printf("** child process %d terminated with exit code %d\n",child ,es);
		}
	}
	}
	return 0;	
}

void errquit(const char *msg){
	fprintf(stderr,"%s",msg);
	exit(-1);
}

void printprompt(){
	printf("sdb> ");
}

void gdbhelp(){

	fprintf(stdout,"- break {instruction-address}: add a break point\n");
	fprintf(stdout,"- cont: continue execution\n");
	fprintf(stdout,"- delete {break-point-id}: remove a break point\n");
	fprintf(stdout,"- disasm addr: disassemble instructions in a file or a memory region\n");
	fprintf(stdout,"- dump addr: dump memory content\n");
	fprintf(stdout,"- exit: terminate the debugger\n");
	fprintf(stdout,"- get reg: get a single value from a register\n");
	fprintf(stdout,"- getregs: show registers\n");
	fprintf(stdout,"- help: show this message\n");
	fprintf(stdout,"- list: list break points\n");
	fprintf(stdout,"- load {path/to/a/program}: load a program\n");
	fprintf(stdout,"- run: run the program\n");
	fprintf(stdout,"- vmmap: show memory layout\n");
	fprintf(stdout,"- set reg val: get a single value to a register\n");
	fprintf(stdout,"- si: step into instruction\n");
	fprintf(stdout,"- start: start the program and stop at the first instruction\n");
}

void gdbexit(){
	exit(0);
}

void gdblist(){
	for (int i = 0; i < bpoint_arr_size; i++)
        printf("%d: %llx\n",i,bpoint_arr[i]->n_addr);
}

void gdbvmmap(pid_t pid){
	char pidmapath [1024];
	sprintf(pidmapath,"/proc/%d/maps",pid);
	
	FILE *input_file = fopen(pidmapath, "r");
	if(!input_file) perror("fopen");

	char contents [1024];
	unsigned long long vm_start, vm_end;
	char vm_flags [1024];
	unsigned long long vm_pgoff;
	char d_inode [1024];
	char i_ino [1024];
	char vm_filename [1024];

    while (fgets(contents,sizeof(contents),input_file) != NULL){
		/*
		contents[strlen(contents)-1]=0;
		printf("=>%s",contents);
		*/
		sscanf(contents,"%llx-%llx %s %llu %s %s %s", &vm_start, &vm_end, vm_flags, &vm_pgoff, d_inode, i_ino, vm_filename);
		printf("%016llx-%016llx %c%c%c %llu %s\n", vm_start, vm_end, vm_flags[0], vm_flags[1], vm_flags[2], vm_pgoff, vm_filename);
    }

    fclose(input_file);

}

void gdbdump(const unsigned long long dump_addr, unsigned char *const ptr){
	char c;
	printf("\t%llx:",dump_addr);
	for (int i = 0; i < 16; i++)
	{
		printf(" %2.2x",ptr[i]);
	}
	
	printf("\t|");
	for (int i = 0; i < 16; i++)
	{
		c = (isprint(ptr[i]))? ptr[i]: '.';
		printf("%c",c);
	}
	printf("|\n");
}

void gdbdisasm(char *programstr,unsigned long long disasm_addr){
	pid_t child2;
	if((child2 = fork())<0) errquit("disasm @fork");
	if(child2 == 0){
		if(ptrace(PTRACE_TRACEME, 0, 0, 0)<0) errquit("ptrace");
		char *const argvar[]={programstr,NULL};//there will be not arguments
		char environ [1024];
		strcpy(environ,getenv("PATH"));
		strcat(environ,":.");
		setenv("PATH",environ,1);
		execvp(programstr,argvar);
		errquit("execvp");
	}else{
		int status;
		if(waitpid(child2, &status, 0) < 0) errquit("disasm @waitpid");
		assert(WIFSTOPPED(status));
		ptrace(PTRACE_SETOPTIONS, child2, 0, PTRACE_O_EXITKILL);

		
		long ret, ret2;
		unsigned char *ptr = malloc(200*sizeof(unsigned char));
		memset(ptr,'\0',200);

		ret = ptrace(PTRACE_PEEKTEXT, child2, disasm_addr, 0);

		for (int i = 0; i < 18; i++){
			ret = ptrace(PTRACE_PEEKTEXT, child2, disasm_addr+(i*8), 0);
			memcpy(ptr+(i*8), (unsigned char *)&ret, 8);
		}

		disasm_instr(disasm_addr, (unsigned long long)(sh_addr+sh_size), ptr);
		free(ptr);

	}
}

void disasm_single_instr(unsigned long long start_addr, unsigned char *CODE, char *output){
	csh handle;
	cs_insn *insn;
	size_t count;

	if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
		return;
	count = cs_disasm(handle, CODE, 150, start_addr, 0, &insn);

	if (count > 0) {

		for (size_t j = 0; j < 1; j++) {

			char *print_code= malloc(64*sizeof(char));
			memset(print_code,'\0',64);
			
			for (int i = 0; i < insn[j].size; i++){
				char code_buff[4];
				sprintf(code_buff," %02hhx",insn[j].bytes[i]);
				strcat(print_code,code_buff);
			}			
			sprintf(output,"\t%llx:%-45s\t%-10s%s",(unsigned long long)insn[j].address,print_code,insn[j].mnemonic,insn[j].op_str);
			free(print_code);

		}

		cs_free(insn, count);
	} else
		printf("ERROR: Failed to disassemble given code!\n");

	cs_close(&handle);
}

void disasm_instr(unsigned long long start_addr, unsigned long long limit_addr, unsigned char *CODE){
	
	csh handle;
	cs_insn *insn;
	size_t count;

	if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
		return;
	count = cs_disasm(handle, CODE, 150, start_addr, 0, &insn);

	if (count > 0) {

		for (size_t j = 0; j < count; j++) {
			if(j >= 10 || (unsigned long long)insn[j].address >= limit_addr){
				cs_free(insn, count);
				if(j<10) printf("** the address is out of the range of the text segment\n");
				return;
			}

			char *print_code= malloc(64*sizeof(char));
			memset(print_code,'\0',64);
			
			for (int i = 0; i < insn[j].size; i++){
				char code_buff[4];
				sprintf(code_buff," %02hhx",insn[j].bytes[i]);
				strcat(print_code,code_buff);
			}			
			printf("\t%llx:%-45s\t%-10s%s\n",(unsigned long long)insn[j].address,print_code,insn[j].mnemonic,insn[j].op_str);
			free(print_code);

		}

		cs_free(insn, count);
	} else
		printf("ERROR: Failed to disassemble given code!\n");

	cs_close(&handle);

}

int readtextsection(const char *filename, unsigned long *ans_addr, unsigned long *ans_size){

    int fd;
    int val;

    Elf64_Ehdr elfHdr;
    Elf64_Shdr sectHdr;
    FILE* ElfFile = NULL;
    char* SectNames = NULL;

    ElfFile = fopen(filename, "r");
    if(ElfFile == NULL) {
        printf("fopen");
        return -1;
    }

    //preberemo elf header
    fread(&elfHdr, 1, sizeof(elfHdr), ElfFile);

    //premik do section tabele
    fseek(ElfFile, elfHdr.e_shoff + elfHdr.e_shstrndx * elfHdr.e_shentsize, SEEK_SET);
    fread(&sectHdr, 1, sizeof(sectHdr), ElfFile);
    SectNames = malloc(sectHdr.sh_size);
    fseek(ElfFile, sectHdr.sh_offset, SEEK_SET);
    fread(SectNames, 1, sectHdr.sh_size, ElfFile);

    for (int idx = 0; idx < elfHdr.e_shnum; idx++){
        char* name = "";
        fseek(ElfFile, elfHdr.e_shoff + idx * sizeof(sectHdr), SEEK_SET);
        fread(&sectHdr, 1, sizeof(sectHdr), ElfFile);
        // print section name
        if (sectHdr.sh_name) name = SectNames + sectHdr.sh_name;
        if(!strcmp(name,".text")){
            *ans_addr = sectHdr.sh_addr;
            *ans_size = sectHdr.sh_size;
            break;
        }
    }

    fclose(ElfFile);    
    return 0;
}

void init_bpoint_arr(){
    for (int i = 0; i < 256; i++)
    {
        bpoint_arr[i]=NULL;
    }
    
}

void insert_bpoint_arr(unsigned long long bp_addr,unsigned char bp_orig_char, const char* bp_msg){
    bpoint *newnode = malloc(sizeof(bpoint));
    newnode->n_addr = bp_addr;
    newnode->n_chr = bp_orig_char;
    strcpy(newnode->n_msg,bp_msg);
    bpoint_arr[bpoint_arr_size++]=newnode;

}

void remove_bpoint_arr(int idx){
    if(idx >= bpoint_arr_size || bpoint_arr_size == 0) return;
    
    for (int i = idx+1; i < bpoint_arr_size; i++)
    {
        bpoint_arr[i-1]=bpoint_arr[i];
    }

    bpoint_arr[bpoint_arr_size-1]=NULL;
    bpoint_arr_size--;
    

}

int seek_bpoint_arr(unsigned long long search_addr){
    for (int i = 0; i < bpoint_arr_size; i++)
    {
        if(bpoint_arr[i]->n_addr == search_addr) return i;
    }
    return -1;
    
}

void print_bpont_arr(){
    for (int i = 0; i < 10; i++)
    {
        if(bpoint_arr[i]==NULL) printf("this is emtpy!\n");
        else printf("%d :addr->%llx, original: %x, %s",i,bpoint_arr[i]->n_addr,bpoint_arr[i]->n_chr,bpoint_arr[i]->n_msg);
    }
    printf("done print tree!\n\n");
    
}

void print_bpont_arr_clean(){
    
    for (int i = 0; i < bpoint_arr_size; i++)
        printf("%d :addr->%llx, original: %x, %s\n",i,bpoint_arr[i]->n_addr,bpoint_arr[i]->n_chr,bpoint_arr[i]->n_msg);
    
}

void wrongstateinput(char *wrong_input){

	if((!strcmp("break",wrong_input)) || (!strcmp("b",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("cont",wrong_input)) || (!strcmp("c",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("delete",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("disasm",wrong_input)) || (!strcmp("d",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("dump",wrong_input)) || (!strcmp("x",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("get",wrong_input)) || (!strcmp("g",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("getregs",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("load",wrong_input))){
		printf("** state must be NOT LOADED\n");
	}else if((!strcmp("run",wrong_input)) || (!strcmp("r",wrong_input))){
		printf("** state must be LOADED or RUNNING\n");
	}else if((!strcmp("vmmap",wrong_input)) || (!strcmp("m",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("set",wrong_input)) || (!strcmp("s",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("si",wrong_input))){
		printf("** state must be RUNNING\n");
	}else if((!strcmp("start",wrong_input))){
		printf("** state must be LOADED\n");
	}else{
		printf("** The command '%s' is not currently supported...\n",wrong_input);
	}
}






