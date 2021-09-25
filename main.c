
//This program is written by PRADEEP KUMAR (IIT G)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUILTIN_COMMANDS 7    // Number of builtin commands defined


char PWD[1024];         // Present Working Directory
char PATH[1024];        // Path to find the commands
char past_com[10][80];  // Array to store history
int  argCount = 0;      // No. of commands already entered

/*
 * Built-in command names
 * NOTE: cd, echo, printenv are available in execvp()
 */
char * builtin[] = {"cd", "exit", "help", "echo", "history", "setenvir", "printenv"};




/*
 * Built-in command functions
 */

/*
 * Function:  shell_cd
 * -------------------
 *  changes current working directory
 *
 * args: arguments to the cd command, will consider only the first argument after the command name
 */

int shell_cd(char ** args){
    if (args[1] == NULL){
        fprintf(stderr, "minsh: one argument required\n");
    }
    else if (chdir(args[1]) < 0){
        perror("minsh");
    }
    getcwd(PWD, sizeof(PWD));    // Update present working directory
    return 1;
}

/*
 Function: set environment variable
 */
int shell_setenvir(char * args[]){
            // Case 'setenv': we set an environment variable to a value
            if((args[1] == NULL) && args[2] == NULL){
                printf("%s","Not enought input arguments\n");
                return -1;
            }
    
            if (args[2] == NULL){
                setenv(args[1], "", 1);
                
                
            // We set the variable to the given value
            }else{
                setenv(args[1], args[2], 1);
            }
    return 1;
}

/*
 Function: get environment variable
 */
int shell_printenv(char **args){
    printf("%s\n", getenv(args[1]));
    return 1;
}
 
/*
 * Function:  shell_exit
 *  exits from the shell
 * return: status 0 to indicate termination
 */
int shell_exit(char ** args){
    return 0;
}

/*
 * Function:  shell_help
 * prints a small description
 * return: status 1 to indicate successful termination
 */
int shell_help(char ** args){
    printf("\nWelcome to my mini shell");
    printf("\nHere are the inbuilt commands:");
    printf("\n\t- help");
   
    printf("\n\t- cd dir");
    printf("\n\t- history");
    printf("\n\t- echo [string to echo]");
    printf("\n\t- setenvir [evironment var name] [environment value]");
    printf("\n\t- printenv [evironment var name]");
    printf("\n\t- exit");
    printf("\n\n Here are external functions (executed by execvp()):");
    printf("\n\t- clear");
    printf("\n\t- ls [-ail] [dir1 dir2 ...]");
    printf("\n\t- cp source target (or) cp file1 [file2 ...] dir");
    printf("\n\t- mv source target (or) mv file1 [file2 ...] dir");
    printf("\n\t- rm file1 [file2 ...]");
    printf("\n\t- mkdir dir1 [dir2 ...]");
    printf("\n\t- rmdir dir1 [dir2 ...]");
    printf("\n\t- ln [-s] source target");
    printf("\n\t- cat [file1 file2 ...]");
    printf("\n\t-  and many more.......");
    printf("\n\n");
    printf("Other features : ");
    printf("\n\t* Input, Output and append Redirection (<, >, >> respectively)  : ");
    printf("\n\t* Single level piping can also be performed");
    printf("\n\n");
    return 1;
}



/*
 * Function:  shell_echo
 * ---------------------
 *  displays the string provided

 * return: status 1 to indicate successful termination
 */
int shell_echo(char ** args){
    int i = 1;
    while (args[i]!=NULL){
        // End of arguments
    
        printf("%s ", args[i]);
        i++;
    }
    printf("\n");
    return 4;
}

int shell_history() {
    
    if(argCount == 0)
        printf("No command has been entered so far.\n");
    
    for (int i = 1; i<argCount; i++) {
       printf("%d %s\n",argCount - i, past_com[i]);
    }

    return 1;
}




/*
 * Array of function pointers to built-in command functions
 */
int (* builtin_function[]) (char **) = {
    &shell_cd,
    &shell_exit,
    &shell_help,
    &shell_echo,
    &shell_history,
    &shell_setenvir,
    &shell_printenv
};


/*
 * Function:  split_command_line
 * -----------------------------
 *  splits a commandline into tokens using strtok()
 *
 * command: a line of command read from terminal
 *
 * returns: an array of pointers to individual tokens
 */

//one * means it is an array of characters, double star means it is an arrray of arrays
char ** split_command_line(char * command){
        int position = 0;
        int no_of_tokens = 64;
        char ** tokens = malloc(sizeof(char *) * no_of_tokens);
        char delim[2] = " ";

        // Split the command line into tokens with space as delimiter
        char * token = strtok(command, delim);
        while (token != NULL){
                tokens[position] = token;
                position++;
                token = strtok(NULL, delim);
        }
        tokens[position] = NULL;
        return tokens;
}

