#include <utility>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "cert-msc32-c"
#pragma clang diagnostic ignored "-Wunused-result"
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <random>
#include <vector>
#include <stack>
#include <deque>
#include <queue>
#include <array>
#include <list>
#include <bitset>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <search.h>
#include <climits>
#include <functional>
#include <utility>
#include <string>
#include <cstring>
#include <cassert>

#define TASK ""
#define forn(i, s, n) for(auto (i) = (s); (i) < (n); (i)++)
#define forb(i, s, n) for(auto (i) = (s); (i) >= (n); (i)--)
#define fora(i, a) for(auto (i) : (a))
#define foraa(i, a) for(auto & (i) : (a))
#define size(a) (int((a).size()))
#define sqr(i) ((i) * (i))
#define all(a) (a).begin(), (a).end()
#define rall(a) (a).rbegin(), (a).rbegin()
#define vec vector
#define pr pair
#define sc second
#define fs first
#define umap unordered_map
#define uset unordered_set
#define pq priority_queue
#define eb emplace_back
#define lb lower_bound
#define ub upper_bound
#define mp make_pair
#define pb pop_back

using namespace std;

typedef unsigned int uint;
typedef long long ll;
typedef unsigned long long ull;
typedef long double ld;

mt19937 randomer;

struct ExpressionParser {
private:
    string s;
    int now{};
    const string IMPLICATION = "->";
    const string IMPLICATION_ADD = ">-";
    const string OR = "|";
    const string AND = "&";
    const string OPEN = "(";
    const string CLOSE = ")";
    const string COMMA = ",";
    const string EXCLAMATION = "!";
    //гавно из жопы
    string APOSTROPHE = "";

    bool valid() {
        return s[now] != '!' && s[now] != '.' && s[now] != ')' && s[now] != '(' && s[now] != '&' && s[now] != '|' && s[now] != '-';
//        if (s.substr(static_cast<unsigned long>(now), static_cast<unsigned long>(size(APOSTROPHE))) == APOSTROPHE) {
//            ans += s[now++];
//            ans += s[now++];
//            return true;
//        }
//        if ('A' <= s[now]) {
//            if ('0' <= s[now]) {
//                return s[now] <= 'Z' || s[now] <= '9';
//            } else {
//                return s[now] <= 'Z';
//            }
//        } else {
//            return '0' <= s[now] && s[now] <= '9';
//        }
    }

    string parse_single() {
        string ans;
        if (s.substr(static_cast<unsigned long>(now), 1) == OPEN) {
            now++;
            ans = parse_implication();
            now++;
        } else if (s.substr(static_cast<unsigned long>(now), 1) == EXCLAMATION) {
            now++;
            ans = OPEN + EXCLAMATION + parse_single() + CLOSE;
        }
        else
            while (now < size(s) && valid())
                ans += s[now++];
        return ans;
    }

    void fast_add(string &to, const string &fun, const string &what) {
        reverse(all(to));
        to += COMMA;
        to += fun;
        to += OPEN;
        reverse(all(to));
        to += COMMA;
        to += what;
        to += CLOSE;
    }

    string parse_and() {
        string ans = parse_single();
        while (s.substr(static_cast<unsigned long>(now), 1) == AND) {
            now++;
            fast_add(ans, AND, parse_single());
        }
        return ans;
    }

    string parse_or() {
        string ans = parse_and();
        while (s.substr(static_cast<unsigned long>(now), 1) == OR) {
            now++;
            fast_add(ans, OR, parse_and());
        }
        return ans;
    }

    string parse_implication() {
        string ans = parse_or();
        while (s.substr(static_cast<unsigned long>(now), 2) == IMPLICATION) {
            now+=2;
            fast_add(ans, IMPLICATION_ADD, parse_implication());
        }
        return ans;
    }

public:
    ExpressionParser() {
        APOSTROPHE.resize(3);
        APOSTROPHE[0] = char(-30);
        APOSTROPHE[1] = char(-128);
        APOSTROPHE[2] = char(-103);
    }

    string parse(string s) {
        now = 0;
        this->s = move(s);
        return parse_implication();
    }
};

int main() {
//    freopen("input.txt", "r", stdin);
//    freopen("output.txt", "w", stdout);
//    freopen(TASK".in", "r", stdin);
//    freopen(TASK".out", "w", stdout);
    iostream::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    randomer.seed(static_cast<unsigned long>(time(nullptr)));
    string s;
    getline(cin, s);
//    if(s == "P1’->!QQ->!R10&S|!T&U&V") {
//        //return 1;
//        cout << "(->,P1’,(->,(!QQ),(|,(&,(!R10),S),(&,(&,(!T),U),V))))";
//        return 0;
//    }
    ExpressionParser parser;
    cout << parser.parse(move(s));
    return 0;
}

#pragma clang diagnostic pop