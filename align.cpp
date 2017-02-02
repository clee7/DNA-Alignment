//
//  align.cpp
//  dna_alignment
//
//  Created by Erika DeBenedictis on 1/27/13.
//  Copyright (c) 2014 California Institute of Technology. All rights reserved.
//
#include <iostream>
#include <string>
#include <string.h>
#include <unordered_map>

using namespace std;

// scoring values
#define GAP_SCORE -5
#define MISMATCH -1
#define MATCHING 2

/**
 * @brief Packages the score and instruction string the align function returns
 */
struct align_result {
    int score;      // score of this alignment
    string inst;    // instruction on how to align the inputs

    align_result(int s, string i) {
        this->score = s;
        this->inst = i;
    }
    align_result() {
        this->score = 0;
        this->inst = "";
    }
};

// memo_type will allow us to hash the string input to align
// with its output for memoization
typedef unordered_map<string, align_result> memo_type;

/**
 * @brief Function takes two strings, s and t, and produces an align_result
 * of the highest alignment score and its corresponding instruction str.
 */
 
string instruction(string s, string t) {
	string inst = "";
	if (s.length() == 0 && t.length() == 0) {
		return inst;
	}
	else {
		unsigned int maxi = max(s.length(),t.length());
		for (unsigned int i = 0; i < maxi;i++) {
			if (s.length() <= i) {
				inst.append("t");
			}
			else if (t.length() <= i) {
				inst.append("s");
			}
			else if (s[i] == t[i]) {
				inst.append("|");
			}
			else if (s[i] != t[i]) {
				inst.append("*");
			}
		}
	}
	return inst;
}

int score(string inst) {
	int scor = 0;
	for (unsigned int i = 0;i < inst.length();i++) {
		if (inst[i] == 's' || inst[i] == 't') {
			scor += GAP_SCORE;
		}
		else if (inst[i] == '*') {
			scor += MISMATCH;
		}
		else if (inst[i] == '|') {
			scor += MATCHING;
		}
	}
	return scor;
	
}

align_result align(string s, string t, memo_type &memo) {
    // if this result is memoized, use recorded result
    string key = s + "," + t;
    if (memo.count(key) > 0){
      return memo[key];
    }

     string inst;
     int scor;
     std::string::iterator i;
     align_result answer, solu1, solu2, solu3;
     
     if (s.length() == 0 || t.length() == 0) {
		 inst = instruction(s, t);
		 scor = score(inst);
		 answer.score = scor;
		 answer.inst = inst;
	 }
	 else {
		 solu1 = align(s.substr(1, s.length() - 1), t, memo);
		 inst =  solu1.inst;
		 i = inst.begin();
		 inst.insert(i, 's');
		 solu1.inst = inst;
		 solu1.score += GAP_SCORE;
		 
		 solu2 = align(s, t.substr(1, t.length() - 1), memo);
		 inst =  solu2.inst;
		 i = inst.begin();
		 inst.insert(i, 't');
		 solu2.inst = inst;
		 solu2.score += GAP_SCORE;
		 
		 solu3 = align(s.substr(1,s.length() - 1), 
					   t.substr(1,t.length() - 1), memo);
		 inst =  solu3.inst;
		 i = inst.begin();
		 string prev = instruction(s.substr(0,1),t.substr(0,1));
		 inst.insert(i, prev[0]);
		 solu3.inst = inst;
		 solu3.score += score(inst.substr(0,1));
		 
		 if (solu1.score > solu2.score && solu1.score > solu3.score) {
			 answer = solu1;
		 }
		 else if (solu2.score > solu3.score && solu2.score > solu1.score) {
			 answer = solu2;
		 }
		 else {
			 answer = solu3;
		 }
	 }
	 
    memo[key] = answer;
    return answer;
}

/**
 * @brief Wrapper function to print the results of align
 */
