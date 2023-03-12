/**
 *   Fast BPE Merge List Building
 * 
 *   Usage:
 * 
 *   ./bpe <vocab size> < ~/bpe/1-prepare/words.txt
 */
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <queue>
#include <tuple>
#include "third-parth/utf8.h"
using namespace std;

inline void decode_utf8(const string& bytes, wstring& wstr) {
  utf8::utf8to32(bytes.begin(), bytes.end(), back_inserter(wstr));
}

inline void encode_utf8(const wstring& wstr, string& bytes) {
  utf8::utf32to8(wstr.begin(), wstr.end(), back_inserter(bytes));
}

struct Word {
  list<wstring> toks;
  int freq;
};
int main(int argc, char *argv[]) {
  int vocabSize = stoi(argv[1]);

  list<Word> words;
  using TokPair = pair<wstring, wstring>;
  map<TokPair, unordered_set<Word*>> wordsWithPair;
  unordered_set<wstring> vocab;
  vector<pair<wstring, wstring>> merges;
  set<tuple<int, wstring, wstring>> pq;
  map<TokPair, int> pairFreq;
  {
    string word;
    int freq;
    while (cin >> word >> freq) {
      list<wstring> toks;
      wstring bytes;
      decode_utf8(word, bytes);
      for (auto c : bytes) {
        wstring s{c};
        vocab.insert(s);
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
  auto update = [&](const wstring& a, const wstring& b, int val) {
    auto p = make_pair(a, b);
    auto [i, _] = pairFreq.try_emplace(p);
    pq.erase(make_tuple(i->second, a, b));
    i->second += val;
    if (i->second) pq.emplace(i->second, a, b);
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
  while (vocab.size() < vocabSize) {
    cerr << vocab.size() << ' ' << merges.size() << " " << pq.size() << endl;
    auto [freq, a, b] = pop();
    auto p = make_pair(a, b);
    auto v = a+b;
    auto itr = wordsWithPair.find(p);
    if (itr == wordsWithPair.end()) continue;
    vocab.insert(v);
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
          auto p2 = make_pair(v, *next(i));
          updates[p1] -= word->freq;
          wordsWithPair[p1].erase(word);
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
  cout << vocab.size() << endl;
  for (auto& v : vocab) {
    string word;
    encode_utf8(v, word);
    cout << word << '\n';
  }
  cout << merges.size() << endl;
  for (auto& [a, b] : merges) {
    string wa, wb;
    encode_utf8(a, wa), encode_utf8(b, wb);
    cout << wa << ' ' << wb << '\n';
  }
  return 0;
}
