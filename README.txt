توضیح مختصر راجع به برنامه و توابع:

0 main(): accept arguments `argCount` and `args` : for detecting Batch mode
    0.0 Batch mode:
        0.0.0 open the file if exist
        0.0.1 read file line by line
        0.0.2 remove white space using trim()
        0.0.3 print whole line using puts()
        0.0.4 run command using runCmd()
    
    0.1 Interactive mode:
        0.1.0 print current directory using printDir()
        0.1.1 wait for user to enter some command using takeInput()
        0.1.2 run proper command using runCmd()

1 trim(string): navigate `string` and remove whitespace

2 runCmd(cmd):
    2.0 split `cmd` with semicolon character and skip empty commands
    2.1 detect piped or non-piped commands using processString()
        2.1.0 if non-piped: execute command using execArgs()
        2.1.1 if piped: execute command using execArgsPiped()

3 processString(string, parsed, parsedpipe):
    3.0 parse `string` using parsePipe()
        3.0.0 if piped: give args to both `parsed` and `parsedpipe`
        3.0.1 if non-piped: give args only to `parsed`
    3.1 check for own commands using ownCmdHandler()

4 ownCmdHandler(str): we have 2 own commands including 'quit' and 'cd'
    4.0 if 'quit': print Goodbye and exit(0)
    4.1 if 'cd' : change directory using builtin func chdir()

5 execArgs(args): create child process using fork() and  \
                  execute proper command using execvp(`args`)

6 execArgsPiped(args, parsedpipe) : create two child processes using fork() \
                                    and execute each using `args` and `parsedpipe`

7 takeInput() :
    7.0 get user input using readline()
    7.1 detect empty string
    7.2 add buffer to history using add_history()

8 printDir(): print directory using getcwd()
9 signal_handler(int) : activate sigswitch and don't break