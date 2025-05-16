#include <time.h>
#include "MultinomialNB_Email.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <chrono>
#include <experimental/filesystem>
namespace fs = std::experimental;
using std::vector;
using namespace std;


int main()
{
	// string path = "enron5";
	string path = "dataset_reduced";
	// int m = 2047;
	int m = 20;
	long long int timeElapsed = 0;
	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed), tmpmNEmail;
	tmpmNEmail = MNB_Email;
	tmpmNEmail.train(timeElapsed);
	tmpmNEmail.classify(path, timeElapsed);
	tmpmNEmail.printConfionMat();
	return 0;
}