//  MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>			/* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>
//#include <libparser.so>

#define MAX_COMMANDS 8


// files in case of redirection
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
	printf("****  Exiting MSH **** \n");
	//signal(SIGINT, siginthandler);
	exit(0);
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
	//reset first
	for(int j = 0; j < 8; j++)
		argv_execvp[j] = NULL;

	int i = 0;
	for ( i = 0; argvv[num_command][i] != NULL; i++)
		argv_execvp[i] = argvv[num_command][i];
}


/**
 * Main sheell  Loop  
 */
int main(int argc, char* argv[])
{
	/**** Do not delete this code.****/
	int end = 0; 
	int executed_cmd_lines = -1;
	char *cmd_line = NULL;
	char *cmd_lines[10];

	if (!isatty(STDIN_FILENO)) {
		cmd_line = (char*)malloc(100);
		while (scanf(" %[^\n]", cmd_line) != EOF){
			if(strlen(cmd_line) <= 0) return 0;
			cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
			strcpy(cmd_lines[end], cmd_line);
			end++;
			fflush (stdin);
			fflush(stdout);
		}
	}

	/*********************************/

	char ***argvv = NULL;
	int num_commands;
	
	int accsum=0; // variable to store the acumulated sum

	while (1) 
	{
		int status = 0;
		int command_counter = 0;
		int in_background = 0;
		signal(SIGINT, siginthandler);

		// Prompt 
		write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

		// Get command
		//********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
		executed_cmd_lines++;
		if( end != 0 && executed_cmd_lines < end) {
			command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
		}
		else if( end != 0 && executed_cmd_lines == end) {
			return 0;
		}
		else {
			command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
		}
		//************************************************************************************************


		/************************ STUDENTS CODE ********************************/
	if (command_counter > 0) {
			if (command_counter > MAX_COMMANDS){
				printf("Error: Maximum number of commands is %d \n", MAX_COMMANDS);
			}
			else {
			// printf("The number of commands is %d\n",command_counter); // !!!!!!!!!
			
	
				// Print command
				// print_command(argvv, filev, in_background);
			}
		} // ****************nuestro programa*************
	else if(command_counter <0){
		perror("Error reading the commands");
	}else {
		if(command_counter==0){
		perror("Error reading the commands");
		}
		else{
			printf("EOF");
		}
	}
	

	
		for (int i =0; i< command_counter; i++){ // now that we want to execute the commands, we need to have them in a vector (argv_execpv) obtained in getCompleteCommand
		getCompleteCommand(argvv,i); 
		}

		if(strcmp(argvv[0][0], "mycalc") == 0) {
			
		// function my calc

			if (argvv[0][1]!=NULL && argvv[0][2]!=NULL && argvv[0][3]!=NULL){

				if (strcmp(argvv[0][2],"add")== 0){
				
				int add1 = atoi(argvv[0][1]);
				int add2 = atoi (argvv[0][3]);
				int result = add1 + add2;
				
                  		
				accsum +=  result;
                      		char array[20];
                      		sprintf(array, "%d", accsum);
                      		const char *value = array;

				if (setenv("Acc", value, 1) < 0) {
						perror("Error with the value of the environment variable\n");}

				char buffer[1024];
				snprintf(buffer,1024,"[OK] %d + %d = %d; Acc %s \n",add1,add2,result,getenv("Acc"));
				if((write(2, buffer,strlen(buffer))) < 0){
					perror ("Error while writing in standard error output");}
			
				

				}
				else if (strcmp(argvv[0][2],"mod")== 0){
				
				int num1 = atoi(argvv[0][1]);
				int num2 = atoi (argvv[0][3]);
				int result = num1 % num2;
				int quotient = abs(num1/num2);

				char buffer[1024];
				snprintf(buffer,1024,"[OK] %d %% %d = %d; Quotient %d \n",num1,num2,result,quotient);
				if((write(2, buffer,strlen(buffer))) < 0){
					perror ("Error while writing in standard error output");}
			
					
				

				}else{ 
				
				// control of syntaxis in case 3 arguments are included BUT do not include add or mod
				int length = strlen("[ERROR] The structure of the command is mycalc <operand_1> <add/mod> <operand_2>\n");
				if((write(2,"[ERROR] The structure of the command is mycalc <operand_1> <add/mod> <operand_2>\n",length)) < 0){
					perror ("Error while writing in standard error output");}
 	 				
				}
			
			

			}else{ 
			// control of syntaxis in case not 3 arguments are included after mycalc
			int length = strlen("[ERROR] The structure of the command is mycalc <operand_1> <add/mod> <operand_2>\n");
				if((write(2,"[ERROR] The structure of the command is mycalc <operand_1> <add/mod> <operand_2>\n",length)) < 0){
					perror ("Error while writing in standard error output");}
 			}
		
		// ********************************************** MYCP **************************************
		}else if(strcmp(argvv[0][0], "mycp") == 0){
			if (argvv[0][1]!=NULL && argvv[0][2]!=NULL) {
				int origfd; //file to be read and copy its information
				origfd = open(argvv[0][1], O_RDONLY, 0644);
				if (origfd >=0){
					int copyfd = open(argvv[0][2], O_WRONLY | O_CREAT | O_TRUNC, 0666); //file where the information is copied
					if (copyfd >=0){
					
						int nread,nwrite;
						char buffer[1024];
						
						nread=read(origfd, &buffer, 1024);
						while (nread > 0){
						 nwrite = write(copyfd, &buffer, nread);
						 if (nwrite < 0) {
                            			    if (close(copyfd) < 0) {
                              			perror("Error closing the destiny file\n");			
                            				perror("Error writing in the file\n");
                            				}
                            			}	
                       		        nread=read(origfd, &buffer,1024);
						
						if (nread < 0) {
                        				perror("Error reading the file\n");
                        				if (close(origfd) < 0) {
                          					perror("Error closing the initial file\n");}
						}
						} // end of while
						//Now we close both files
						if (close(copyfd) < 0) {
                              				perror("Error closing the destiny file\n");}				
						if (close(origfd) < 0) {
                          				perror("Error closing the initial file\n");}

						char buffer2[1024];
						snprintf(buffer2,1024,"[OK] Copy has been successful between %s and %s\n", argvv[0][1], argvv[0][2]);
						if((write(2, buffer2,strlen(buffer2))) < 0){
							perror ("Error while writing in standard error output");}

						
						

					}else{ // copyfd<0
					
					int length = strlen("[ERROR] Error opening the destination file");
					if((write(1, "[ERROR] Error opening the destination file",length)) < 0){
						perror ("Error while writing in standard error output");}
					}

				

					
				}else{		
				int length = strlen("[ERROR] Error opening the original file");
				if((write(1, "[ERROR] Error opening the original file",length)) < 0){
					perror ("Error while writing in standard error output");}
				}


			}else{
				int length = strlen("[ERROR] The structure of the command is mycp <original_file> <copied_file>");
				if((write(1, "[ERROR] The structure of the command is mycp <original_file> <copied_file>",length)) < 0){
					perror ("Error while writing in standar error output");}
			}

		

		}else if (command_counter ==1){ //*************************** Simple Commands ********************
			int pid = fork();
			int fd=0,status;
			if (pid==0){ // child process
			//Executing the command
// redireccionamientos de 0,1,2 input,output,error
			if (strcmp(filev[0], "0")!= 0){ // If there is an input file, close STDOUT input & open it
				if ((close(0))<0){
				perror("Error closing default descriptor 0");
				}
				if (fd = open(filev[0],O_RDONLY,0666)<0){
				perror("Error opening file");}
			}
			if (strcmp(filev[1], "0")!= 0){ // If there is an output file, close STDOUT & open file
				if ((close(1))<0){
				perror("Error closing default descriptor 1");
				}
				if (fd = open(filev[1],O_WRONLY|O_CREAT|O_TRUNC,0666)<0){
				perror("Error opening file");}
			}			
			if (strcmp(filev[2], "0")!= 0){ // If there is error file, close error and open file
				if ((close(2))<0){
				perror("Error closing default descriptor 2");
				}
				if (fd = open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666)<0){
				perror("Error opening file");}
			}	


				// Executing the simple command stored in argvv[0]
				if (execvp(argvv[0][0],argvv[0]) <0){
					perror("Error executing the command\n");
					exit(0);
				} 
					
			} else if (pid<0){
			perror("An error occured during the fork");
			}
			else{ //father
				if (fd!=0){// A file has been opened, then close the file			
				if((close(fd)) < 0){
																					 					perror("Error closing the descriptor");}
			
				}

				if(in_background !=1){ // back to the parent waiting
					while(pid !=wait (&status) );
					
				} else{ // back to the parent without waiting
					printf("Pid = [%d]\n",getpid());
				} 
			
			}
		  }else if(command_counter>1) {// ******************************MULTIPLE COMMANDS ******************
			int fdes=0;
                  	int fd[2];
                  	int pid, status;
			
			
			if (strcmp(filev[0], "0")!= 0){ // If there is an input file, close STDOUT input & open it
				if ((close(0))<0){
				perror("Error closing default descriptor 0");
				}
				if (fdes = open(filev[0],O_RDONLY,0666)<0){
				perror("Error opening file");}
			}
			if (strcmp(filev[1], "0")!= 0){ // If there is an output file, close STDOUT & open file
				if ((close(1))<0){
				perror("Error closing default descriptor 1");
				}
				if (fdes = open(filev[1],O_WRONLY|O_CREAT|O_TRUNC,0666)<0){
				perror("Error opening file");}
			}					
			if (strcmp(filev[2], "0")!= 0){ // If error file, close error and open error file
				if ((close(2))<0){
				perror("Error closing default descriptor 2");
				}
				if (fdes = open(filev[2],O_WRONLY|O_CREAT|O_TRUNC,0666)<0){
				perror("Error opening file");}
			}	

			int newinput = dup(STDIN_FILENO);

 			for (int i = 0; i < command_counter; i++) {
                    //We create as many pipes as commands but one
                    		if (i != command_counter - 1) {
                     			if (pipe(fd) < 0) {
                        		perror("Error en pipe\n");
                        		exit(0);
                      			}
                   		}
                   		

                    		/* Create the process to connect a sequence of commands */
                    		pid = fork();
                    		if (pid==0){ // child 
                    		
					//the standard input is now the one given by previous child
					if (dup2(newinput,STDIN_FILENO) < 0){;// we position newinput in place of the standard input
						perror("Error duplicating and placing the descriptor");	
					}

					if (close(newinput)<0){
						perror("Error closing the descriptor");
					}
					
					if (i!= command_counter-1){
					//enviar input 
						if (dup2(fd[1],STDOUT_FILENO)<0){ //we position fd[1] in place of the standard output
						perror("Error duplicating and placing the descriptor");
						}
						if (close(fd[1])<0){
							perror("Error closing the descriptor");
						}
						if (close(fd[0])<0){
							perror("Error closing the descriptor");
						}
					}

					if(execvp(argvv[i][0],argvv[i]) <0) {
						perror("Error executing the command");
						exit(0);
					} // we use execvp as we expect input arguments and the path for the program to
 					break;

				} else if(pid<0){
					perror("An error occured during the fork");
				} else{//father
					
					if (close(newinput)<0){
						perror("Error closing the descriptor");
					}

					if (i!= command_counter-1){
						//the father gives value to the standard input so next child can use it if it is not the last child

						if ((newinput = dup(fd[0])) < 0){;// if it is not the last process the father gives a value so that the child can use it in next command and this way we connect the sequence of commands
							perror("Error duplicating the descriptor");	
						}
						
						if (dup(fd[0])<0){
							perror("Error duplicating the descriptor");	
						}
						if (close(fd[1])<0){
							perror("Error closing the descriptor");
						}
					}
					
                  		
				}// close switch
			} // close for
			 	if (fdes!=0){// If a file has been opened, then close the file			
					if((close(fdes)) < 0){
																					 						perror("Error closing the file opened");}
			
				}

				if(in_background !=1){ // back to the parent waiting
					while(pid != wait (&status));
					
				}
				else{ // back to the parent without waiting
				printf("Pid = [%d]\n",getpid());} 
                    			
                 		 
                  
		  		}


	
	}// end of while(1)
	return 0;
}

