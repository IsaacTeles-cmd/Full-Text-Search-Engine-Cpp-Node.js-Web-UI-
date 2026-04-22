#include <iostream>
#include "SearchEngine.h"
#include <string>

using namespace std;

void performIndexing(SearchEngine& engine) {
    engine.clear();
    engine.indexFile("../data/file1.txt");
    engine.indexFile("../data/file2.txt");
    engine.saveIndex("../storage/index.dat");
}

int main(int argc, char* argv[]){

    SearchEngine engine;
    engine.loadIndex("../storage/index.dat");

    if (engine.isEmpty()) {
        performIndexing(engine);
    }

    string firstArg = argv[1];

    // Supports multiple words
    string query;
    for(int i = 1; i < argc; i++){
        query += argv[i];
        if(i != argc - 1) query += " ";
    }

    if (firstArg == "reindex") {
        performIndexing(engine);
        cout << "{ \"status\": \"reindex completed\" }" << endl;
        return 0;
    }

    // Autocomplete implemented (FINALLY)
    if (firstArg == "--autocomplete" && argc >= 3) {
        string prefix = argv[2];
        auto suggestions = engine.autocomplete(prefix);

        cout << "[";
        for (size_t i = 0; i < suggestions.size(); i++) {
            cout << "\"" << suggestions[i] << "\"" << (i == suggestions.size() - 1 ? "" : ",");
        }
        cout << "]" << endl;
        return 0;
    }

    auto results = engine.search(query);

    // Return JSON
    cout << "[";
    for (size_t i = 0; i < results.size(); i++) {
        cout << "{ \"file\": \"" << results[i].file << "\", \"score\": " << results[i].score << " }";
        if (i != results.size() - 1) cout << ",";
    }
    cout << "]" << endl;

    return 0;
}