#define _CRT_SECURE_NO_WARNINGS

#include <thread>
#include <unistd.h>
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
#include <search.h>
#include <unordered_set>
#include <search.h>
#include <climits>
#include <functional>
#include <utility>
#include <string>
#include <cstring>
#include <cassert>
#include <fstream>

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

const string OPEN = "(";
const string CLOSE = ")";
const string IMPLICATION = "->";
const string OR = "|";
const string AND = "&";
const string EXCLAMATION = "!";

struct Expression {
    virtual ~Expression() = default;

    virtual bool is_same(Expression *) = 0;

    virtual string to_string() = 0;
};

int counter = 0;
map<string, pr<int, Expression *> > checker;

bool isLow(char c) {
    return 'a' <= c && c <= 'z';//ochen nadeus, chto askii sovmestno
}

struct Variable : public Expression {
    string var;

    explicit Variable(string _var) : var(std::move(_var)) {}

    bool is_same(Expression *_other) override {
        auto *other = dynamic_cast<Variable *>(_other);
        if (other == nullptr)
            return false;
        if (isLow(other->var[0])) {
            //////////////////////////////////////////////////////////////////////// in fun
            if (checker[other->var].fs != counter) {
                checker[other->var].fs = counter;
                checker[other->var].sc = this;
                return true;
            } else {
                return is_same(checker[other->var].sc);
            }
            //////////////////////////////////////////////////////////////////////// in fun
        }
        return this->var == other->var;
    }

    string to_string() override {
        return var;//maybe here no use of parenthesis, really hope so
    }
};

//glavnoe - ne sravnivat axioma s axioma!
struct Operation {
    string op;

    explicit Operation(string c) : op(std::move(c)) {}
};

struct BinaryOperation : public Expression, public Operation {
    Expression *l, *r;

    BinaryOperation(string c, Expression *_l, Expression *_r) : Operation(std::move(c)), l(_l), r(_r) {}

    bool is_same(Expression *_other) override {
        auto *other = dynamic_cast<BinaryOperation *>(_other);
        if (other == nullptr) {
            auto *axioma = dynamic_cast<Variable *>(_other);
            if (axioma == nullptr || !isLow(axioma->var[0]))
                return false;
            //////////////////////////////////////////////////////////////////////// in fun
            if (checker[axioma->var].fs != counter) {
                checker[axioma->var].fs = counter;
                checker[axioma->var].sc = this;
                return true;
            } else {
                return is_same(checker[axioma->var].sc);
            }
            //////////////////////////////////////////////////////////////////////// in fun
        }
        return this->op == other->op && l->is_same(other->l) && r->is_same(other->r);
    }

    string to_string() override {
        return OPEN + l->to_string() + op + r->to_string() + CLOSE;
    }
};

struct UnaryOperation : public Expression, public Operation {
    Expression *expr;

    UnaryOperation(string c, Expression *_expr) : Operation(std::move(c)), expr(_expr) {}

    bool is_same(Expression *_other) override {
        auto *other = dynamic_cast<UnaryOperation *>(_other);
        if (other == nullptr) {
            auto *axioma = dynamic_cast<Variable *>(_other);
            if (axioma == nullptr || !isLow(axioma->var[0]))
                return false;
            //////////////////////////////////////////////////////////////////////// in fun
            if (checker[axioma->var].fs != counter) {
                checker[axioma->var].fs = counter;
                checker[axioma->var].sc = this;
                return true;
            } else {
                return is_same(checker[axioma->var].sc);
            }
            //////////////////////////////////////////////////////////////////////// in fun
        }
        return this->op == other->op && expr->is_same(other->expr);
    }

    string to_string() override {
        return op + expr->to_string();//maybe here no use of parenthesis, really hope so
    }
};

struct ExpressionParser {
private:
    string s;
    int now{};

    bool valid() {
        return s[now] != '!' && s[now] != '.' && s[now] != ')' && s[now] != '(' && s[now] != '&' && s[now] != '|' &&
               s[now] != '-' && s[now] != ' ';
    }

