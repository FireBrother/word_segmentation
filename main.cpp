#include <iostream>
#include <vector>
#include <string>
#include "Limonp/StringUtil.hpp"
#include "Limonp/Logger.hpp"

using namespace std;

typedef unordered_map<u16string, double> dict_t;

dict_t global_dict;
double min_dict_value = 3.14e+100, max_dict_value = 3.14e+100;


int load_dict(const string& filename,  dict_t& dict=global_dict) {
    ifstream fin(filename);
    if (!fin) {
        LogError("Opening dict file %s failed.", filename.c_str());
    }
    dict.clear();
    double sum = 0.0, freq;
    string word, tag;
    int count = 0;
    while (fin >> word >> freq >> tag) {
        count++;
        unicode_t unicode = converter.from_bytes(word);
        dict[unicode] = freq;
        sum += freq;
        if (count > 10)
            break;
    }
    for (auto &x : dict) {
        x.second = log(x.second / sum);
    }
    for (auto &itr : dict) {
        min_dict_value = min(min_dict_value, itr.second);
        max_dict_value = max(max_dict_value, itr.second);
    };
    LogInfo("Loading dict finished: %d words loaded.", count);
    return count;
}

int main() {
    dict_t dict;
    cout << load_dict(DICT_FILEPATH, dict) << endl;
    cout << dict << endl;
    cout << min_dict_value << ' ' << max_dict_value << endl;
    return 0;
}