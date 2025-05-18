#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm> // for std::rotate
using namespace std;

vector<int64_t> secureSum(vector<int64_t> &TVGlobal)
{
    int m = TVGlobal.size();
    vector<int64_t> temp1 = TVGlobal;
    vector<int64_t> temp2 = TVGlobal;
    for (int i = 0; i < (m / 2) - 1; ++i)
    {
        temp1 = temp2;
        // TODO: replace rotation and addition with homomorphic rotation and addition
        rotate(temp2.begin(), temp2.begin() + pow(2, i), temp2.end());
        for (size_t idx = 0; idx < temp2.size(); ++idx) temp2[idx] += temp1[idx];
    }
    return temp2;
}

int main()
{
    int m = 8;

    vector<int64_t> TVGlobal(m, 0);
    TVGlobal[0] = 1;
    TVGlobal[1] = 2;
    TVGlobal[2] = 3;
    TVGlobal[3] = 4;
    TVGlobal[4] = 5;
    TVGlobal[5] = 6;
    TVGlobal[6] = 7;
    TVGlobal[7] = 8;
    cout << "Training Vector Global:" << endl;
    for (const auto &value : TVGlobal)
        cout << value << " ";
    cout << endl;
    cout << endl;

    vector<int64_t> TVGlobal_secureSummed = secureSum(TVGlobal);
    cout << "Training Vector Global Secure Summed:" << endl;
    for (const auto &value : TVGlobal_secureSummed)
        cout << value << " ";
}