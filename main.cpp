#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <queue>
#include "Limonp/StringUtil.hpp"
#include "Limonp/Logger.hpp"

using namespace std;

typedef unordered_map<unicode_t, double> dict_t;

double get(const dict_t& dict, unicode_t key, double value) {
    auto itr = dict.find(key);
    if (itr != dict.end())
        return itr->second;
    return value;
}

dict_t global_dict;
double min_dict_value = 1e10, max_dict_value = 1e10;


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

void find_shortest_path(vector<unsigned int>& path, vector<vector<double> >& adjmat) {
    path.clear();
    const unsigned long N = adjmat.size();
    queue<unsigned int> q;
    vector<bool> inq(N, false);
    vector<double> dis(N, -1000000000);
    path.resize(N);

    q.push(0);
    inq[0] = true;
    while (!q.empty()) {
        unsigned int now = q.front();
        q.pop();
        inq[now] = false;
        for (unsigned int next = now + 1; next < N; next++) {
            if (dis[next] < dis[now] + adjmat[now][next]) {
                dis[next] = dis[now] + adjmat[now][next];
                path[next] = now;
                if (!inq[next])
                    q.push(next);
            }
        }
    }

}

vector<unicode_t> _cut(const unicode_t& sentence, const dict_t& dict = global_dict) {
    vector<vector<double> > adjmat;
    const unsigned long N = sentence.size() + 1;
    adjmat.resize(N);
    for (auto &row: adjmat)
        row.resize(N);
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = i + 1; j < N; j++) {
            unicode_t unicode = sentence.substr(i, j - i);
            adjmat[i][j] = get(dict, unicode, min_dict_value*unicode.length()) - min_dict_value * unicode.length() + 1e-6;
        }
    }

    // 发现英文串
    int e_begin, e_end;
    for (e_begin = -1, e_end = 0; e_end < N; e_end++) {
        if (is_english(sentence[e_end])) {
            if (e_begin == -1) e_begin = e_end;
        }
        else {
            if (e_begin != -1) {
                adjmat[e_begin][e_end] = max_dict_value;
                e_begin = -1;
            }
        }
    }
    vector<unsigned int> path;
    vector<unicode_t> words;
    find_shortest_path(path, adjmat);
    words.reserve(path.size());
    unsigned long pos = N - 1;
    while (pos != 0) {
        words.push_back(sentence.substr(path[pos], pos - path[pos]));
        pos = path[pos];
    }
    reverse(words.begin(),words.end());
    return words;
}

vector<unicode_t> cut(const unicode_t& sentence, const dict_t& dict = global_dict) {
    vector<unicode_t> words;
    size_t begin, end;
    for (begin = 0, end = 0; end < sentence.length(); end++) {
        if (P_set.find(sentence[end]) != unicode_t::npos) {
            // LogDebug("Cutting \"%s\".", Unicode2gbk(sentence.substr(begin, end - begin)).c_str());
            auto tmp = _cut(sentence.substr(begin, end - begin), dict);
            for (const auto& w : tmp) {
                words.push_back(w);
            }
            begin = end + 1;
            if (sentence[end] != u' ')
                words.push_back(unicode_t(1, sentence[end]));
        }
    }
    // LogDebug("Cutting \"%s\".", Unicode2gbk(sentence.substr(begin, end - begin)).c_str());
    if (begin != end) {
        auto tmp = _cut(sentence.substr(begin, end - begin), dict);
        for (const auto& w : tmp) {
            words.push_back(w);
        }
    }
    return words;
}

vector<string> cut(const string& sentence, const dict_t& dict = global_dict) {
    vector<string> words;
    vector<unicode_t> vecUniWords = cut(converter.from_bytes(sentence), dict);
    for(const auto& word: vecUniWords) {
        words.push_back(converter.to_bytes(word));
    };
    return words;
}

int main() {
    load_dict(DICT_FILEPATH);
    cout << min_dict_value << ' ' << max_dict_value << endl;
    string src;
    while (true) {
        cout << "请输入字符串：";
        cin >> src;
        if (src == "exit")
            break;
        cout << cut(src) << endl;
    }
    return 0;
}