#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <thread>
#include <chrono>
#include <stdexcept>
using namespace std;
// ANSI COLOR CODES
namespace C {
    const string R      = "\033[0m";
    const string BLD    = "\033[1m";
    const string WHT    = "\033[97m";
    const string CYN    = "\033[96m";
    const string YEL    = "\033[93m";
    const string GRN    = "\033[92m";
    const string RED    = "\033[91m";
    const string MAG    = "\033[95m";
    const string BLU    = "\033[94m";
    const string BG_BLU = "\033[44m";
    const string BG_GRN = "\033[42m";
    const string BG_RED = "\033[41m";
    const string BG_BLK = "\033[40m";
    const string BLK    = "\033[30m";
}
const int W = 52;
void header(const string& title, const string& bg = C::BG_BLU) {
    cout << "\n  " << bg << C::BLD << C::WHT
         << " " << left << setw(W) << title << " "
         << C::R << "\n";
}
void bannerRow(const string& text, const string& bg = C::BG_BLU) {
    int pad = (W - (int)text.size()) / 2;
    string centered(pad, ' ');
    centered += text;
    cout << "  " << bg << C::BLD << C::WHT
         << " " << left << setw(W) << centered << " "
         << C::R << "\n";
}

void field(const string& label, const string& col, const string& val) {
    cout << C::CYN << "  " << left << setw(14) << label
         << ": " << col << C::BLD << val << C::R << "\n";
}
void ok(const string& msg) {
    cout << "  " << C::BG_GRN << C::WHT << C::BLD << "  [OK] " << msg << "  " << C::R << "\n\n";
}
void err(const string& msg) {
    cout << "  " << C::BG_RED << C::WHT << C::BLD << " [!] " << msg << " " << C::R << "\n";
}

//STATIC MEMBER
class AppConfig {
public:
    static int    totalObjects;
    static string version;
    static string name;
};
int    AppConfig::totalObjects = 0;
string AppConfig::version      = "2.0";
string AppConfig::name         = "DIGONTO";

//TEMPLATE
template <typename T>
class ResultWrapper {
public:
    T data; int rank;
    ResultWrapper(T d, int r = 1) : data(d), rank(r) { AppConfig::totalObjects++; }

    //Operator overloading
    friend ostream& operator<<(ostream& os, const ResultWrapper<T>& rw) {
        os << "  " << C::BG_BLK << C::CYN << C::BLD << " [" << rw.rank << "] "
           << C::R << "  " << C::WHT << rw.data << C::R;
        return os;
    }
    bool operator>(const ResultWrapper<T>& o) const { return rank > o.rank; }
};
//ABSTRACT CLASS — ISearchCore
class ISearchCore {
public:
    virtual void execute(string query) = 0;  //Virtual Function
    virtual ~ISearchCore() {}
};
//CUSTOM EXCEPTIONS

class FileException  : public runtime_error  {
public: FileException(const string& m)  : runtime_error("File Error: " + m) {} };
class InputException : public invalid_argument {
public: InputException(const string& m) : invalid_argument("Input Error: " + m) {} };
//Logger (saves/shows search history)
//Class & Objects, Encapsulation
class Logger {
private:
    string logFile;
    string timestamp() {
        time_t t = time(0); tm* n = localtime(&t);
        ostringstream os;
        os << "[" << (n->tm_year+1900) << "-"
           << setw(2) << setfill('0') << (n->tm_mon+1) << "-"
           << setw(2) << setfill('0') << n->tm_mday << " "
           << setw(2) << setfill('0') << n->tm_hour << ":"
           << setw(2) << setfill('0') << n->tm_min  << "]";
        return os.str();
    }
public:
    Logger() : logFile("search_log.txt") { AppConfig::totalObjects++; }      // Constructors
    Logger(string fn) : logFile(fn)      { AppConfig::totalObjects++; }      // Constructor Overloading
    ~Logger() {}

    void log(const string& q) {
        try {
            ofstream f(logFile, ios::app);
            if (!f.is_open()) throw FileException("Cannot open " + logFile);
            f << timestamp() << " " << q << "\n";
        } catch (const FileException& e) { cerr << C::YEL << "  [WARN] " << e.what() << C::R << "\n"; }
    }