/*
 * Function:  read_command_line
 * ----------------------------
 *  reads a commandline from terminal
 *
 * returns: a line of command read from terminal
 */
char * read_command_line(void){
        int position = 0;
        int buf_size = 1024;
        char * command = (char *)malloc(sizeof(char) * 1024);
        char c;

        // Read the command line character by character
        c = getchar();
        while (c != EOF && c != '\n'){
                command[position] = c;

                // Reallocate buffer as and when needed
                if (position >= buf_size){
                        buf_size += 64;
                        command = realloc(command, buf_size);
                }

                position++;
                c = getchar();
        }
        return command;
}

/*
 * Function:  start_process
 * ------------------------
 *  starts and executes a process for a command
 *
 * args: arguments tokenized from the command line
 *
 * return: status 1
 */
/* to learn fork(), pipe(), dup(), signals etc watch the following playlist
 https://www.youtube.com/watch?v=DiNmwwQWl0g
 */
int start_process(char ** args){

        int status;
        pid_t pid, wpid;

        pid = fork();

        if (pid == 0){  // It's the child process
        
        // Find the path of the command
        char cmd_dir[1024];
        strcpy(cmd_dir, PATH);
        strcat(cmd_dir, args[0]);
        
        // Execute the required process
        if ( execvp( args[0], args ) == -1){ // Error
            perror("minsh");
        }

        exit(EXIT_FAILURE);    // To exit from child process
        }
        else if (pid < 0){      // Error in forking
        perror("minsh");
        }
        else{           // It's the parent process
                do{
                        wpid = waitpid(pid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }

        return 1;
}

/*
 * Function:  shell_execute
 * ------------------------
 *  determines and executes a command as a built-in command or an external command
 *
 * args: arguments tokenized from the command line
 *
 * return: return status of the command
 */

int shell_execute(char ** args){

    if (args[0] == NULL){    // Empty command
        return 1;
    }

    // Copy the current Standard Input and Output file descriptors
    // so they can be restored after executing the current command
    int std_in, std_out, std_err;
    std_in = dup(0);
    std_out = dup(1);
    std_err = dup(2);

    // Check if redirection operators are present
    int i = 1;

    while ( args[i] != NULL ){

        if ( strcmp( args[i], "<" ) == 0 ){    // Input redirection
            int inp = open( args[i+1], O_RDONLY );
            if ( inp < 0 ){
                perror("minsh");
                return 1;
            }

            if ( dup2(inp, 0) < 0 ){
                perror("minsh");
                return 1;
            }
            close(inp);
            args[i] = NULL;
            args[i+1] = NULL;
            i += 2;
        }
        
        else if( strcmp(args[i], ">") == 0 ){    // Output redirection

            int out = open( args[i+1], O_WRONLY | O_TRUNC | O_CREAT, 0755 );
            if ( out < 0 ){
                perror("minsh");
                return 1;
            }

            if ( dup2(out, 1) < 0 ){
                perror("minsh");
                return 1;
            }
            close(out);
            args[i] = NULL;
            args[i+1] = NULL;
            i += 2;
        }
        else if( strcmp( args[i], ">>") == 0 ){    // Output redirection (append)
            int out = open( args[i+1], O_WRONLY | O_APPEND | O_CREAT, 0755 );
            if ( out < 0 ){
                perror("minsh");
                return 1;
            }

            if ( dup2(out, 1) < 0 ){
                perror("minsh");
                return 1;

            }
            close(out);
            args[i] = NULL;
            args[i+1] = NULL;
            i += 2;
        }
        else{
            i++;
        }
    }

    // If the command is a built-in command, execute that function
    for(i = 0 ; i < BUILTIN_COMMANDS ; i++){
        if ( strcmp(args[0], builtin[i]) == 0 ){
            int ret_status = (* builtin_function[i])(args);
            
            // Restore the Standard Input and Output file descriptors
            dup2(std_in, 0);
            dup2(std_out, 1);
            dup2(std_err, 2);

            return ret_status;
        }
    }

    // For other commands, execute a child process
    int ret_status = start_process(args);

    // Restore the Standard Input and Output file descriptors
    dup2(std_in, 0);
    dup2(std_out, 1);
    dup2(std_err, 2);

    return ret_status;
}



/*
 Function to check pipe in command and hence separating the two commands on different sides of pipe
 */

int check_pipe(char *line, char** pipeargs ){
    
    
    for (int i = 0; i<2; i++) {
        pipeargs[i] = strsep(&line, "|");
    }
    if (pipeargs[1] == NULL) {
        return 0;
    }
    return 1;
    
}
/*
 Function to store readlines in history
 */
void add_into_arr(char *string) {
    
    for (int i = 9; i > 0; --i)
        strcpy(past_com[i], past_com[i - 1]);
        
    strcpy(past_com[0], string);
    ++argCount;
}

/*
 Function to execute commands that are piped.
 */
void shell_execute_piped(char** parsed, char** parsedpipe)
{
    int p2c[2];
    int c2p[2];
    int i;
    int pid;
    char buf[4096];
    memset(buf,0 , 4096);
    for (i = 0; i<2; i++) {
        pipe(p2c);
        pipe(c2p);
        
        pid = fork();
        if (pid<0) {
            return;
        }
        if (pid == 0) {
            close(1);
            dup2(c2p[1], 1);
            close(0);
            dup2(p2c[0], 0);
            close(p2c[1]);
            close(c2p[0]);
            if (i==0) {
                execvp(parsed[0], parsed);
                fprintf(stderr, "could not execute\n");
                exit(1);
            }else{
                execvp(parsedpipe[0], parsedpipe);
                fprintf(stderr, "could not execute\n");
                exit(1);
            }
        }
        write(p2c[1], buf, strlen(buf));
        close(p2c[1]);
        close(c2p[1]);
        memset(buf, 0, 4096);
        if (read(c2p[0], buf, 4096) > 0) {
            if (i==1) {
                printf("%s\n", buf);
            }
        }
    }
    return;
}

/*
 * Function:  shell_loop
 * ---------------------
 *  main loop of the Mini-Shell
 */

void shell_loop(void){

   

    char * command_line;  //to store the input command

    /* if command has a pipe, then separate and store the two different parts
        -----------
        for eg.
        cat file.txt | grep -n ad
        pipe_parts[0] = cat file.txt
        pipe_parts[1] = grep - n ad

        it will be done with the help of check_pipe() function
    */
    char ** pipe_parts = malloc(sizeof(char*)*2); 

    
    /*
    To store the different parts of command in an array [arguments are separated on the basis of spaces]
    --------------- 
    for eg.
    cat input.txt > output.txt
     
     arguments[0] =  cat          arguments[1] = input.txt
     arguments[2] = >             arguments[3] = output.txt
 
    this will be done with the help of split_command_line() function
     
    NOTE: in case if there is pipe in command then arguments will store the separated values(based on " ") of pipe_part[0]
     
    */ 
    char ** arguments;
    
    
    /*
     If command has pipe then it stores the separted values of pipe_parts[1]
     */
    char ** pipeargs;
    
   
    int status = 1;

    while (status){
        printf("psh>> ");
        
        command_line = read_command_line();        //read input and store in command_line
        
        if ( strcmp(command_line, "") == 0 ){      //if no command entered then skip the rest
            continue;
        }

        
        
        
        add_into_arr(command_line);                // store the command in history
        
        
        
        
        
        /*
            checkpipe() checks if command has a pipe, then separate and store the two different parts in pipe_parts[0]
            and pipe_parts[1]
            then arguments store separated values of pipe_parts[0]
            and pipeargs store separated values of pipe_parts[1]
            execArgsPiped() then executes the commands
        */
        
        if (check_pipe(command_line, pipe_parts)) {
            arguments = split_command_line(pipe_parts[0]);
            pipeargs = split_command_line(pipe_parts[1]);
            shell_execute_piped(arguments, pipeargs);
            free(pipeargs);
            free(pipe_parts);
        }
        
        
        /*
         if no pipe in command
         */
        else{
                arguments = split_command_line(command_line);
                status = shell_execute(arguments);    //executes the command
        }
        
        //free the memory
        free(command_line);
        free(arguments);
        
        
    }
}





/*
 Function to print welcome statements of the program
 */
void start_shell(){
    printf("****** Welcome to Pradeep's Shell ******\n");
    printf("******         ⚙️⚙️⚙️             ******\n");
    printf("******       ⦿⦿⦿⦿▷◁⦿⦿⦿⦿          ******\n");
}
/*
 Function to print statements after exit command
 */
void shell_end(){
    printf("******       ⦿⦿⦿⦿▷◁⦿⦿⦿⦿           ******\n");
    printf("******         ⚙️⚙️⚙️              ******\n");
    printf("****** Thank you for using this shell ******\n");
}







/*
 * Function:  main
 */
int main(int argc, char ** argv){
        start_shell();
      
    
        // Main loop of the shell
        shell_loop();
    
        
        shell_end();
    
        return 0;
}







