#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <cctype>
#include <cstdint>
#include <algorithm>
#include <string>

#ifdef _WIN32
#include <windows.h>
std::string getExecutablePath() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string exePath(path);
    return exePath.substr(0, exePath.find_last_of("\\/"));
}
#elif __linux__
#include <unistd.h>
#include <limits.h>
std::string getExecutablePath() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string exePath = std::string(result, (count > 0) ? count : 0);
    return exePath.substr(0, exePath.find_last_of("/"));
}
#else
std::string getExecutablePath() {
    return "";
}
#endif

std::vector<std::string> split(const std::string& input, char delimiter) {
    std::vector<std::string> parts;
    std::string current;

    for (char c : input) {
        if (c == delimiter) {
            parts.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }

    parts.push_back(current);
    return parts;
}

std::vector<std::string> splitPreservingEscapes(const std::string& input) {
    std::vector<std::string> parts;
    std::string current;
    bool escape = false;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (escape) {
            current += c;
            escape = false;
        } else if (c == '\\') {
            escape = true;
        } else if (c == ';') {
            parts.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        parts.push_back(current);
    }

    return parts;
}

std::string stringToLowercase(const std::string& input) {
    std::string output;
    output.reserve(input.size());
    for (char c : input) {
        output.push_back(std::tolower(static_cast<unsigned char>(c)));
    }
    return output;
}

std::string escapeWord(const std::string& word) {
    std::string escaped;
    for (char c : word) {
        if (c == '\\') escaped += "\\\\";
        else if (c == ';') escaped += "\\;";
        else escaped += c;
    }
    return escaped;
}

std::string unescapeWord(const std::string& word) {
    std::string result;
    bool escaping = false;

    for (char c : word) {
        if (escaping) {
            result += c;
            escaping = false;
        } else if (c == '\\') {
            escaping = true;
        } else {
            result += c;
        }
    }

    return result;
}

std::deque<int> intToCompressedIndex(int num) {
    std::deque<int> values;

    if (num == 0)
        values.push_front(0);
    else
        while (num > 0) {
            int remainder = num % 92;
            values.push_front(remainder);
            num /= 92;
        }

    return values;
}

int compressedIndexToInt(std::deque<int> compressedIndex) {
    int num = 0;

    for (int digit : compressedIndex) {
        num = num * 92 + digit;
    }

    return num;
}

char intToAscii(int num) {
    int ascii = num + 32;
    if (ascii >= 42) ascii++;
    if (ascii >= 43) ascii++;
    if (ascii >= 59) ascii++;
    return (char)ascii;
}

int asciiToInt(char ascii) {
    int num = (int)ascii;
    if (num >= 59) num--;
    if (num >= 43) num--;
    if (num >= 42) num--;
    return num - 32;
}

int findWordInDict(const std::string& word, const std::vector<std::string>& dict) {
    for (int i = 0; i < (int)dict.size(); i++) {
        if (word == dict[i]) {
            return i;
        }
    }
    return -1;
}

int findLongestPrefixInDict(const std::string& word, const std::vector<std::string>& dict) {
    int index = -1;
    size_t bestLength = 0;

    for (int i = 0; i < (int)dict.size(); i++) {
        const std::string& dictWord = dict[i];
        if (word.size() >= dictWord.size() && word.compare(0, dictWord.size(), dictWord) == 0) {
            if (dictWord.size() > bestLength) {
                bestLength = dictWord.size();
                index = i;
            }
        }
    }

    return index;
}

std::string parseWord(const std::string& word, const std::vector<std::string>& dict) {
    int index = findWordInDict(word, dict);
    bool isFullMatch = (index != -1);

    if (!isFullMatch) {
        index = findLongestPrefixInDict(word, dict);
        if (index == -1) return '*' + word;
    }

    std::deque<int> compressedIndex = intToCompressedIndex(index);
    std::string res;
    for (int digit : compressedIndex) {
        res.push_back(intToAscii(digit));
    }

    if (!isFullMatch) {
        const std::string& matchedPrefix = dict[index];
        std::string suffix = word.substr(matchedPrefix.size());
        if (!suffix.empty()) {
            res += '+';
            res += suffix;
        }
    }

    return res;
}

std::string parseCompressed(const std::string& compressedWord, const std::vector<std::string>& dict) {
    if (compressedWord.empty()) return "";

    if (compressedWord[0] == '*') return compressedWord.substr(1);

    size_t plusPos = compressedWord.find('+');
    std::string encoded = (plusPos == std::string::npos) ? compressedWord : compressedWord.substr(0, plusPos);
    std::string suffix = (plusPos == std::string::npos) ? "" : compressedWord.substr(plusPos + 1);

    std::deque<int> compressedIndex;
    for (char c : encoded) {
        compressedIndex.push_back(asciiToInt(c));
    }

    int index = compressedIndexToInt(compressedIndex);
    if (index < 0 || index >= (int)dict.size()) {
        return "INVALID_INDEX";
    }

    return unescapeWord(dict[index] + suffix);
}

std::vector<std::string> loadDict(const std::string& dictName) {
    std::ifstream dictIn(dictName + ".txt");
    if (!dictIn) {
        std::cerr << "Dictionary file not found: " << dictName << "\n";
        return {};
    }

    std::string buf;
    std::vector<std::string> dict;
    while (std::getline(dictIn, buf)) {
        dict.push_back(buf);
    }

    return dict;
}

std::vector<uint8_t> packBits(const std::vector<bool>& bits) {
    std::vector<uint8_t> bytes((bits.size() + 7) / 8, 0);

    for (size_t i = 0; i < bits.size(); ++i) {
        if (bits[i]) {
            bytes[i / 8] |= (1 << (7 - (i % 8)));
        }
    }
    return bytes;
}

std::vector<bool> unpackBits(const std::vector<uint8_t>& bytes, size_t totalBits) {
    std::vector<bool> bits;
    bits.reserve(totalBits);

    for (size_t i = 0; i < totalBits; ++i) {
        uint8_t byte = bytes[i / 8];
        bool bit = (byte & (1 << (7 - (i % 8)))) != 0;
        bits.push_back(bit);
    }

    return bits;
}

std::string encodeBytesToBase92(std::vector<uint8_t> bytes) {
    if (bytes.empty()) return "";

    std::string result;

    while (!(bytes.size() == 1 && bytes[0] == 0)) {
        if (bytes.empty()) break;

        int remainder = 0;
        std::vector<uint8_t> newBytes;

        for (size_t i = 0; i < bytes.size(); ++i) {
            int acc = remainder * 256 + bytes[i];
            int digit = acc / 92;
            remainder = acc % 92;

            if (!newBytes.empty() || digit != 0) {
                newBytes.push_back(digit);
            }
        }

        if (newBytes.empty()) {
            newBytes.push_back(0);
        }

        bytes = newBytes;
        result.push_back(intToAscii(remainder));
    }

    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<uint8_t> decodeBase92ToBytes(const std::string& input) {
    std::vector<uint8_t> bytes = {0};

    for (char c : input) {
        int val = asciiToInt(c);
        std::vector<uint8_t> result;

        int carry = val;
        for (size_t i = 0; i < bytes.size(); ++i) {
            int acc = bytes[i] * 92 + carry;
            result.push_back(acc & 0xFF);
            carry = acc >> 8;
        }

        while (carry > 0) {
            result.push_back(carry & 0xFF);
            carry >>= 8;
        }

        bytes = result;
    }

    std::reverse(bytes.begin(), bytes.end());
    return bytes;
}

std::string formatCapsBitmap(const std::vector<bool>& caps) {
    std::string res;
    size_t run = 0;

    for (bool bit : caps) {
        if (bit) {
            if (run != 0) {
                std::deque<int> digits = intToCompressedIndex(static_cast<int>(run));
                for (int digit : digits) {
                    res.push_back(intToAscii(digit));
                }
                run = 0;
            }
            res.push_back(';');
        } else {
            run++;
        }
    }

    if (run != 0) {
        std::deque<int> digits = intToCompressedIndex(static_cast<int>(run));
        for (int digit : digits) {
            res.push_back(intToAscii(digit));
        }
    }

    return res;
}

std::vector<bool> unformatCapsBitmap(const std::string& caps) {
    std::vector<bool> res;
    size_t i = 0;

    while (i < caps.size()) {
        if (caps[i] == ';') {
            res.push_back(true);
            i++;
        } else {
            std::deque<int> digits;
            while (i < caps.size() && caps[i] != ';') {
                digits.push_back(asciiToInt(caps[i]));
                i++;
            }
            int runLength = compressedIndexToInt(digits);
            res.insert(res.end(), runLength, false);
        }
    }

    return res;
}

void compress(const std::string& fileName, const std::string& dictName) {
    std::ifstream in(fileName);
    if (!in) {
        std::cerr << "File not found: " << fileName << "\n";
        return;
    }

    std::vector<std::string> dict = loadDict(dictName);
    if (dict.empty()) return;

    std::string outFile = fileName;
    size_t dotPos = outFile.rfind('.');
    if (dotPos != std::string::npos) {
        outFile.erase(dotPos);
    } else {
        std::cout << "Invalid file name" << std::endl;
        return;
    }

    std::ofstream out(outFile + ".ctff");
    std::string buf;
    std::vector<bool> caps;

    while (std::getline(in, buf)) {
        for (char c : buf) {
            if (std::isalpha(static_cast<unsigned char>(c))) {
                caps.push_back(std::isupper(static_cast<unsigned char>(c)));
            }
        }        
        
        std::vector<std::string> words = split(buf, ' ');
        buf.clear();

        for (const std::string& word : words) {
            std::string lowerWord = stringToLowercase(word);
            std::string encoded = parseWord(lowerWord, dict);
            buf.append(escapeWord(encoded) + ';');
        }
        if (!buf.empty()) buf.pop_back();

        out << buf << std::endl;
    }

    out << "\\UPR\n";
    out << formatCapsBitmap(caps);

    in.close();
    out.close();
}

void decompress(const std::string& fileName, const std::string& dictName) {
    std::ifstream in(fileName);
    if (!in) {
        std::cerr << "File not found: " << fileName << "\n";
        return;
    }

    std::string outFile = fileName;
    size_t dotPos = outFile.rfind('.');
    if (dotPos != std::string::npos) {
        outFile.erase(dotPos);
    } else {
        std::cout << "Invalid file name" << std::endl;
        return;
    }

    std::ofstream out(outFile + ".txt");
    std::vector<std::string> lines;
    std::string line;

    std::vector<std::string> dict = loadDict(dictName);
    if (dict.empty()) return;

    bool foundUPR = false;
    std::string capsEncoded;
    while (std::getline(in, line)) {
        if (line == "\\UPR") {
            foundUPR = true;
            std::getline(in, capsEncoded);
            break;
        }
        lines.push_back(line);
    }

    std::vector<bool> caps;
    if (foundUPR) {
        caps = unformatCapsBitmap(capsEncoded);
    } else {
        std::cerr << "No \\UPR section found.\n";
    }

    size_t capIndex = 0;
    for (const std::string& encodedLine : lines) {
        std::vector<std::string> words = splitPreservingEscapes(encodedLine);
        std::string buf;

        for (const std::string& word : words) {
            std::string decoded = parseCompressed(word, dict);
            std::string adjusted;

            for (char c : decoded) {
                if (std::isalpha(static_cast<unsigned char>(c))) {
                    if (capIndex < caps.size() && caps[capIndex]) {
                        adjusted += std::toupper(static_cast<unsigned char>(c));
                    } else {
                        adjusted += c;
                    }
                    capIndex++;
                } else {
                    adjusted += c;
                }
            }

            buf.append(adjusted + ' ');
        }

        if (!buf.empty()) buf.pop_back();
        out << buf << '\n';
    }

    in.close();
    out.close();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "No file specified\n";
        return 1;
    }

    std::string fileName = argv[1];

    std::string exeDir = getExecutablePath();

    std::string dictBase = exeDir + "/dict/";

    std::string dictName;
    if (argc < 3) {
        dictName = dictBase + "english";
    } else {
        dictName = dictBase + argv[2];
    }

    if (fileName.size() >= 5 && fileName.substr(fileName.size() - 5) == ".ctff") {
        decompress(fileName, dictName);
    } else {
        compress(fileName, dictName);
    }

    return 0;
}
