
#include <iostream>
#include <vector>
#include <string>

using namespace std;

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

vector<string> split(string line, char separator = ' ') {
    vector<string> parsedInput;
    int c = 0;
    int num_quotes = 0;
    bool inside_quotes = false;
    int start = 0;

    for(int i = 0; i < line.size(); i++) {

        if (separator == ' ' && (line[i] == '\'' || line[i] == '\"')) {
            num_quotes++;
            string word;
            if (num_quotes %2 !=0) {
                inside_quotes = true;
                start = i+1;
            }
            else { //quote finished 
                inside_quotes = false;
                for (int j = start; j < i; j++)
                    word.push_back(line[j]);
                parsedInput.push_back(word);
            }
        }
       else if (inside_quotes == false) {
            if((line[i] == separator) || (i == (line.size()-1))) {
                string words ;
                if (i == line.size() - 1) {
                    for (int j = c; j < i + 1; j++)
                        words.push_back(line[j]);
                }
                else {
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

int main(){
    string testString =  "    hello    \"   w\'or         \'ld   \"!!     ";
    cout << testString << endl;
    std::cout << trim(testString) << std::endl;

    return 0;
}