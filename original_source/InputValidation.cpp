#include "InputValidation.h"
#include <algorithm>
#include <climits>
#include <istream>
#include <limits>
#include <ostream>
#include <set>
using namespace std;
namespace floatapp {
namespace {
bool digits(const string& s, unsigned long long limit, unsigned long long& result) {
    if (s.empty()) return false;
    unsigned long long n=0;
    for (unsigned char c : s) {
        if (c<'0' || c>'9' || n>(limit-(c-'0'))/10) return false;
        n=n*10+(c-'0');
    }
    result=n;
    return true;
}
bool cleanText(const string& s, size_t limit, bool required) {
    if (s.size()>limit) return false;
    bool nonblank=false;
    for (unsigned char c : s) {
        if (c<32 || c==127) return false;
        if (c!=' ') nonblank=true;
    }
    return !required || nonblank;
}
}
bool parseMoney(const string& text, long long& out) {
    const size_t dot=text.find('.');
    const string whole=text.substr(0,dot);
    string fraction=dot==string::npos ? "" : text.substr(dot+1);
    if (dot!=string::npos && (fraction.empty() || fraction.size()>2)) return false;
    unsigned long long units=0, cents=0;
    const auto limit=static_cast<unsigned long long>(numeric_limits<long long>::max());
    if (!digits(whole,limit/100,units)) return false;
    if (!fraction.empty()) {
        if (!digits(fraction,99,cents)) return false;
        if (fraction.size()==1) cents*=10;
    }
    if (units>(limit-cents)/100) return false;
    out=static_cast<long long>(units*100+cents);
    return true;
}
bool parseInteger(const string& text, int& out, int minimum, int maximum) {
    if (minimum>maximum || text.empty()) return false;
    bool negative=text[0]=='-';
    unsigned long long n=0;
    const auto limit=negative ? static_cast<unsigned long long>(INT_MAX)+1 : static_cast<unsigned long long>(INT_MAX);
    if (!digits(text.substr(negative ? 1 : 0),limit,n)) return false;
    const long long value=negative ? -static_cast<long long>(n) : static_cast<long long>(n);
    if (value<minimum || value>maximum) return false;
    out=static_cast<int>(value);
    return true;
}
bool isValidDate(const string& s) {
    if (s.size()!=10 || s[4]!='-' || s[7]!='-') return false;
    unsigned long long y=0,m=0,d=0;
    if (!digits(s.substr(0,4),9999,y) || !digits(s.substr(5,2),99,m) || !digits(s.substr(8,2),99,d)) return false;
    if (y==0 || m<1 || m>12) return false;
    const int days[]={31,28,31,30,31,30,31,31,30,31,30,31};
    int maximum=days[m-1];
    if (m==2 && (y%400==0 || (y%4==0 && y%100!=0))) ++maximum;
    return d>=1 && d<=static_cast<unsigned long long>(maximum);
}
bool isValidText(const string& text, size_t maxLength) { return cleanText(text,maxLength,true); }
bool validateData(const AppData& data, string& error) {
    error.clear();
    if (data.reservedSavingsCentavos<0) { error="Reserved savings cannot be negative."; return false; }
    set<int> ids;
    auto check=[&](int id, const string& label, const string& date, const string& description, long long amount) {
        if (id<=0 || id>INT_MAX-1 || !ids.insert(id).second) error="Record IDs must be positive, unique across all records, and at most INT_MAX-1.";
        else if (!isValidText(label)) error="A required record text is blank, too long, or contains a control character.";
        else if (!isValidDate(date)) error="A record date is invalid; use YYYY-MM-DD.";
        else if (!cleanText(description,200,false)) error="A description is too long or contains a control character.";
        else if (amount<=0) error="Record amounts must be positive centavos.";
        return error.empty();
    };
    for (const auto& r:data.receivedIncomes) if (!check(r.id,r.source,r.date,r.description,r.amountCentavos)) return false;
    for (const auto& r:data.expectedPayments) if (!check(r.id,r.source,r.expectedDate,r.description,r.amountCentavos)) return false;
    for (const auto& r:data.expenses) if (!check(r.id,r.category,r.date,r.description,r.amountCentavos)) return false;
    for (const auto& r:data.bills) if (!check(r.id,r.title,r.dueDate,"",r.amountCentavos)) return false;
    return true;
}
bool nextRecordId(const AppData& data, int& out, string& error) {
    if (!validateData(data,error)) return false;
    int maximum=0;
    for (const auto& r:data.receivedIncomes) maximum=max(maximum,r.id);
    for (const auto& r:data.expectedPayments) maximum=max(maximum,r.id);
    for (const auto& r:data.expenses) maximum=max(maximum,r.id);
    for (const auto& r:data.bills) maximum=max(maximum,r.id);
    if (maximum>=INT_MAX-1) { error="No further record ID is available."; return false; }
    out=maximum+1;
    return true;
}
bool readMenuChoice(istream& in, ostream& out, int minimum, int maximum, int& value) {
    if (minimum>maximum) return false;
    string line;
    while (getline(in,line)) {
        if (parseInteger(line,value,minimum,maximum)) return true;
        out<<"Enter a whole number from "<<minimum<<" to "<<maximum<<": ";
    }
    return false;
}
bool readMoney(istream& in, ostream& out, long long& value, bool allowZero) {
    string line; long long parsed=0;
    while (getline(in,line)) {
        if (parseMoney(line,parsed) && (allowZero || parsed>0)) { value=parsed; return true; }
        out<<"Enter "<<(allowZero ? "a nonnegative" : "a positive")<<" amount with at most two decimal places: ";
    }
    return false;
}
bool readDate(istream& in, ostream& out, string& value) {
    string line;
    while (getline(in,line)) {
        if (isValidDate(line)) { value=line; return true; }
        out<<"Enter a valid date (YYYY-MM-DD): ";
    }
    return false;
}
bool readRequiredText(istream& in, ostream& out, string& value) {
    string line;
    while (getline(in,line)) {
        if (isValidText(line)) { value=line; return true; }
        out<<"Enter nonblank text of at most 200 bytes without control characters: ";
    }
    return false;
}
}