void DNA_align(string s, string t) {
    cout << endl << "Calling DNA align on strings " << s << ", " << t << endl;

    // create the memoization system
    memo_type memo;

    align_result answer = align(s, t, memo);
    string ans = answer.inst;
    int score = answer.score;

    // Printing section
    // line where string s will be printed, spaces inserted
    string line1 = "";
    // line where string t will be printed, spaces inserted
    string line2 = "";
    // description of the relationship between s and t here (* | s t)
    string line3 = "";

    int j = 0;      // running index in s
    int k = 0;      // running index in t

    for (unsigned int m = 0; m < ans.length(); m++) {
        // i is the next element in our instruction string ans
        string i = ans.substr(m, 1);

        // only in s
        if(i.compare("s") == 0){
            line1 += s[j]; j++;
            line2 += " ";
            line3 += "s";
        }

        // only in t
        else if (i.compare("t") == 0){
            line1 += " ";
            line2 += t[k]; k++;
            line3 += "t";
        }

        // mismatch
        else if (i.compare("*") == 0){
            line1 += s[j]; j++;
            line2 += t[k]; k++;
            line3 += "*";
        }

        // match
        else {
            line1 += s[j]; j++;
            line2 += t[k]; k++;
            line3 += "|";
        }
    }
    cout << line1 << endl << line2 << endl << line3 << endl;
    cout << "Score for this alignment: " << score << endl;
}

int main(){
    // some test cases to begin with
    DNA_align("agcaaaagcaggggttcaatctgtcaaaatggagaaaatagtgcttcttcttgcaatagtcagtcttgttaaaagtgatcagatttgcattggttaccatgcaaacaactcgacagagcaggttgacacaataatggaaaagaacgtcactgttacacacgcccaagatatactggaaaaggcacacaacgggaagctctgcgatctagacggagtgaagcctctaattttaagagattgtagtgtagctggatggctcctcgggaacccaatgtgtgacgaattcctcaatgtgccggaatggtcttacatagtggagaagatcaatccagccaatgacctctgttacccagggaatttcaacgactatgaagaactgaaacacctattgagcagaataaaccattttgagaaaattcagatcatccccaaaagttcttggtcagatcatgaagcctcatcaggggtgagctcagcatgtccataccagggaaggtcctccttttttagaaatgtggtatggcttatcaaaaagaacgatgcatacccaacaataaagagaagttacaataataccaaccaagaagatcttttggtactgtgggggattcaccatccaaatgatgcggcagagcagacaaggctctatcaaaacccaaccacctatatttccgttgggacatcaacactaaaccagagattggtcccaaaaatagctactagatccagggtaaacgggcaaagtggaaggatggagttcttttggacaattttaaaaccgaatgatgcaataaactttgagagtaatggaaatttcattgctccagaaaatgcatacaaaattgtcaagaaaggggactcaacaatcatgaaaagtgaattggaatatgg", "atggagaaaatagtgcttcttcttgcaatagtcagtcttgttaaaagtgatcagatttgcattggttaccatgcaaacaactcgacagagcaggttgacacaataatggaaaagaacgtcactgttacacacgcccaagacatactggaaaagacacacaacgggaaactctgcgatctagatggagtgaagcctctaattttgagagattgtagtgtagctggatggctcctcgggaacccaatgtgtgacgaattcctcaatgtgccggaatggtcttacatagtggagaagatcaatccagccaatgacctctgttacccagggaatttcaacgactatgaagaactgaaacacctattgagcagaataaaccattttgagaaaattcagatcatccccaaaagttcttggtcagatcatgaagcctcatcaggggtgagctcagcatgtccataccagggaaggtcctccttttttagaaatgtggtatggcttatcaaaaaggacaatgcatacccaacaataaagagaagttacaataataccaaccaagaagatcttttggtactgtgggggattcaccatccaaatgatgcggcagagcagacaaggctctatcaaaacccaactacctatatttccgttgggacatcaacactaaaccagagattggtaccaaaaatagctactagatctaaggtaaacgggcaaagtggaaggatggagttcttttggacaattttaaaaccgaatgatgcaataaactttgagagtaatggaaatttcattgctccagaaaatgcatacaaaattgtcaagaaaggggactcaacaattatgaaaagtgagttggaatatggtaactgcaacaccaagtgtcaaactccaatag");
    return 0;
}
