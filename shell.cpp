#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <sstream>
#include<ctime>
#include <time.h>

using namespace std;

/*
TODO:

 - Implement cd ~ (takes you to the home directory)


*/

string trimFrontBack(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

string trim(string input) {
    //Eliminate whitespace at the front and back
    input = trimFrontBack(input);

    string trimString = "";

    vector<char> quoteChecker;
    bool inQuotations = false;

    for (int i = 0; i < input.size(); i++){
        //Handle quotes
        if (input[i] == '\"') {
            quoteChecker.push_back(input[i]);
            inQuotations = true;

            if (quoteChecker.at(0) == quoteChecker.at(quoteChecker.size()-1) && quoteChecker.size() > 1){
                inQuotations = false;
                quoteChecker.clear();
            }
        }
        else if (input[i] == '\'') {
            quoteChecker.push_back(input[i]);
            inQuotations = true;

            if (quoteChecker.at(0) == quoteChecker.at(quoteChecker.size()-1)  && quoteChecker.size() > 1){
                inQuotations = false;
                quoteChecker.clear();
            }
        }

        // Handle extra spaces
        if (isspace(input[i]) && isspace(input[i-1]) && !inQuotations) {
            continue;
        }
        
        trimString.push_back(input[i]);

    }

    return trimString;
}

//Ensure this is working
char** vec_to_char_array(vector<string>& parts)
{
	char** result = new char* [parts.size() +1]; // adds one because of NULL
	for (int i = 0; i < parts.size(); i++)
	{
		result[i] = (char*) parts[i].c_str();
	}

	result [parts.size()] = NULL;
	return result;
}

vector<string> split(string line, char separator = ' ') {
    vector<string> parsedInput;
    int c = 0;
    int num_quotes = 0;
    bool inside_quotes = false;
    int start = 0;

    for(int i = 0; i < line.size(); i++) {

        if (separator == ' ' && (line[i] == '\'' || line[i] == '\"'))
        {
            num_quotes++;
            string word;
            if (num_quotes %2 !=0)
            {
                inside_quotes = true;
                start = i+1;
            }
            else //quote finished
            {
                inside_quotes = false;
                for (int j = start; j < i; j++)
                    word.push_back(line[j]);
                parsedInput.push_back(word);
            }
        }
       else if (inside_quotes == false)
        {
            if((line[i] == separator) || (i == (line.size()-1)))
            {
                string words ;
                if (i == line.size() - 1)
                {
                    for (int j = c; j < i + 1; j++)
                        words.push_back(line[j]);
                }
                else
                {
                    for (int j = c; j < i; j++)
                        words.push_back(line[j]);
                }
                parsedInput.push_back(words);
                c = i + 1;
            }

        }
    }
    return parsedInput;
}

int main()
{
	vector <int> bgp; //background processes
	int keyboard = dup(0);  //copies standard input of user from keyboard
	int stdout = dup(1); //copies standard output

    char buff[1000]; // Needs to be large enough to store directory paths (Might increase later idk)

	string currDir = getcwd(buff,sizeof(buff));
    chdir("..");
    string prevDir = getcwd(buff,sizeof(buff));
    chdir(currDir.c_str());

	while (true) {

        //Loop through background processes
		for(int i = 0; i < .size(); i++) {
			if(waitpid(bgp[i], 0, WNOHANG) == bgp[i]) { // //WNOHANG returns immediately if no child has exited //Wait to kill the parent until ALL child processes are done
				cout << "Process: " << bgp[i] << " ended" <<endl;
				bgp.erase(bgp.begin() + i); // remove process from list

                //one less process, need to subtract i by 1 to maintain order of looping through processes
				i--;
			}
		}

		//copy the keyboard and terminal value to 0 index of descriptor table
		dup2(keyboard,0); //keyboard input --> 0
		dup2(stdout,1); //stdout --> 1

		//char* name = getlogin_r(3);

        //Print prompt
        time_t time;
		cout << "osboxes " << ctime(time);

		string inputline;
		getline (cin, inputline); //get a line from standard input

		if(inputline == string("exit"))
		{
			cout << "Bye!! End of Shell " <<endl;
			break;
		}

		// Check for background processes
		bool bg = false;
		inputline = trim(inputline);//gets rid of leading and trailing spaces
		//cout << "Input line : " <<inputline  <<"blaah" <<endl;
		if(inputline[inputline.size()-1] =='&')
		{
			cout << "Background process found" <<endl;
			bg = true;
			inputline = trim(inputline.substr(0,inputline.size()-1));//removes & + the space from the end of the string
		}

		vector<string> pparts = split(inputline, '|');
//		for (auto i:pparts)
//        {
//		    cout << "PPARTS " <<i <<endl;
//        }


		for( int i = 0; i < pparts.size();i++)
        {
		    int fds[2];
		    pipe(fds);
            inputline = trim(pparts[i]);

		    int pid = fork();

            if (pid == 0) //child process
            {
                //cd stuff

                if(trim(inputline).find("cd") ==0)
                {
                    currDir = getcwd(buff,sizeof(buff));
                   // cout << trim(inputline).find("-");
                    if (trim(inputline).find("-") == 3) {
                        //string dirname = trim(split(inputline)[1]);
                        chdir(prevDir.c_str());
                        prevDir = currDir;
                    }
                    else
                    {
                        string dirname = trim(split(inputline)[1]);
                        chdir(dirname.c_str());
                        prevDir = currDir;
                    }
                    continue;
                }



                //do the IO redirection HERE -> open the file then call dup2
                int pos = inputline.find('>');
                if (pos >= 0)
                {
                    string command = trim(inputline.substr(0,pos)); //ls
                    string filename = trim(inputline.substr(pos+1));//filename
                    inputline = command;
                    int fd = open(filename.c_str(), O_WRONLY|O_CREAT,S_IWUSR|S_IRUSR);
                    dup2(fd,1);
                    close(fd);
                }

                pos =inputline.find('<');
                if (pos >= 0)
                {
                    string command = trim(inputline.substr(0,pos)); //ls
                    string filename = trim(inputline.substr(pos+1));//filename
                    inputline = command;
                    int fd = open(filename.c_str(), O_RDONLY,S_IWUSR|S_IRUSR);
                    dup2(fd,0);
                    close(fd);
                }


                if( i < pparts.size() - 1)
                {
                    dup2(fds[1] , 1);
                    close(fds[1]);
                    //cout <<inputline <<endl;
                }
               // inputline = pparts.at(i);

                vector <string> parts = split(inputline);
                char** args = vec_to_char_array (parts);
                execvp(args[0],args);

            }
            else
            {
//			//pipe between two processes
//			char buf [100];
//			read (fds[0],buf, 100);
//			printf("PARENT: Recv %s\n", buf);

                if(bg == false)
                {
                    if (i == pparts.size()-1)
                    {
                        waitpid(pid,0,0);

                    }
                    //waitpid(pid,0,0); //parent waits for child process
                }
                else
                {
                    bgp.push_back(pid); //we do not loose track of the background processes
                    //cout << "Here" <<endl;
                }

                dup2(fds[0],0);
                close(fds[1]); //fd[1] MUST be closed, otherwise the next level will wait
            }
		}
	}
	return 0;
}