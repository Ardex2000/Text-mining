#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>
#include <cstdio>
#include <cstdlib>

// Function to install Bzip2 based on the operating system
#ifdef _WIN64
void installBzip2() {
    std::system("powershell -Command \"Start-Process 'powershell' -ArgumentList 'Install-Package -Name bzip2 -Force' -Verb RunAs\"");
}
#else
void installBzip2() {
    std::system("sudo apt-get update && sudo apt-get install -y bzip2");
}
#endif

// Function to check if a file exists
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

// Function to search for a single word in the dictionary
void searchDictionary(const std::unordered_map<std::string, std::vector<double>>& dict_new) {
    std::string response;
    bool first_time = true;

    while (true) {
        if (first_time) {
            std::cout << "Do you want to search for a single word in the dictionary? (Yes/Tak/tak/1): ";
            first_time = false;
        } else {
            std::cout << "Do you want to search for another single word? (Yes/Tak/tak/1): ";
        }

        std::cin >> response;

        if (response == "Yes" || response == "Tak" || response == "tak" || response == "1") {
            std::string word;
            std::cout << "Enter the word to search for: ";
            std::cin >> word;

            auto search = dict_new.find(word);
            if (search != dict_new.end()) {
                std::cout << "Found: " << search->first << "\n";
                std::cout << "Vector: ";
                for (auto &val : search->second) {
                    std::cout << val << " ";
                }
                std::cout << "\n";
            } else {
                std::cout << "Not found. Ensure to provide words without extra characters.\n";
            }
        } else {
            std::cout << "Ending\n";
            break;
        }
    }
}

// Function to search for a word and return its vector, or a neutral vector if not found
std::vector<double> searchDictionary(const std::unordered_map<std::string, std::vector<double>>& dict_new, const std::string& word) {
    auto search = dict_new.find(word);
    if (search != dict_new.end()) {
        return search->second;
    } else {
        return std::vector<double>(300, 0.0);  // Return a neutral vector
    }
}

// Function to calculate the mean vector from a list of vectors
std::vector<double> calculateMeanVector(const std::vector<std::vector<double>>& vectors) {
    if (vectors.empty()) {
        return std::vector<double>(300, 0.0);  // Return a neutral vector
    }

    int vector_size = vectors[0].size();
    std::vector<double> mean_vector(vector_size, 0.0);

    for (const auto& vec : vectors) {
        for (int i = 0; i < vector_size; ++i) {
            mean_vector[i] += vec[i];
        }
    }

    // Calculate the mean
    for (int i = 0; i < vector_size; ++i) {
        mean_vector[i] /= vectors.size();
    }

    return mean_vector;
}

// Function to split a sentence into words
std::vector<std::string> splitWords(const std::string& sentence) {
    std::istringstream iss(sentence);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}

// Function to split text into sentences based on punctuation
std::vector<std::string> splitSentences(const std::string& text) {
    std::vector<std::string> sentences;
    std::string sentence;
    int i = 0;
    while (i < text.length()) {
        if (text.substr(i, 3) == "...") {
            if (!sentence.empty()) {
                sentences.push_back(sentence);
                sentence.clear();
            }
            i += 3;
        } else if (text[i] == '.' || text[i] == '?') {
            while (i < text.length() && (text[i] == '.' || text[i] == '?' || text[i] == ' ')) {
                i++;
            }
            if (!sentence.empty()) {
                sentences.push_back(sentence);
                sentence.clear();
            }
        } else {
            sentence += text[i];
            i++;
        }
    }
    if (!sentence.empty()) {
        sentences.push_back(sentence);
    }
    return sentences;
}

int main() {
    system("chcp 1250 > NULL");
    installBzip2();
    std::cout << "The model works exclusively with English words." << std::endl;
    std::cout << "Loading the model, please wait.\n";
    time_t start = std::time(NULL);

    // Download the model from the internet if not present
    const std::string filename = "enwiki_20180420_300d.txt";
    if (!fileExists(filename)) {
        std::system("curl -O http://wikipedia2vec.s3.amazonaws.com/models/en/2018-04-20/enwiki_20180420_300d.txt.bz2");
        std::system("bzip2 -d enwiki_20180420_300d.txt.bz2");
    } else {
        std::cout << "File already exists. Skipping download." << std::endl;
    }

    std::ifstream file("enwiki_20180420_300d.txt");
    std::string line;
    std::unordered_map<std::string, std::vector<double>> dict_new; // Dictionary

    // Read the header from the file
    std::getline(file, line);

    // Process the file line by line
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string word;
        std::vector<std::string> split_line;

        // Split the line into words
        while (iss >> word) {
            split_line.push_back(word);
        }

        // The key is the first word, the rest are the vector
        std::string key = split_line[0];

        // Vectors
        std::vector<double> values;
        for (size_t i = 1; i < split_line.size(); ++i) {
            values.push_back(std::stod(split_line[i]));
        }

        dict_new[key] = values;
    }

    file.close();

    // Load stopwords
    std::string path = "G:\\LLM\\corpora\\stopwords\\english";
    std::ifstream stopwordsFile(path);
    std::vector<std::string> stopwords;

    while (std::getline(stopwordsFile, line)) {
        stopwords.push_back(line);
    }

    stopwordsFile.close();

    time_t stop = std::time(NULL);
    int diff_time = std::difftime(stop, start);

    // Report the time taken to load the model
    switch (diff_time) {
        case 1:
            std::cout << "Language model loaded in : " << diff_time << " second" << std::endl;
            break;
        case 2:
        case 3:
        case 4:
            std::cout << "Language model loaded in : " << diff_time << " seconds" << std::endl;
            break;
        default:
            std::cout << "Language model loaded in : " << diff_time << " seconds" << std::endl;
            break;
    }

    searchDictionary(dict_new);

    std::cin.ignore();  // To catch new input correctly
    std::cout << "Enter your sentences: ";
    std::string userInput;
    std::getline(std::cin, userInput);

    // Split input into sentences
    std::vector<std::string> sentences = splitSentences(userInput);

    std::cout << "Detected " << sentences.size() << " sentences." << std::endl;

    std::vector<std::vector<double>> sentence_mean_vectors;

    // Process each sentence
    for (const auto& sentence : sentences) {
        std::vector<std::string> words = splitWords(sentence);
        std::vector<std::vector<double>> vectors;
        for (const auto& word : words) {
            vectors.push_back(searchDictionary(dict_new, word));
        }
        sentence_mean_vectors.push_back(calculateMeanVector(vectors));
    }

    while (true) {
        std::cout << "Enter the sentence number you want to view in Word2vec notation (or 'Exit' to end): ";
        std::string input;
        std::cin >> input;

        if (input == "Exit") {
            break;
        }

        int sentence_index;
        try {
            sentence_index = std::stoi(input);  // Convert input to integer
        } catch (...) {
            std::cout << "Invalid number. Please try again.\n";
            continue;
        }

        if (sentence_index <= 0 || sentence_index > sentences.size()) {  // Out of range
            std::cout << "Invalid number. Please try again.\n";
            continue;
        }

        int index = sentence_index - 1;  // Adjust for 0-based index
        std::cout << "Sentence: " << sentences[index] << '\n';
        std::cout << "Word2vec: ";

        for (auto val : sentence_mean_vectors[index]) {
            std::cout << val << ' ';
        }
        std::cout << '\n';
    }

    return 0;
}
