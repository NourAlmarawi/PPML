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
	int m = multiplied_query.size() / 2 - 1;
	int n = static_cast<int>(log2(m + 1));
	vector<long double> temp = multiplied_query;

	// Sum first half (0 to m)
	vector<long double> first_half(temp.begin(), temp.begin() + m + 1);
	vector<long double> first_sum = first_half;
	for (int i = 0; i < n; ++i)
	{
		vector<long double> rotated = first_sum;
		rotate(rotated.begin(), rotated.begin() + (1 << i), rotated.end());
		for (size_t idx = 0; idx < first_sum.size(); ++idx)
			first_sum[idx] += rotated[idx];
	}

	// Sum second half (m+1 to end)
	vector<long double> second_half(temp.begin() + m + 1, temp.end());
	vector<long double> second_sum = second_half;
	for (int i = 0; i < n; ++i)
	{
		vector<long double> rotated = second_sum;
		rotate(rotated.begin(), rotated.begin() + (1 << i), rotated.end());
		for (size_t idx = 0; idx < second_sum.size(); ++idx)
			second_sum[idx] += rotated[idx];
	}

	// Combine results
	vector<long double> result = temp;
	for (int i = 0; i <= m; ++i)
		result[i] = first_sum[i];
	for (int i = 0; i < second_sum.size(); ++i)
		result[m + 1 + i] = second_sum[i];

	return result;
}

int main()
{
	string path = "enron5";
	// string path = "dataset_reduced";
	// string path = "test_data";
	int m = 2047;
	// int m = 16;
	// int polyModulus = 16384;
	int polyModulus = 4096;
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
	cout << "CLASSIFICATION ON test_file.txt" << endl;
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
	// cout << setw(15) << "index"
	// 	 << setw(15) << "Feature"
	// 	 << setw(20) << "query"
	// 	 << endl;
	// for (int i = 0; i < query.size(); i++)
	// {
	// 	cout << setw(15) << i
	// 		 << setw(15) << selectedFeatures[i]
	// 		 << setw(20) << query[i]
	// 		 << endl;
	// }

	query[0] = 1;
	vector<int> duplicatedQuery = query;
	query.insert(query.end(), duplicatedQuery.begin(), duplicatedQuery.end());

	vector<long double> query_multiplied(query.size(), 0.0);
	for (size_t i = 0; i < query.size() && i < finalModel.size(); ++i)
		query_multiplied[i] = query[i] * finalModel[i];

	//? debug
	// cout << "finalModel.size()=" << finalModel.size() << endl;
	// cout << "query.size()=" << query.size() << endl;
	// cout << setw(15) << "index"
	// 	 << setw(15) << "query"
	// 	 << setw(20) << "finalModel"
	// 	 << setw(20) << "query_multiplied"
	// 	 << endl;
	// for (int i = 0; i < query.size(); i++)
	// {
	// 	cout << setw(15) << i
	// 		 << setw(15) << query[i]
	// 		 << setw(20) << finalModel[i]
	// 		 << setw(20) << query_multiplied[i]
	// 		 << endl;
	// }

	// secure sum
	vector<long double> summed_query = secureSum(query_multiplied);
	// cout << "multiplied_query.size()=" << query_multiplied.size() << endl; //? debug
	// cout << "summed_query.size()=" << summed_query.size() << endl;  // ? debug
	// for (size_t i = 0; i < summed_query.size(); ++i) cout << summed_query[i] << " "; //? debug

	cout << endl << "comparing the results" << endl;
	if (summed_query[0] > summed_query[m + 1])
		cout << "The email is classified as HAM" << endl;
	else
		cout << "The email is classified as SPAM" << endl;

	cout << endl
		 << "CLASSIFICATION ON ENRON5" << endl;
	// Confusion matrix counters
	int true_ham = 0, false_spam = 0, true_spam = 0, false_ham = 0;
	vector<pair<string, int>> test_dirs = {{"ham", 0}, {"spam", 1}};
	for (const auto &dirinfo : test_dirs)
	{
		int total_files = 3675 + 1500;
		int processed_files = 0;
		auto print_progress = [&](int processed) {
			int percent = static_cast<int>(100.0 * processed / total_files);
			cout << "\rProgress: [";
			int barWidth = 50;
			int pos = percent * barWidth / 100;
			for (int i = 0; i < barWidth; ++i)
				cout << (i < pos ? '=' : ' ');
			cout << "] " << percent << "% (" << processed << "/" << total_files << ")" << flush;
		};
		
		string dirpath = path + "/" + dirinfo.first;
		for (const auto &entry : fs::filesystem::directory_iterator(dirpath))
		{
			if (!fs::filesystem::is_regular_file(entry.status()))
				continue;
			ifstream infile(entry.path().string());
			vector<int> q(selectedFeatures.size(), 0);
			string w;
			while (infile >> w)
			{
				for (size_t i = 1; i < selectedFeatures.size(); ++i)
				{
					if (w == selectedFeatures[i])
						q[i]++;
				}
			}
			infile.close();
			q[0] = 1;
			vector<int> dq = q;
			q.insert(q.end(), dq.begin(), dq.end());
			vector<long double> qm(q.size(), 0.0);
			for (size_t i = 0; i < q.size() && i < finalModel.size(); ++i)
				qm[i] = q[i] * finalModel[i];
			vector<long double> sq = secureSum(qm);
			bool is_ham = sq[0] > sq[m + 1];
			if (dirinfo.second == 0)
			{ // actual ham
				if (is_ham)
					true_ham++;
				else
					false_spam++;
			}
			else
			{ // actual spam
				if (!is_ham)
					true_spam++;
				else
					false_ham++;
			}
		}
	}

	cout << "\nConfusion Matrix:\n";
	cout << "                Predicted HAM   Predicted SPAM\n";
	cout << "Actual HAM   " << setw(10) << true_ham << setw(18) << false_spam << endl;
	cout << "Actual SPAM  " << setw(10) << false_ham << setw(18) << true_spam << endl;

	return 0;
}