    void showHistory(int n = 10) {
        try {
            ifstream f(logFile);
            if (!f.is_open()) throw FileException("No history file.");
            vector<string> lines;
            string line;
            while (getline(f, line)) lines.push_back(line);
            header("  SEARCH HISTORY");
            int start = max(0, (int)lines.size() - n);
            if (lines.empty()) cout << C::YEL << "  (No history yet)\n" << C::R;
            else for (int i = start; i < (int)lines.size(); i++)
                cout << "  " << C::BG_BLK << C::CYN << " >> " << C::R << "  " << lines[i] << "\n";
            cout << "\n";
        } catch (const FileException& e) { cout << C::YEL << "  " << e.what() << C::R << "\n"; }
    }
};
//WeatherModule (Class & Objects + Encapsulation)
class WeatherModule {
private:
    string conds[7] = {"Sunny","Cloudy","Rainy","Stormy","Hazy","Windy","Foggy"};
    string cols[7]  = {C::YEL, C::WHT, C::BLU, C::RED, C::MAG, C::CYN, C::WHT};
    string icons[7] = {"[SUNNY]","[CLOUDY]","[RAINY]","[STORMY]","[HAZY]","[WINDY]","[FOGGY]"};
public:
    WeatherModule() { AppConfig::totalObjects++; }
    ~WeatherModule() {}

    //Function overloading
    void show(string city) {
        if (city.empty()) city = "Khulna";
        else city[0] = toupper(city[0]);
        int temp = rand()%15+20, idx = rand()%7, hum = rand()%50+30;
        int wind = rand()%30+5,  uv  = rand()%11, vis = rand()%10+1;
        header("  LIVE WEATHER  -  " + city);
        field("Temperature", C::RED, to_string(temp) + " C");
        cout << C::CYN << "  " << left << setw(14) << "Condition"
     << ": " << cols[idx] << C::BLD << conds[idx] << C::R << "\n";
        field("Humidity",   C::GRN, to_string(hum)  + "%");
        field("Wind Speed", C::YEL, to_string(wind) + " km/h");
        field("UV Index",   C::MAG, to_string(uv)   + "/10");
        field("Visibility", C::WHT, to_string(vis)  + " km");
        cout << "\n";
    }
    void show(string city, char unit) {                   //Function Overloaded
        show(city);
        int temp = rand()%15+20;
        if (unit=='F'||unit=='f')
            field("Fahrenheit", C::RED, to_string(temp*9/5+32) + " F");
    }
};
//TimerModule (Class & Objects + Encapsulation)
class TimerModule {
public:
    TimerModule() { AppConfig::totalObjects++; }
    ~TimerModule() {}

    void startTimer(int sec) {
        try {
            if (sec <= 0)   throw InputException("Timer must be > 0 seconds.");
            if (sec > 3600) throw InputException("Timer cannot exceed 3600 seconds.");
            header("  COUNTDOWN TIMER");
            cout << "  " << C::GRN << C::BLD << "Started for " << C::WHT << sec << C::GRN << " second(s)...\n" << C::R;
            for (int i = sec; i > 0; --i) {
                string tc = (i > sec*0.6) ? C::GRN : (i > sec*0.3) ? C::YEL : C::RED;
                cout << "\r  " << tc << C::BLD << "Time remaining: " << C::WHT << setw(4) << i << "s  " << C::R << flush;
                this_thread::sleep_for(chrono::seconds(1));
            }
            cout << "\r  " << C::BG_BLU << C::WHT << C::BLD << "  [BEEP!] Time's up!                          " << C::R << "\n\n";
        } catch (const InputException& e) { err(e.what()); }
    }
};
//CalendarModule (Class & Objects + Encapsulation)
class CalendarModule {
private:
    string months[13] = {"","January","February","March","April","May","June",
                         "July","August","September","October","November","December"};
    string days[7]    = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

    int firstDayOf(int y, int m) {
        static int t[] = {0,3,2,5,0,3,5,1,4,6,2,4};
        if (m < 3) y--;
        return (y + y/4 - y/100 + y/400 + t[m-1] + 1) % 7;
    }
public:
    CalendarModule() { AppConfig::totalObjects++; }
    ~CalendarModule() {}

