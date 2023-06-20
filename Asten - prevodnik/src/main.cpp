#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <cassert>

using namespace std;

class CFormatter {
public:
    CFormatter() = default;
    CFormatter(const char delimiter)
            : m_Delimiter(delimiter) {}

    void ProcessFile(const string & filename) {
        ifstream stream(filename);
        string line;
        while (getline(stream, line, '\n')) {
            m_Output.push_back(ProcessLine(line));
        }
    }

    void OutputFile(const string filename) {

    }

private:
    vector<int> ProcessLine(const string & line) {
        size_t multiplier = 1;
        vector<int> res;
        stringstream stream(line);
        string buffer;
        getline(stream, buffer, m_Delimiter); // odstraneni cisla radku
        for (size_t i = 0; i < 40; i++) {
            int readInt = stoi(buffer);
            if (readInt > (16 * multiplier))
            {
                multiplier++;
                continue;
            }
        }
    }

    int pow(int base, int exponent)
    {
        int res = 1;
        for(size_t i = 0 ; i < exponent ; i++)
            res = res * base;
        return res;
    }
    /**
     * Prevadi vektor desitkovych cisel reprezentujici poziti jednicek v binarnim cisle na decimalni cislo
     * @param source
     * @return decimalni cislo
     */
    int ConvertNumber (const vector<int> & source)
    {
        int res = 0;
        for(size_t i = 0 ; i < source.size() ; i++)
        {
            res += pow(2, 16 - source[i]);
        }
        return res;
    }
/** @param m_Output  - vektor jednotlivych radek vystupniho csv souboru */
    vector<vector<int>> m_Output;
    char m_Delimiter;
    friend class CTester;
};

class CApplication {
public:
    CApplication() = default;
    CApplication(const string & in_file, const string & out_file, const char delimiter = ',')
            : m_Delimiter(delimiter),
              m_InFile(move(in_file)),
              m_OutFile(move(out_file)) {}

    void Run() {
        if (!ValidateFile()) {
            throw invalid_argument("Soubor \"" + m_InFile + "\" nelze otevřít.");
        }
        CFormatter formatter(m_Delimiter);
        formatter.ProcessFile(m_InFile);
        formatter.OutputFile(m_OutFile);
    }

private:
    bool ValidateFile() const {
        ifstream stream;
        stream.open(m_InFile);
        if (!stream.is_open())
            return false;
        return true;
    }

    char m_Delimiter;
    string m_InFile;
    string m_OutFile;
    friend class CTester;
};
class CTester
{
public:
    void Run ()
    {
        CApplication app;
        CFormatter formatter;
        assert (formatter.ConvertNumber({10,12,16}) == 81);
        assert (formatter.ConvertNumber({}) == 0);
        assert (formatter.ConvertNumber({16}) == 1);
        assert (formatter.ConvertNumber({10}) == 64);
    }
};


int main(int argc, char *argv[]) {
    //vstupni parametry mohou byt 2 nebo 3 ( oddelovac je implicitne ','
    if (argc != 3 && argc != 4)
        cout << "Chybí parametr, formát[vstupní soubor, výtupní soubor, oddělovač]" << endl;
    CApplication app(argv[1], argv[2], *argv[3]);

    try {
        app.Run();
    }
    catch (const invalid_argument & ex) {

    }

    CTester tester;
    tester.Run();

    return 0;

}