    void skip() {
        while (s[now] == ' ') now++;
    }

    Expression *parse_single() {
        Expression *ans;
        skip();
        if (s.substr(static_cast<unsigned long>(now), 1) == OPEN) {
            now++;
            skip();
            ans = parse_implication();
            now++;
        } else if (s.substr(static_cast<unsigned long>(now), 1) == EXCLAMATION) {
            now++;
            skip();
            ans = new UnaryOperation(EXCLAMATION, parse_single());
        } else {
            string var;
            skip();
            while (now < size(s) && valid())
                var += s[now++];
            ans = new Variable(var);
        }
        skip();
        return ans;
    }

    Expression *parse_and() {
        skip();
        Expression *ans = parse_single();
        skip();
        while (s.substr(static_cast<unsigned long>(now), 1) == AND) {
            now++;
            skip();
            ans = new BinaryOperation(AND, ans, parse_single());
            skip();
        }
        skip();
        return ans;
    }

    Expression *parse_or() {
        skip();
        Expression *ans = parse_and();
        skip();
        while (s.substr(static_cast<unsigned long>(now), 1) == OR) {
            now++;
            skip();
            ans = new BinaryOperation(OR, ans, parse_and());
            skip();
        }
        skip();
        return ans;
    }

    Expression *parse_implication() {
        skip();
        Expression *ans = parse_or();
        skip();
        while (s.substr(static_cast<unsigned long>(now), 2) == IMPLICATION) {
            now += 2;
            skip();
            ans = new BinaryOperation(IMPLICATION, ans, parse_implication());
            skip();
        }
        skip();
        return ans;
    }

public:
    ExpressionParser() = default;

    Expression *parse(string s) {
        now = 0;
        this->s = std::move(s);
        return parse_implication();
    }
};

struct Info {
    static const char HYPO = 1;
    static const char AXIOMA = 2;
    static const char MP = 3;
    int first;
    string MP_IMPLICATION, MP_BASE;
    int n;
    char used;
    char t;
};

map<string, Info> proved;
vec<pr<int, string> > answer;

void dfs(const string &s) {
    auto &v = proved[s];
    if (v.used)
        return;
    v.used = 1;
    answer.eb(v.first, s);
    if (v.t == Info::MP) {
        dfs(v.MP_BASE);
        dfs(v.MP_IMPLICATION);
    }
}
//1. add axiomas
//2. иду сверху вниз по утверждениям
//3. поддерживаю сет хешей утверждений, что уже доказаны
//3.1. а так же поддерживаю мап из хеша утверждения в его порядковый номер
//3.2. и ещё мап из хеша в строку, которая описывает, что она: номер гипотезы, номер аксиомы, или мп и два хеша
//4. по сету чекаю, доказан ли я
//4.1. если доказан - то иду дальше
//5. если нет - сначала пытаюсь себя как модус поненс
//6. для этого поддерживаю сет хешей утверждений, что доказаны как модус поненс, и к ним держу два хеши утверждений, из которых я доказан
//7. если доказан как мп - просто добавляю в сет доказаных утверждений, мап[утверждание->номер], и удаляю из сета мп, и в закдиываю, что строка мп и два хеша
//7.1. если во мне есть импликация, то ебнуть в мп-мап для того, чтобы использовать себя как мп
//8. если не доказан как мп- пытаюсь доказать себя как аксиома или гипотеза
//9. если получилось - делаю пункт 7, а также нужно подддерживать нечто для понимания, что я доказал
//10. если не получилось - иду дальше
//11. потом чекать, что последнее есть то, что нам нужно и оно доказано
//12. в таком случае восстановить ответ
//13. просто идем рекурсивно от ответа, закидываем в массив пару из номера + хеша. сортируем по номеру. потом перенумеруем номера и понятно короч

