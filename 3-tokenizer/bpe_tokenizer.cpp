#include <iostream>
#include <map>
#include <list>
#include <climits>
using namespace std;

int main() {
  int v, w;
  cin >> v;
  map<pair<string, string>, int> rank;
  for (int i = 0; i < v; ++i) {
    string a, b;
    cin >> a >> b;
    rank[make_pair(a, b)] = i;
  }
  cin >> w;
  while (w--) {
    string s;
    cin >> s;
    list<string> tok;
    for (char c : s) tok.insert(tok.end(), string{c});
    while (true) {
      int best = INT_MAX;
      decltype(tok)::iterator k;
      for (auto i = tok.begin(); next(i) != tok.end(); ++i) {
        auto itr = rank.find(make_pair(*i, *next(i)));
        if (itr == rank.end()) continue;
        if (itr->second < best) {
          best = itr->second;
          k = i;
        }
      }
      if (best == INT_MAX) break;
      *k += *next(k);
      tok.erase(next(k));
    }
    for (auto& t : tok) cout << t << ' ';
    cout << '\n';
  }
  return 0;
}
