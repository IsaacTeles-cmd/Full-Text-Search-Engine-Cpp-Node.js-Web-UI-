#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>


// Store search results.
struct Result {
    std::string file;
    double score;
};

class SearchEngine{
    private:
         // The Inverted Index: Word -> File Set -> Frequency.
        std::map<std::string, std::map<std::string, int>> index;
        std::set<std::string> documents;

    public:
        // Constructor.
        SearchEngine() = default;

        std::string normalize(std::string word);

        void indexFile(const std::string& filename);
        std::vector<Result> search(const std::string& query);

        // Data persistence.
        void saveIndex(const std::string& filename);
        void loadIndex(const std::string& filename);

        // Check if index is empty.
        bool isEmpty() const;

        std::vector<std::string>autocomplete(const std::string& prefix);

        // Clear memory.
        void clear();
};