#include <time.h>
#include "MultinomialNB_Email.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <chrono>
#include <experimental/filesystem>
#include <seal/seal.h>
namespace fs = std::experimental;
using std::vector;
using namespace std;
using namespace seal;

vector<long double> secureSum(vector<long double> &multiplied_query)
{
	int m = multiplied_query.size();
	int n = static_cast<int>(log2(m));
	vector<long double> temp1 = multiplied_query;
	vector<long double> temp2 = multiplied_query;
	for (int i = 0; i < n; ++i)
	{
		temp1 = temp2;
		// TODO: replace rotation and addition with homomorphic rotation and addition
		rotate(temp2.begin(), temp2.begin() + pow(2, i), temp2.end());
		for (size_t idx = 0; idx < temp2.size(); ++idx)
			temp2[idx] += temp1[idx];
	}
	return temp2;
}

int main()
{
	// string path = "enron5";
	// string path = "dataset_reduced";
	string path = "test_data";
	// int m = 2047;
	int m = 16;
	// int polyModulus = 16384;
	int polyModulus = 100;
	long long int timeElapsed = 0;

	MultinomialNB_Email MNB_Email = MultinomialNB_Email(path, m, timeElapsed), tmpmNEmail;
	tmpmNEmail = MNB_Email;
	tmpmNEmail.train(timeElapsed);

	// create the model vector
	vector<string> selectedFeatures;
	vector<long double> logOfProbWordIsHam;
	vector<long double> logOfProbWordIsSpam;
	long double logProbHam;
	long double logProbSpam;
	tmpmNEmail.getSelectedFeaturesParameters(selectedFeatures, logOfProbWordIsHam, logOfProbWordIsSpam, logProbHam, logProbSpam);
	vector<long double> finalModel(polyModulus, 0.0);

	cout << "logOfProbWordIsHam.size()=" << logOfProbWordIsHam.size() << endl;
	cout << "logOfProbWordIsSpam.size()=" << logOfProbWordIsSpam.size() << endl;
	finalModel[0] = logProbHam;
	finalModel[m + 1] = logProbSpam;
	// finalModel[0] = 999; //? debug
	// finalModel[m + 1] = 999; //? debug
	for (int i = 1; i <= m; i++)
	{
		finalModel[i] = logOfProbWordIsHam[i];
		finalModel[m + 1 + i] = logOfProbWordIsSpam[i];
		// finalModel[i] = 555; //? debug
		// finalModel[m + 1 + i] = 222; //? debug
	}

	//? debug
	// cout << "finalModel.size()=" << finalModel.size() << endl;
	// for (size_t i = 0; i < finalModel.size(); ++i)
	// {
	// 	cout << "finalModel[" << i << "]=" << finalModel[i] << endl;
	// }

	// create the query
	cout << "CLASSIFICATION" << endl;
	string filename = "test_data/test_file.txt";
	ifstream infile(filename);
	vector<int> query(selectedFeatures.size(), 0);
	string word;
	while (infile >> word)
	{
		for (size_t i = 1; i < selectedFeatures.size(); ++i)
		{
			if (word == selectedFeatures[i])
			{
				query[i]++;
			}
		}
	}
	infile.close();

	//? debug
	cout << setw(15) << "index"
		 << setw(15) << "Feature"
		 << setw(20) << "query"
		 << endl;
	for (int i = 0; i < query.size(); i++)
	{
		cout << setw(15) << i
			 << setw(15) << selectedFeatures[i]
			 << setw(20) << query[i]
			 << endl;
	}

	query[0] = 1;
	vector<int> duplicatedQuery = query;
	query.insert(query.end(), duplicatedQuery.begin(), duplicatedQuery.end());

	vector<long double> result_multiplication(query.size(), 0.0);
	for (size_t i = 0; i < query.size() && i < finalModel.size(); ++i)
		result_multiplication[i] = query[i] * finalModel[i];

	cout << "finalModel.size()=" << finalModel.size() << endl;
	cout << "query.size()=" << query.size() << endl;
	cout << setw(15) << "index"
		 << setw(15) << "query"
		 << setw(20) << "finalModel"
		 << setw(20) << "result_multiplication"
		 << endl;
	for (int i = 0; i < query.size(); i++)
	{
		cout << setw(15) << i
			 << setw(15) << query[i]
			 << setw(20) << finalModel[i]
			 << setw(20) << result_multiplication[i]
			 << endl;
	}

	// secure sum
	// vector<long double> test = {1, 2, 3, 4, 5, 6, 7, 8};
	vector<long double> summed_query = secureSum(result_multiplication);
	cout << "summed_query.size()=" << summed_query.size() << endl;
	for (size_t i = 0; i < summed_query.size(); ++i) cout << "summed_query[" << i << "]=" << summed_query[i] << " ";

	return 0;
}