    void showMonth(int year, int month) {
        try {
            if (month<1||month>12)       throw InputException("Month must be 1-12.");
            if (year<1900||year>2100)    throw InputException("Year must be 1900-2100.");
            int dim[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
            if ((year%4==0&&year%100!=0)||year%400==0) dim[2]=29;
            header("  " + months[month] + " " + to_string(year));
            string hdrs[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
            cout << "  ";
            for (int i=0;i<7;i++) {
                string hc = (i==0)?C::RED:(i==6)?C::YEL:C::CYN;
                cout << hc << C::BLD << setw(4) << hdrs[i] << C::R;
            }
            cout << "\n  ";
            int fd = firstDayOf(year, month);
            for (int i=0;i<fd;i++) cout << "    ";
            for (int d=1;d<=dim[month];d++) {
                int col = (d-1+fd)%7;
                string dc = (col==0)?C::RED:(col==6)?C::YEL:C::WHT;
                cout << dc << setw(4) << d << C::R;
                if (col==6&&d!=dim[month]) cout << "\n  ";
            }
            cout << "\n\n";
        } catch (const InputException& e) { err(e.what()); }
    }

    void showToday() {
        time_t t = time(0); tm* n = localtime(&t);
        ostringstream ds, ts;
        ds << n->tm_mday << "/" << (n->tm_mon+1) << "/" << (n->tm_year+1900);
        ts << setw(2)<<setfill('0')<<n->tm_hour<<":"
           << setw(2)<<setfill('0')<<n->tm_min <<":"
           << setw(2)<<setfill('0')<<n->tm_sec;
        header("  TODAY'S DATE & TIME");
        field("Day",  C::YEL, days[n->tm_wday]);
        field("Date", C::WHT, ds.str());
        field("Time", C::GRN, ts.str());
        cout << "\n";
    }
};
//CalculatorModule (Class & Objects + Encapsulation)
class CalculatorModule {
public:
    CalculatorModule() { AppConfig::totalObjects++; }
    ~CalculatorModule() {}

    void run() {
        header("  CALCULATOR   (e.g.  12 + 7)");
        cout << "  " << C::CYN << "Expression: " << C::WHT;
        string line; getline(cin, line); cout << C::R;
        try {
            double a, b; char op;
            istringstream iss(line);
            if (!(iss >> a >> op >> b)) throw InputException("Use format: num op num");
            double result = 0;
            if      (op=='+') result = a+b;
            else if (op=='-') result = a-b;
            else if (op=='*') result = a*b;
            else if (op=='/') { if (b==0) throw InputException("Division by zero!"); result = a/b; }
            else if (op=='%') { if ((int)b==0) throw InputException("Modulo by zero!"); result=(int)a%(int)b; }
            else throw InputException("Unknown operator. Use + - * / %");
            cout << "\n  " << C::BG_BLK << C::GRN << C::BLD
                 << "  Result: " << C::WHT << a << " " << op << " " << b << " = " << result
                 << "  " << C::R << "\n\n";
        } catch (const InputException& e) { err(e.what()); }
    }
};
//NotepadModule (Class & Objects + Encapsulation)
class NotepadModule {
private:
    vector<string> notes;    //STL vector
    string noteFile;
    void loadNotes() {
        notes.clear(); ifstream f(noteFile); if (!f.is_open()) return;
        string line; while (getline(f,line)) notes.push_back(line);
    }
public:
    NotepadModule() : noteFile("notes.txt") { AppConfig::totalObjects++; loadNotes(); }
    ~NotepadModule() {}

    void addNote(const string& text) {
        try {
            if (text.empty()) throw InputException("Note cannot be empty.");
            time_t t = time(0); tm* n = localtime(&t);
            ostringstream ts;
            ts << "[" << (n->tm_year+1900) << "-"
               << setw(2)<<setfill('0')<<(n->tm_mon+1) << "-"
               << setw(2)<<setfill('0')<<n->tm_mday << "] ";
            string entry = ts.str() + text;
            notes.push_back(entry);
            ofstream f(noteFile, ios::app);
            if (!f.is_open()) throw FileException("Cannot save note.");
            f << entry << "\n";
            ok("Note saved!");
        } catch (const exception& e) { err(e.what()); }
    }

    void showNotes() {
        header("  MY NOTES");
        if (notes.empty()) cout << "  " << C::YEL << "(No notes yet. Use: note add <text>)\n" << C::R;
        else for (int i=0;i<(int)notes.size();i++)
            cout << "  " << C::BG_BLK << C::CYN << C::BLD << " [" << (i+1) << "] "
                 << C::R << "  " << notes[i] << "\n";
        cout << "\n";
    }
};
//FileHandler — database load/save (Class & Objects + Encapsulation + File Handling)
class FileHandler {
private:
    string filename;
public:
    FileHandler() : filename("input.txt") { AppConfig::totalObjects++; }          // Constructor
    FileHandler(string fn) : filename(fn) { AppConfig::totalObjects++; }          // Constructor Overloading
    ~FileHandler() {}

    void loadData(map<string,set<int>>& index, vector<string>& storage) {
        try {
            index.clear(); storage.clear();
            ifstream file(filename);
            if (!file.is_open()) throw FileException("Cannot open " + filename);
            string line; int id = 0;
            while (getline(file, line)) {
                size_t pipe = line.find('|');
                if (pipe == string::npos) continue;
                storage.push_back(line.substr(pipe+1));
                stringstream ss(line); string word;
                while (ss >> word) {
                    size_t p = word.find('|');
                    if (p!=string::npos) word = word.substr(0,p);
                    word.erase(remove_if(word.begin(),word.end(),::ispunct),word.end());
                    transform(word.begin(),word.end(),word.begin(),::tolower);
                    if (!word.empty()) index[word].insert(id);
                }
                id++;
            }
        } catch (const FileException& e) { cerr << C::YEL << "  [WARN] " << e.what() << C::R << "\n"; }
    }

    void addEntry(const string& title, const string& desc) {
        try {
            ofstream file(filename, ios::app);
            if (!file.is_open()) throw FileException("Cannot open " + filename);
            file << "\n" << title << "|" << desc;
            ok("Entry added!");
        } catch (const FileException& e) { err(e.what()); }
    }

    void exportResults(const string& query, vector<string>& results) {
        try {
            ofstream file("results.txt", ios::app);
            if (!file.is_open()) throw FileException("Cannot write results.txt");
            file << "Query: " << query << "\n";
            for (auto& r : results) file << "  - " << r << "\n";
            file << "\n";
            ok("Exported to results.txt");
        } catch (const FileException& e) { err(e.what()); }
    }
};
//SINGLE INHERITANCE
class DataManager : public FileHandler {
protected:
    map<string,set<int>> keywordMap;   //STL map + set
    vector<string>       vault;         //STL vector
public:
    DataManager() : FileHandler("input.txt") { AppConfig::totalObjects++; }
    ~DataManager() {}
    void sync()          { loadData(keywordMap, vault); }
    int  totalRecords()  { return (int)vault.size(); }
};
//SearchStats — tracks search success/fail counts (Class & Objects + Encapsulation)
class SearchStats {
protected:
    int searchCount, successCount, failCount;
public:
    static int globalSearches;   //Static — shared across all instances
    SearchStats() : searchCount(0), successCount(0), failCount(0) { AppConfig::totalObjects++; }
    ~SearchStats() {}
    void recordSuccess() { searchCount++; successCount++; globalSearches++; }
    void recordFail()    { searchCount++; failCount++;    globalSearches++; }

    //Operator overloading
    friend ostream& operator<<(ostream& os, const SearchStats& s) {
        os << C::BG_BLU << C::BLD << C::WHT << " SESSION STATS" << string(W-13,' ') << " " << C::R << "\n"
           << C::CYN << "  Session Searches : " << C::WHT << C::BLD << s.searchCount  << C::R << "\n"
           << C::GRN << "  Successful       : " << C::WHT << C::BLD << s.successCount << C::R << "\n"
           << C::RED << "  Not Found        : " << C::WHT << C::BLD << s.failCount    << C::R << "\n"
           << C::YEL << "  All-Time Total   : " << C::WHT << C::BLD << globalSearches << C::R << "\n\n";
        return os;
    }
};
int SearchStats::globalSearches = 0;
//MULTIPLE INHERITANCE — SearchCore : DataManager + SearchStats
class SearchCore : public DataManager, public SearchStats {
protected:
    Logger logger;
public:
    SearchCore() : DataManager(), SearchStats(), logger("search_log.txt") { AppConfig::totalObjects++; }
    ~SearchCore() {}

    void suggest(const string& partial) {
        string low = partial;
        transform(low.begin(), low.end(), low.begin(), ::tolower);
        set<string> sugg;
        for (auto& [word, ids] : keywordMap)
            if (word.size()>=low.size() && word.substr(0,low.size())==low)
                sugg.insert(word);
        if (!sugg.empty()) {
            cout << "  " << C::YEL << C::BLD << "Did you mean: " << C::R;
            int cnt=0;
            for (const string& s : sugg) {
                cout << C::BG_BLK << C::CYN << C::BLD << " [" << s << "] " << C::R << " ";
                if (++cnt>=5) break;
            }
            cout << "\n";
        }
    }

    vector<string> doSearch(const string& query) {
        stringstream ss(query); string word; vector<string> terms;
        while (ss >> word) {
            transform(word.begin(),word.end(),word.begin(),::tolower);
            terms.push_back(word);
        }
        set<int> resultSet; bool first = true;
        for (const string& t : terms) {
            if (keywordMap.count(t)) {
                if (first) { resultSet = keywordMap[t]; first = false; }
                else {
                    set<int> inter;
                    set_intersection(resultSet.begin(),resultSet.end(),
                                     keywordMap[t].begin(),keywordMap[t].end(),
                                     inserter(inter,inter.begin()));
                    resultSet = inter;
                }
            } else suggest(t);
        }
        vector<string> results;
        for (int idx : resultSet) results.push_back(vault[idx]);
        return results;
    }
};
// HYBRID INHERITANCE — SearchEngine : SearchCore + ISearchCore
class SearchEngine : public SearchCore, public ISearchCore {
    WeatherModule    weatherApp;
    CalendarModule   calApp;
    TimerModule      timerApp;
    CalculatorModule calcApp;
    NotepadModule    noteApp;
    vector<string>   recentSearches;   //STL vector
    string*          lastQuery;        //Dynamic memory

public:
    SearchEngine() : SearchCore(), lastQuery(new string("")) { AppConfig::totalObjects++; }    //Constructor
    SearchEngine(string logFile) : SearchCore(), lastQuery(new string("")) {                   //Constructor Overloading
        logger = Logger(logFile); AppConfig::totalObjects++;
    }
    ~SearchEngine() {                                                                          //Destructor
        delete lastQuery;
        cout << "\n  " << C::MAG << C::BLD << "Goodbye! DIGONTO shutdown complete.\n" << C::R;
        cout << *this;
    }

    SearchEngine* getThis() { return this; }   //this Pointer

    void printWelcome() {
        string blank(W, ' ');
        string line1 = "D I G O N T O";
        string line2 = "Intelligent Search & Utility Engine";

        auto center = [&](const string& s) {
            int pad = (W - (int)s.size()) / 2;
            return string(pad, ' ') + s + string(W - pad - (int)s.size(), ' ');
        };

        cout << "\n";
        cout << "  " << C::BG_BLU << C::BLD << C::WHT << " " << blank                << " " << C::R << "\n";
        cout << "  " << C::BG_BLU << C::BLD << C::WHT << " " << center(line1)         << " " << C::R << "\n";
        cout << "  " << C::BG_BLU << C::BLD << C::CYN << " " << center(line2)         << " " << C::R << "\n";
        cout << "  " << C::BG_BLU << C::BLD << C::WHT << " " << blank                << " " << C::R << "\n";
        cout << "  " << C::BG_BLU << C::BLD << C::WHT << " " << blank                << " " << C::R << "\n\n";
        cout << "  " << C::BG_BLU << C::BLD << C::WHT
             << " " << left << setw(W) << " COMMAND              DESCRIPTION" << " " << C::R << "\n";

        auto row = [](const string& cmd, const string& desc) {
            cout << "  " << C::YEL << C::BLD << left << setw(20) << cmd
                 << C::R << "  " << C::WHT << desc << C::R << "\n";
        };
        row("[keyword]",       "Search the knowledge base");
        row("weather [city]",  "Live weather (default: Khulna)");
        row("cal",             "Show today's date and time");
        row("cal YYYY MM",     "Show a monthly calendar");
        row("timer [sec]",     "Start a countdown timer");
        row("calc",            "Calculator  (e.g. 5 + 3)");
        row("note add <text>", "Save a quick note");
        row("note list",       "View all saved notes");
        row("add",             "Add new entry to database");
        row("history",         "View recent search history");
        row("stats",           "Show session statistics");
        row("export",          "Export last results to file");
        row("help",            "Show this help menu");
        row("exit",            "Exit DIGONTO");
        cout << "\n";
    }

    //POLYMORPHISM — override abstract execute()+ Virtual Function
    void execute(string query) override {
        replace(query.begin(),query.end(),'[',' ');
        replace(query.begin(),query.end(),']',' ');
        stringstream ss(query); string cmd; ss >> cmd;
        transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);
        *lastQuery = query;

        if (cmd=="weather") {
            string city; getline(ss>>ws, city);
            weatherApp.show(city);
        }
        else if (cmd=="timer") {
            int sec=0;
            if (ss>>sec) timerApp.startTimer(sec);
            else err("Usage: timer 10");
        }
        else if (cmd=="calc"||cmd=="calculator") {
            calcApp.run();
        }
        else if (cmd=="note") {
            string sub; ss>>sub;
            transform(sub.begin(),sub.end(),sub.begin(),::tolower);
            if (sub=="list") noteApp.showNotes();
            else if (sub=="add") {
                string text; getline(ss>>ws, text);
                if (text.empty()) {
                    cout << "  " << C::CYN << "Enter note: " << C::WHT;
                    getline(cin, text); cout << C::R;
                }
                noteApp.addNote(text);
            } else cout << "  " << C::YEL << "Usage: note add <text>  OR  note list\n" << C::R;
        }
        else if (cmd=="cal"||cmd=="calendar") {
            int y,m;
            if (ss>>y>>m) calApp.showMonth(y,m);
            else          calApp.showToday();
        }
        else if (cmd=="stats")   { cout << "\n"; cout << *this; }
        else if (cmd=="history") { logger.showHistory(10); }
        else if (cmd=="help")    { printWelcome(); }
        else if (cmd=="export")  {
            if (!recentSearches.empty()) {
                vector<string> res = doSearch(recentSearches.back());
                exportResults(recentSearches.back(), res);
            } else cout << "  " << C::YEL << "[!] No search to export yet.\n" << C::R;
        }
        else {
            logger.log(query);
            recentSearches.push_back(query);
            vector<string> results = doSearch(query);

            if (!results.empty()) {
                header("  SEARCH RESULTS");
                cout << C::GRN << C::BLD << "  Found " << C::WHT << results.size()
                     << C::GRN << " result(s):\n" << C::R;
                for (int i=0;i<(int)results.size();i++) {
                    ResultWrapper<string> rw(results[i], i+1);
                    cout << rw << "\n";
                }
                cout << "\n";
                recordSuccess();
            } else {
                cout << "\n  " << C::BG_RED << C::WHT << C::BLD
                     << " No results for '" << query << "'. " << C::R << "\n";
                cout << "  " << C::YEL << "Add it to the database? (yes/no): " << C::WHT;
                string choice; getline(cin, choice); cout << C::R;
                transform(choice.begin(),choice.end(),choice.begin(),::tolower);
                if (choice=="yes"||choice=="y") {
                    string desc;
                    cout << "  " << C::CYN << "Description for '" << C::WHT << query << C::CYN << "': " << C::WHT;
                    getline(cin, desc); cout << C::R;
                    addEntry(query, desc);
                    this->sync();
                }
                recordFail();
            }
        }
    }

    void start() {
        srand(time(0));
        this->sync();
        printWelcome();
        string input;
        while (true) {
            cout << "\n  " << C::MAG << C::BLD << "Search" << C::R << C::WHT << C::BLD << " > " << C::R;
            getline(cin, input);
            input.erase(0, input.find_first_not_of(" \t"));
            if (!input.empty()) input.erase(input.find_last_not_of(" \t")+1);
            if (input.empty()) continue;

            if (input=="exit") {
                cout << "\n  " << C::BG_GRN << C::WHT << C::BLD
                     << " Thank you for using DIGONTO.  " << C::R << "\n";
                break;
            }
            if (input=="add") {
                string t, d;
                cout << "  " << C::CYN << "Title       : " << C::WHT; getline(cin,t); cout << C::R;
                cout << "  " << C::CYN << "Description : " << C::WHT; getline(cin,d); cout << C::R;
                addEntry(t, d); this->sync(); continue;
            }
            ISearchCore* enginePtr = this;    //Base class pointer
            enginePtr->execute(input);        //Runtime polymorphism
        }
    }
};
int main() {
    //object
    Logger         log1,         log2("custom_log.txt");
    FileHandler    fh1,          fh2("input.txt");
    WeatherModule  weather;
    CalendarModule cal;
    SearchStats    stats;
    DataManager    dm;    //Single inheritance
    SearchCore     sc;    //Multiple inheritance

    //Template with different types
    ResultWrapper<string> r1("AI Result", 1);
    ResultWrapper<int>    r2(100,         2);
    ResultWrapper<double> r3(3.14,        3);

    // Suppress unused-variable warnings
    (void)log1;(void)log2;(void)fh1;(void)fh2;
    (void)weather;(void)cal;(void)stats;(void)dm;(void)sc;
    (void)r1;(void)r2;(void)r3;

    SearchEngine engine;                           //Hybrid inheritance
    ISearchCore* basePtr  = &engine;               //Base class pointer
    SearchEngine* selfPtr = engine.getThis();      //this pointer
    (void)basePtr; (void)selfPtr;

    engine.start();
    return 0;
}   //All destructors called at scope end
