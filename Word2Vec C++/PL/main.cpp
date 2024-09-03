#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>
#include <cstdio>
#include <cstdlib>



#ifdef _WIN64
void installBzip2() {
    std::system("powershell -Command \"Start-Process 'powershell' -ArgumentList 'Install-Package -Name bzip2 -Force' -Verb RunAs\"");
}
#else
void installBzip2() {
    std::system("sudo apt-get update && sudo apt-get install -y bzip2");
}
#endif


bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}


void searchDictionary(const std::unordered_map<std::string, std::vector<double>>& dict_new) {
    std::string response;
    bool first_time = true;

    while (true) {
        if (first_time) {
            std::cout << "Czy chcesz wyszukać pojedyncze słowo w słowniku? (Yes/Tak/tak/1): ";
            first_time = false;
        } else {
            std::cout << "Czy chcesz wyszukać kolejne pojedyncze słowo? (Yes/Tak/tak/1): ";
        }

        std::cin >> response;

        if (response == "Yes" || response == "Tak" || response == "tak" || response == "1") {
            std::string word;
            std::cout << "Podaj słowo do wyszukania: ";
            std::cin >> word;

            auto search = dict_new.find(word);
            if (search != dict_new.end()) {
                std::cout << "Znaleziono: " << search->first << "\n";
                std::cout << "Wektor: ";
                for (auto &val : search->second) {
                    std::cout << val << " ";
                }
                std::cout << "\n";
            } else {
                std::cout << "Nie znaleziono, pamiętaj by podawać wyłącznie słowa bez dodatkowych znaków\n";
            }
        } else {
            std::cout << "Koniec\n";
            break;
        }
    }
}

std::vector<double> searchDictionary(const std::unordered_map<std::string, std::vector<double>>& dict_new, const std::string& word) {
    auto search = dict_new.find(word);
    if (search != dict_new.end()) {
        return search->second;
    } else {
        return std::vector<double>(300, 0.0);  // Zwraca neutralny wektor
    }
}

std::vector<double> calculateMeanVector(const std::vector<std::vector<double>>& vectors) {
    if (vectors.empty()) {
        return std::vector<double>(300, 0.0);  // Zwraca neutralny wektor
    }

    int vector_size = vectors[0].size();
    std::vector<double> mean_vector(vector_size, 0.0);

    for (const auto& vec : vectors) {
        for (int i = 0; i < vector_size; ++i) {
            mean_vector[i] += vec[i];
        }
    }

    // Średnia
    for (int i = 0; i < vector_size; ++i) {
        mean_vector[i] /= vectors.size();
    }

    return mean_vector;
}

std::vector<std::string> splitWords(const std::string& sentence) {
    std::istringstream iss(sentence);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}

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
    std::cout << "Model działa wyłącznie na słowach w języku angielskim." << std::endl;
    std::cout << "Ładowanie modelu, proszę czekać.\n";
    time_t start = std::time(NULL);

    // Pobieranie modelu z internetu
    const std::string filename = "enwiki_20180420_300d.txt";
    if (!fileExists(filename)) {
        std::system("curl -O http://wikipedia2vec.s3.amazonaws.com/models/en/2018-04-20/enwiki_20180420_300d.txt.bz2");
        std::system("bzip2 -d enwiki_20180420_300d.txt.bz2");
    } else {
        std::cout << "File already exists. Skipping download." << std::endl;
    }

    std::ifstream file("enwiki_20180420_300d.txt");
    std::string line;
    std::unordered_map<std::string, std::vector<double>> dict_new; //"SŁOWNIK"

    // Nagłówki w zbiorze
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string word;
        std::vector<std::string> split_line;

        // Dzieli linię
        while (iss >> word) {
            split_line.push_back(word);
        }

        // Klucz to pierwsze słowo, reszta to wektor
        std::string key = split_line[0];

        // Wektory
        std::vector<double> values;
        for (size_t i = 1; i < split_line.size(); ++i) {
            values.push_back(std::stod(split_line[i]));
        }

        dict_new[key] = values;
    }

    file.close();

    // Ładowanie stopwords
    std::string path = "G:\\LLM\\corpora\\stopwords\\english";
    std::ifstream stopwordsFile(path);
    std::vector<std::string> stopwords;

    while (std::getline(stopwordsFile, line)) {
        stopwords.push_back(line);
    }

    stopwordsFile.close();

    time_t stop = std::time(NULL);
    int diff_time = std::difftime(stop, start);

    switch (diff_time) {
        case 1:
            std::cout << "Model językowy załadowany w : " << diff_time << " sekundę" << std::endl;
            break;
        case 2:
        case 3:
        case 4:
            std::cout << "Model językowy załadowany w : " << diff_time << " sekundy" << std::endl;
            break;
        default:
            std::cout << "Model językowy załadowany w : " << diff_time << " sekund" << std::endl;
            break;
    }

    searchDictionary(dict_new);

    std::cin.ignore();  // Bez tego nie łapie nowego inputu.
    std::cout << "Wpisz swoje zdania: ";
    std::string userInput;
    std::getline(std::cin, userInput);

    // Split
    std::vector<std::string> sentences = splitSentences(userInput);

    std::cout << "Wykryto " << sentences.size() << " zdań." << std::endl;

    std::vector<std::vector<double>> sentence_mean_vectors;

    for (const auto& sentence : sentences) {
        std::vector<std::string> words = splitWords(sentence);
        std::vector<std::vector<double>> vectors;
        for (const auto& word : words) {
            vectors.push_back(searchDictionary(dict_new, word));
        }
        sentence_mean_vectors.push_back(calculateMeanVector(vectors));
    }

    while (true) {
        std::cout << "Podaj numer zdania, które chcesz zobaczyć w notacji Word2vec (lub 'Wyjście' aby zakończyć): ";
        std::string input;
        std::cin >> input;

        if (input == "Wyjście") {
            break;
        }

        int sentence_index;
        try {
            sentence_index = std::stoi(input);  // Jakby się wywaliło z jakiegokolwiek powodu
        } catch (...) {
            std::cout << "Nieprawidłowy numer. Spróbuj ponownie.\n";
            continue;
        }

        if (sentence_index <= 0 || sentence_index > sentences.size()) {  // poza zakresem
            std::cout << "Nieprawidłowy numer. Spróbuj ponownie.\n";
            continue;
        }

        int index = sentence_index - 1;  // BO jak w Pythonie od 0
        std::cout << "Zdanie: " << sentences[index] << '\n';
        std::cout << "Word2vec: ";

        for (auto val : sentence_mean_vectors[index]) {
            std::cout << val << ' ';
        }
        std::cout << '\n';
    }

    return 0;
}