///возврат картеки и таб в скип
//я не могу указывать для каждого утверждения его первое вхождение, так как оно может быть не доказано
//я должен указывать для утверждения его первое доказаное вхождение
//как вариант - идти сверху вниз и пытаться доказывать из имеющегося: так мне не нужно будет держать переходы
//для каждого доказаного утверждения, если в нем есть импликация и доказана левая часть, для правой части сразу будем указывать, что она доказана
//тогда когда я приду к утверждению, смотрю, правда ли, что оно доказано как модус понус
//если да - то просто добавляю в доказаные и из него пробую переход по импликации
//иначе - чекаю как аксиому или гипотезу
//важно чекнуть в конце, что последнее утверждение в данном файле есть то, что мы доказываем, а так же, что оно доказано
///можно считать с префаера некий хеш и вместо рекурсивного спуска чекать его
//никогда не запускать аксиому от аксиомы!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

int main() {
//    freopen("input.txt", "r", stdin);
//    freopen("output.txt", "w", stdout);
//    freopen(TASK".in", "r", stdin);
//    freopen(TASK".out", "w", stdout);
    iostream::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    randomer.seed(static_cast<unsigned long>(time(nullptr)));
    ExpressionParser parser;
    string start;
    string line;
    getline(cin, line);
    start = line;
    string hypos_s;
    string statement_s;
    vec<Expression *> hypos;
    forn(i, 0, size(line)) {
        if (line[i] == '|' && line[i + 1] == '-') {
            hypos_s = line.substr(0, i);
            statement_s = line.substr(i + 2);
            break;
        }
    }
    hypos_s.push_back(',');
    int last = 0;
    for (int i = 0; i < size(hypos_s); i++) {
        if (hypos_s[i] == ',') {
            hypos.eb(parser.parse(hypos_s.substr(last, i - last)));
            last = i + 1;
        }
    }
    Expression *statement = parser.parse(statement_s);
    vec<Expression *> axiomas(10);
    /*
    (a)->((b)->(a))
    ((a)->(b))->((a)->(b)->(c))->((a)->(c))
    (a)->(b)->(a)&(b)
    (a)&(b)->(a)
    (a)&(b)->(b)
    (a)->(a)|(b)
    (b)->(a)|(b)
    ((a)->(c))->((b)->(c))->((a)|(b)->(c))
    ((a)->(b))->((a)->!(b))->!(a)
    !!(a)->(a)
     */
    vec<string> axis(10);
    axis[0] = "(a)->((b)->(a))";
    axis[1] = "((a)->(b))->((a)->(b)->(c))->((a)->(c))";
    axis[2] = "(a)->(b)->(a)&(b)";
    axis[3] = "(a)&(b)->(a)";
    axis[4] = "(a)&(b)->(b)";
    axis[5] = "(a)->(a)|(b)";
    axis[6] = "(b)->(a)|(b)";
    axis[7] = "((a)->(c))->((b)->(c))->((a)|(b)->(c))";
    axis[8] = "((a)->(b))->((a)->!(b))->!(a)";
    axis[9] = "!!(a)->(a)";
    forn(i, 0, 10)axiomas[i] = parser.parse(axis[i]);
//    forn(i, 0, 10) {
//        forn(j, 0, 10)
//            cout << axiomas[i]->is_same(axiomas[j]);
//        cout << endl;
//    }
    checker["a"] = {0, nullptr};
    checker["b"] = {0, nullptr};
    checker["c"] = {0, nullptr};
    vec<Expression *> sentences;
    map<string, vec<BinaryOperation *> > implications;
    int sz = 1;
    while (getline(cin, line)) {
        auto *sentence = parser.parse(line);
        line = sentence->to_string();
        sentences.eb(sentence);
        if (proved.count(line) == 0) {
            bool done = false;
            if (implications.count(line)) {
                foraa(i, implications[line])
                    if (proved.count(i->l->to_string()) && proved.count(i->to_string())) {
                        proved[line].t = Info::MP;
                        proved[line].first = sz++;
                        proved[line].MP_IMPLICATION = i->to_string();
                        proved[line].MP_BASE = i->l->to_string();
                        done = true;
                        break;
                    }
                if (done)
                    implications.erase(line);
            }
            if (!done) {
                forn(i, 0, size(hypos)) {
                    counter++;
                    if (sentence->is_same(hypos[i])) {
                        proved[line].t = Info::HYPO;
                        proved[line].first = sz++;
                        proved[line].n = i;
                        done = true;
                        break;
                    }
                }
                if (!done)
                    forn(i, 0, size(axiomas)) {
                        counter++;
                        if (sentence->is_same(axiomas[i])) {
                            proved[line].t = Info::AXIOMA;
                            proved[line].first = sz++;
                            proved[line].n = i;
                            done = true;
                            break;
                        }
                    }
            }
            if (done) {
                auto *now = dynamic_cast<BinaryOperation *>(sentence);
                if (now != nullptr && now->op == IMPLICATION) {
                    implications[now->r->to_string()].eb(now);
                }
            } else {
                cout << "Proof is incorrect";
                return 0;
            }
        }
    }
    counter++;
    if (!sentences.back()->is_same(statement)) {
        cout << "Proof is incorrect";
        return 0;
    }
//    cout << "correct" << endl;
//    sleep(100);
    /*
     * 1. скачать хотспот
     * 2. написать ответ
     * 3. проверить на данных мне тестах
     * 4. узкие места и послать
     * */
    forn(i,0,size(hypos)) {
        cout << hypos[i]->to_string();
        if(i != size(hypos) - 1)
            cout << ", ";
        else
            cout << " |- ";
    }
    cout << statement->to_string() << '\n';
    dfs(statement->to_string());
    sort(all(answer));//правда ли, что юзед правильно, и не нужно юник
    map<string, int> mapper;
    forn(i,0,size(answer)){
        mapper[answer[i].sc] = i + 1;
    }
    forn(i,0,size(answer)){
        cout << "[" << i + 1 << ". ";
        if(proved[answer[i].sc].t == Info::AXIOMA){
            cout << "Ax. sch. " << proved[answer[i].sc].n + 1 << "] ";
        }
        else if(proved[answer[i].sc].t == Info::HYPO){
            cout << "Hypothesis " << proved[answer[i].sc].n + 1 << "] ";
        }
        else{
            cout << "M.P. " << mapper[proved[answer[i].sc].MP_IMPLICATION] << ", "
            << mapper[proved[answer[i].sc].MP_BASE] << "] ";
        }
        cout << answer[i].sc << '\n';
    }
//    forn(i, 0, size(sentences)) {
//        cout << sentences[i]->to_string() << " - ";
//        cout << " hypos: ";
//        forn(j,0,size(hypos)){
//            counter++;
//            if(sentences[i]->is_same(hypos[j]))
//                cout << hypos[j]->to_string() << ' ';
//        }
//        cout << " ||| axiomas: ";
//        forn(j,0,size(axiomas)) {
//            counter++;
//            if (sentences[i]->is_same(axiomas[j]))
//                cout << axiomas[j]->to_string() << ' ';
//        }
//        cout << endl;
//    }
//    forn(i, 0, size(sentences)) {
//        forn(j, 0, 10) {
//            counter++;
//            if (sentences[i]->is_same(axiomas[j])) {
//                cout << j << ' ';
//            }
//        }
//        cout << endl;
//    }
//    cout << "hypos" << endl;
//    forn(i,0,size(hypos))
//        cout << hypos[i]->to_string() << endl;
//    cout << "statement" << endl;
//    cout << statement->to_string() << endl;
//    cout << "sentences" << endl;
//    forn(i,0,size(sentences))
//        cout << sentences[i]->to_string() << endl;
//    cout << "axiomas" << endl;
//    forn(i,0,10)
//        cout << axiomas[i]->to_string() << endl;
    return 0;
}