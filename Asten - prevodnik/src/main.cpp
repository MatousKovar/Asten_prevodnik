#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <cassert>
#include <algorithm>

using namespace std;

class CFormatter {
public:
    CFormatter() = default;

    CFormatter(const char delimiter)
            : m_Delimiter(delimiter) {}

    void ProcessFile(const string & filename) {
        ifstream stream;
        stream.open(filename);
        string line;
        while (getline(stream, line, '\n')) {
            cout << line << endl;
            m_Output.push_back(ProcessLine(line));
        }
        stream.close();
    }

    void OutputFile(const string & filename) {
        ofstream stream;
        stream.open(filename);
        for (size_t i = 0; i < m_Output.size(); i++) {
            bool first = true;
            for (size_t j = 0; j < m_Output[i].size(); j++) {
                if (!first) {
                    stream << ", ";
                }
                first = false;
                stream << m_Output[i][j];
            }
            if (i != (m_Output.size() - 1))
                stream << "\n";
        }
        stream.close();
    }

private:
    /**
     * Vzdy se nacitaji cisla, dokud jsou menci nez dany nasobek 16 (radek), potom se cisla prevedou na decimalni cislo
     * Nacitani probiha odkud se nenarazi na eof, nebo prazdny buffer (do konce uz budou jen nuly)
     * @param line
     * @return vektor onsahujici prevedene decimalni cislo pro kazdy radek matice
     */
    vector<int> ProcessLine(const string & line) {
        size_t multiplier = 0;
        vector<int> res;
        vector<int> matrixLine; // vektor, ktery obsahuje indexy '1' v radku matice
        stringstream stream(line);
        string buffer;
        getline(stream, buffer, m_Delimiter); // odstraneni cisla radku
        getline(stream, buffer, m_Delimiter); // nacteni prvniho cisla

        while (true) {
            if (find_if(buffer.begin(), buffer.end(), ::isdigit) != buffer.end()) {
                int readInt = stoi(buffer);
                if (readInt > (16 * (multiplier + 1))) {
                    multiplier++;
                    res.push_back(ConvertNumber(matrixLine));
                    matrixLine.clear();
                    continue;
                }
                matrixLine.push_back(readInt - 16 * multiplier);
            }
            if (!getline(stream, buffer, m_Delimiter))
                break;
        }
        res.push_back(ConvertNumber(matrixLine));
        FillVector(res);
        return res;
    }

    int Pow(int base, int exponent) {
        int res = 1;
        for (size_t i = 0; i < exponent; i++)
            res = res * base;
        return res;
    }

    /**
     * Prida 0, aby dolpnil radek na 16 prvku
     * @param source
     */
    void FillVector(vector<int> & source) {
        while (source.size() != 16)
            source.push_back(0);
    }

    /**
     * Prevadi vektor desitkovych cisel reprezentujici poziti jednicek v binarnim cisle na decimalni cislo
     * @param source
     * @return decimalni cislo
     */
    int ConvertNumber(const vector<int> & source) {
        int res = 0;
        for (size_t i = 0; i < source.size(); i++) {
            res += Pow(2, 16 - source[i]);
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
            throw invalid_argument("Soubor \"" + m_InFile + "\" nelze otevrit. ");
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

class CTester {
public:
    void Run() {
        CApplication app;
        CFormatter formatter(',');
        assert (formatter.ConvertNumber({10, 12, 16}) == 81);
        assert (formatter.ConvertNumber({}) == 0);
        assert (formatter.ConvertNumber({16}) == 1);
        assert (formatter.ConvertNumber({10}) == 64);
        vector<int> a = {21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 1, 2, 0};
        assert (formatter.ProcessLine(
                "4,12,14,16,28,30,32,44,46,48,60,62,64,76,78,80,92,94,96,108,110,112,124,126,128,140,142,144,156,158,160,172,174,176,188,190,192,204,206,208,224,239") ==
                a);
    }
};


int main(int argc, char *argv[]) {
    //vstupni parametry mohou byt 2 nebo 3 ( oddelovac je implicitne ','
    if (argc != 3 && argc != 4) {
        cout << "Chybi parametr, format[vstupni soubor, vytupni soubor, oddelovac]" << endl;
        return EXIT_FAILURE;
    }
    char delimiter = ',';
    if (argc == 4)
        delimiter = *argv[3];
    CApplication app(argv[1], argv[2], delimiter);

    try {
        app.Run();
    }
    catch (const invalid_argument & ex) {
        cout << "Chyba v prevodu: " << ex.what() << "Zkontrolujte oddelovac, a vstupni soubor." << endl;
        return EXIT_FAILURE;
    }
    cout << "Uspech" << endl;

    return 0;

}