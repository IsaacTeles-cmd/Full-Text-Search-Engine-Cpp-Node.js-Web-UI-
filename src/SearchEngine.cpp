#include "SearchEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>

using namespace std;


string SearchEngine::normalize(string word){
    string cleanWord = "";

    for(char c : word){
        // Punctuation check
        if(!ispunct((unsigned char)c)){
            cleanWord += tolower((unsigned char)c); // Enable special characters.   
        }
    }
    return cleanWord;
}

void SearchEngine::indexFile(const string& filename){
    ifstream file(filename);

    if(!file.is_open()){
        cerr << "Error opening file: " << filename << endl;
        return; // Exit the function if the file does not exist.
    }

    documents.insert(filename);

    string word;

    while(file >> word){
        string cleanWord = normalize(word);

        // If the word is not empty, add it to the map.
        if(!cleanWord.empty()){
            index[cleanWord][filename]++;
        }
    }
}

std::vector<Result> SearchEngine::search(const std::string& query){
    stringstream ss(query);
    string word;
    vector<string> words;

    // Separate and normalize.
    while(ss >> word){
        string cleanWord = normalize(word);
        if(!cleanWord.empty()){
            words.push_back(cleanWord);
        }
    }

    if(words.empty() || documents.empty()){
        return {};
    }

    map<string, double> scores;
    double N = (double)documents.size();

    // Calculate score for each document based on term relevance
    for(const string& w : words){
        auto it = index.find(w);
        if(it == index.end()) continue;

        double df = (double)it->second.size();
        if(df == 0) continue;

        // Inverse Document Frequency (IDF)
        double idf = log(N / df + 1) + 1;

        for(const auto& pair : it->second){
            const string& file = pair.first;
            int tf = pair.second; // Term Frequency (TF)

            scores[file] += tf * idf;
        }
    }

    vector<Result> results;

    for(const auto& pair : scores){
        if(pair.second > 0.01){
        results.push_back({pair.first, pair.second});
        }
    }

     std::sort(results.begin(), results.end(),
        [](const Result& a, const Result& b){
            return a.score > b.score;
        }
    );
    return results;
}

// Export index.dat
void SearchEngine::saveIndex(const string& filename){
    ofstream out(filename);

    if(!out.is_open()){
        cerr << "Error saving index.\n";
        return;
    }

    for(const auto& [word, files] : index){
        out << word;

        for(const auto& [file, count] : files){
            out << " " << file << " " << count;
        }

        out << "\n";
    }
}

// Import index.dat
void SearchEngine::loadIndex(const string& filename){
    ifstream in(filename);

    if(!in.is_open()){
        return;
    }

    index.clear();
    documents.clear();

    string line;

    // Parse each line: word followed by file-count pairs
    while(getline(in, line)){
        stringstream ss(line);
        string word;
        ss >> word;

        string file;
        int count;

        while(ss >> file >> count){
            index[word][file] = count;
            documents.insert(file);
        }
    }
}

// Returns true if the index is currently empty
bool SearchEngine::isEmpty() const {
    return index.empty();
}

std::vector<std::string> SearchEngine::autocomplete(const std::string& prefix){
    std::vector<std::string> suggestions;

    std::string clean = normalize(prefix);

    for(const auto& [word, _] : index){ // Prefix: I'm crying. . .
        if(word.find(clean) == 0){
            suggestions.push_back(word);
        }
    }

    // Limit suggestions.
    if(suggestions.size() > 10)
        suggestions.resize(10);

    return suggestions;
}

void SearchEngine::clear(){
    index.clear();
    documents.clear();
}