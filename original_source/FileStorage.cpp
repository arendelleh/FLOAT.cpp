#include "FileStorage.h"
#include "InputValidation.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <system_error>
#include <utility>
using namespace std;
namespace floatapp {
namespace {
// symlink_status also detects dangling links, which must not count as absent.
bool pathEntryExists(const filesystem::path& path, bool& exists, string& error) {
    error_code ec;
    const auto status=filesystem::symlink_status(path,ec);
    if (ec && ec!=errc::no_such_file_or_directory) {
        error="Cannot inspect "+path.string()+": "+ec.message();
        return false;
    }
    exists=filesystem::exists(status);
    return true;
}
bool endOfRow(istringstream& row) { row>>ws; return row.eof(); }
bool quotedField(istringstream& row, string& value) {
    row>>ws;
    return row.peek()=='"' && static_cast<bool>(row>>quoted(value));
}
bool boolField(istringstream& row, bool& value) {
    string token;
    if (!(row>>token) || (token!="0" && token!="1")) return false;
    value=token=="1";
    return true;
}
bool integerField(istringstream& row, long long& value) {
    string token;
    if (!(row>>token) || token.empty()) return false;
    long long result=0;
    for (unsigned char c:token) {
        if (c<'0' || c>'9' || result>(numeric_limits<long long>::max()-(c-'0'))/10) return false;
        result=result*10+(c-'0');
    }
    value=result;
    return true;
}
bool idField(istringstream& row, int& value) {
    long long parsed=0;
    if (!integerField(row,parsed) || parsed>numeric_limits<int>::max()) return false;
    value=static_cast<int>(parsed);
    return true;
}
}
LoadStatus loadData(const string& path, AppData& data, string& error) {
    error.clear();
    error_code ec;
    bool exists=false;
    if (!pathEntryExists(path,exists,error)) return LoadStatus::Error;
    if (!exists) {
        bool backupExists=false, temporaryExists=false;
        if (!pathEntryExists(path+".bak",backupExists,error) ||
            !pathEntryExists(path+".tmp",temporaryExists,error)) return LoadStatus::Error;
        if (backupExists || temporaryExists) {
            error="Data file is missing but recovery files (.bak or .tmp) exist; inspect and restore the appropriate file before continuing.";
            return LoadStatus::Error;
        }
        return LoadStatus::NotFound;
    }
    if (!filesystem::is_regular_file(path,ec) || ec) { error="Data path is not a readable regular file."; return LoadStatus::Error; }
    ifstream file(path);
    if (!file) { error="Cannot open data file for reading."; return LoadStatus::Error; }
    string line;
    if (!getline(file,line) || (line!="FLOAT 1" && line!="PERAPRUDENT 1")) { error="Missing or unsupported data header."; return LoadStatus::Error; }
    AppData parsed;
    bool savingsSeen=false, finished=false;
    size_t lineNumber=1;
    while (getline(file,line)) {
        ++lineNumber;
        istringstream row(line);
        string tag;
        bool valid=static_cast<bool>(row>>tag);
        if (tag=="INCOME") {
            Income r;
            valid=idField(row,r.id) && quotedField(row,r.source) && quotedField(row,r.date) && quotedField(row,r.description) && integerField(row,r.amountCentavos);
            if (valid) parsed.receivedIncomes.push_back(r);
        } else if (tag=="EXPECTED") {
            ExpectedPayment r;
            valid=idField(row,r.id) && quotedField(row,r.source) && quotedField(row,r.expectedDate) && quotedField(row,r.description) && integerField(row,r.amountCentavos) && boolField(row,r.received);
            if (valid) parsed.expectedPayments.push_back(r);
        } else if (tag=="EXPENSE") {
            Expense r;
            valid=idField(row,r.id) && quotedField(row,r.category) && quotedField(row,r.date) && quotedField(row,r.description) && integerField(row,r.amountCentavos);
            if (valid) parsed.expenses.push_back(r);
        } else if (tag=="BILL") {
            Bill r;
            valid=idField(row,r.id) && quotedField(row,r.title) && quotedField(row,r.dueDate) && integerField(row,r.amountCentavos) && boolField(row,r.paid);
            if (valid) parsed.bills.push_back(r);
        } else if (tag=="SAVINGS") {
            valid=!savingsSeen && integerField(row,parsed.reservedSavingsCentavos);
            savingsSeen=true;
        } else if (tag=="END") {
            long long incomes=0, expected=0, expenses=0, bills=0;
            valid=integerField(row,incomes) && integerField(row,expected) && integerField(row,expenses) && integerField(row,bills);
            valid=valid && static_cast<unsigned long long>(incomes)==parsed.receivedIncomes.size() && static_cast<unsigned long long>(expected)==parsed.expectedPayments.size() && static_cast<unsigned long long>(expenses)==parsed.expenses.size() && static_cast<unsigned long long>(bills)==parsed.bills.size() && savingsSeen;
            finished=valid;
        } else valid=false;
        if (!valid || !endOfRow(row)) { error="Invalid data at line "+to_string(lineNumber)+"."; return LoadStatus::Error; }
        if (finished) break;
    }
    if (!finished) { error="Data file is incomplete (missing END counts or savings)."; return LoadStatus::Error; }
    if (getline(file,line)) { error="Unexpected content after END."; return LoadStatus::Error; }
    if (file.bad() || (!file.eof() && file.fail())) { error="Failed while reading data file."; return LoadStatus::Error; }
    if (!validateData(parsed,error)) return LoadStatus::Error;
    data=move(parsed);
    error.clear();
    return LoadStatus::Loaded;
}
bool saveData(const string& path, const AppData& data, string& error) {
    if (!validateData(data,error)) return false;
    const filesystem::path target(path), temporary(path+".tmp"), backup(path+".bak");
    error_code ec;
    // Check the current on-disk data before opening a temporary file or
    // replacing any backup. No earlier successful load is required.
    AppData existing;
    string loadError;
    const auto currentStatus=loadData(path,existing,loadError);
    if (currentStatus==LoadStatus::Error) {
        error="Cannot save over the current data: "+loadError;
        return false;
    }
    const bool targetExists=currentStatus==LoadStatus::Loaded;
    bool backupExists=false, temporaryExists=false;
    if (!pathEntryExists(backup,backupExists,error) ||
        !pathEntryExists(temporary,temporaryExists,error)) return false;
    if (temporaryExists) {
        error="Temporary path already exists; inspect and remove the .tmp file before saving.";
        return false;
    }
    ofstream file(temporary,ios::trunc);
    if (!file) { error="Cannot open temporary data file for writing."; return false; }
    file<<"FLOAT 1\n";
    for (const auto& r:data.receivedIncomes) file<<"INCOME "<<r.id<<' '<<quoted(r.source)<<' '<<quoted(r.date)<<' '<<quoted(r.description)<<' '<<r.amountCentavos<<'\n';
    for (const auto& r:data.expectedPayments) file<<"EXPECTED "<<r.id<<' '<<quoted(r.source)<<' '<<quoted(r.expectedDate)<<' '<<quoted(r.description)<<' '<<r.amountCentavos<<' '<<(r.received ? 1 : 0)<<'\n';
    for (const auto& r:data.expenses) file<<"EXPENSE "<<r.id<<' '<<quoted(r.category)<<' '<<quoted(r.date)<<' '<<quoted(r.description)<<' '<<r.amountCentavos<<'\n';
    for (const auto& r:data.bills) file<<"BILL "<<r.id<<' '<<quoted(r.title)<<' '<<quoted(r.dueDate)<<' '<<r.amountCentavos<<' '<<(r.paid ? 1 : 0)<<'\n';
    file<<"SAVINGS "<<data.reservedSavingsCentavos<<"\nEND "<<data.receivedIncomes.size()<<' '<<data.expectedPayments.size()<<' '<<data.expenses.size()<<' '<<data.bills.size()<<'\n';
    file.flush();
    bool written=static_cast<bool>(file);
    file.close();
    written=written && !file.fail();
    if (!written) { filesystem::remove(temporary,ec); error="Failed writing or closing temporary data file."; return false; }
    if (targetExists) {
        if (backupExists) {
            filesystem::remove(backup,ec);
            if (ec) { error="Cannot remove old backup: "+ec.message()+". Original data remains; .tmp contains the new data."; return false; }
        }
        filesystem::rename(target,backup,ec);
        if (ec) { error="Cannot back up original data: "+ec.message()+". Original data remains; .tmp contains the new data."; return false; }
    }
    filesystem::rename(temporary,target,ec);
    if (ec) {
        error="Cannot install new data: "+ec.message()+".";
        if (targetExists) {
            filesystem::rename(backup,target,ec);
            error+=ec ? " Rollback failed: "+ec.message()+"; recover the .bak file manually." : " Original data was restored.";
        }
        return false;
    }
    error.clear();
    return true;
}
}
