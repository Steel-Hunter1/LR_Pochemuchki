#include "Header.h"


using namespace std;

vector<vector<string>> parseTextToSentences(const string& filename) 
{ // функция разбиения файла на предложения, а предложений на слова

    vector<vector<string>> sentences; // создаем массив предложений
    ifstream file(filename); // открываем файл на чтение
    string line; // буферная строка

    if (!file.is_open()) 
    {
        return sentences;
    }

    // Читаем весь файл в строку
    string text((istreambuf_iterator<char>(file)),
      istreambuf_iterator<char>());
    file.close();

    // Разбиваем текст на предложения
    vector<string> raw_sentences; // массив с цельными предложениями
    size_t start = 0;
    size_t end = text.find_first_of(".!?");
     

    //========================================================================================================================================

//========================================================================================================================================

    while (end != string::npos)
    {
        string sentence = text.substr(start, end - start);
        // Удаляем лишние пробелы в начале и конце
        sentence.erase(sentence.begin(), find_if(sentence.begin(), sentence.end(), [](int ch) { return !isspace(ch); }));
        sentence.erase(find_if(sentence.rbegin(), sentence.rend(), [](int ch) { return !isspace(ch); }).base(), sentence.end());

        if (!sentence.empty())
        {
            raw_sentences.push_back(sentence);
        }

        start = end + 1;
        end = text.find_first_of(".!?", start);
    }



    // Разбиваем каждое предложение на слова
    for (const auto& sentence : raw_sentences)
    {
        vector<string> words;
        istringstream iss(sentence);
        string word;

        while (iss >> word) 
        {
            // Удаляем пунктуацию из слова (очень упрощенно)
            word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
            if (!word.empty()) {
                words.push_back(word);
            }
        }

        if (!words.empty()) {
            sentences.push_back(words);
        }
    }

    return sentences;
}

















