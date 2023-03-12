/**
 *   Fast BPE Merge List Building
 * 
 *   -- Might have bugs though!
 */
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <queue>
#include <tuple>
using namespace std;
struct Word {
  list<string> toks;
  int freq;
};
int main() {
  list<Word> words;
  using TokPair = pair<string, string>;
  map<TokPair, unordered_set<Word*>> wordsWithPair;
  vector<pair<string, string>> merges;
  set<tuple<int, string, string>> pq;
  map<TokPair, int> pairFreq;
  {
    string word;
    int freq;
    while (cin >> word >> freq) {
      list<string> toks;
      for (char c : word) {
        string s{c};
        toks.push_back(s);
      }
      words.push_back({.toks=move(toks), .freq=freq});
    }
  }
  for (auto& word : words) {
    for (auto i = word.toks.begin(); next(i) != word.toks.end(); ++i) {
      auto p = make_pair(*i, *next(i));
      wordsWithPair[p].insert(&word);
    }
  }
  auto update = [&](const string& a, const string& b, int val) {
    auto p = make_pair(a, b);
    auto [i, _] = pairFreq.try_emplace(p);
    pq.erase(make_tuple(i->second, a, b));
    i->second += val;
    pq.emplace(i->second, a, b);
  };
  auto pop = [&]() {
    auto itr = prev(pq.end());
    auto tup = *itr;
    pairFreq.erase(make_pair(get<1>(tup), get<2>(tup)));
    pq.erase(itr);
    return tup;
  };
  for (auto itr = wordsWithPair.begin(); itr != wordsWithPair.end(); ++itr) {
    auto [t1, t2] = itr->first;
    int freq = 0;
    for (auto w : itr->second) freq += w->freq;
    pairFreq[make_pair(t1, t2)] = freq;
    pq.emplace(freq, move(t1), move(t2));
  }
  while (merges.size() < 20000) {
    cerr << merges.size() << " " << pq.size() << endl;
    auto [freq, a, b] = pop();
    auto p = make_pair(a, b);
    auto v = a+b;
    auto itr = wordsWithPair.find(p);
    if (itr == wordsWithPair.end()) continue;
    merges.emplace_back(a, b);
    map<TokPair, int> updates;
    for (auto word : itr->second) {
      for (auto i = word->toks.begin(); i != word->toks.end() && next(i) != word->toks.end(); ++i) {
        if (*i != a|| *next(i) != b) continue;
        if (i != word->toks.begin()) {
          auto p1 = make_pair(*prev(i), a);
          auto p2 = make_pair(*prev(i), v);
          updates[p1] -= word->freq;
          wordsWithPair[p1].erase(word);
          updates[p2] += word->freq;
          wordsWithPair[p2].insert(word);
        }
        i = word->toks.erase(i);
        *i = v;
        if (next(i) != word->toks.end()) {
          auto p1 = make_pair(b, *next(i));
          updates[p1] -= word->freq;
          wordsWithPair[p1].erase(word);
          auto p2 = make_pair(v, *next(i));
          updates[p2] += word->freq;
          wordsWithPair[p2].insert(word);
        }
      }
    }
    wordsWithPair.erase(itr);
    for (auto itr = updates.begin(); itr != updates.end(); ++itr) {
      update(itr->first.first, itr->first.second, itr->second);
    }
    if (pq.empty()) break;
  }
  for (auto& [a, b] : merges) {
    cout << a << ' ' << b << endl;
  }
  return 0;
}
