// TRPO2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "Header.h"
vector <vector<string>> ranged_text;





struct word
{ // для хранения слов с номерами предложений
    int number_of_sentence;
    string word;
    int part_of_speech;
};
struct rhyme_pair
{
    string word1;
    string word2;
};

enum speech_parts
{
    noun, // существительное
    verb,   // глагол
    adjective, // приллагательное
    participle, // причастие
    adverb, //наречие
    numeral, // числительное 
    adverbial_participle, // деепричастие
    pronoun // местоимение

};

vector <word> get_words(bitset<8>  &bitmask, string & file_name)
{ // принимаем биты для частей речи

    
    ranged_text = parseTextToSentences(file_name); // получаем вектор предложений
    vector <word> data; 
    for (int i = 0; i < ranged_text.size(); i++)
    {// цикл по вектору предложений
        for (int j = 0; j < ranged_text[i].size(); j++)

        {// цикл по словам
            word buf; 
            buf.number_of_sentence = i + 1; // номер предложения, в котором находится слово
            buf.word = ranged_text[i][j];

           // выполняются проверки на части речи
            if (bitmask[0] == 1)
            { // если мы ищем существительное
                if (is_noun(buf.word))
                { // если слово существительное
                    buf.part_of_speech = noun; // обозначаем часть речи
                    data.push_back(buf);
                }
            } // аналогично для остальных частей речи
            if (bitmask[1] == 1)
            {

            }
            if (bitmask[2] == 1)
            {

            }
            if (bitmask[3] == 1)
            {

            }
            if (bitmask[4] == 1)
            {

            }
            if (bitmask[5] == 1)
            {

            }
            if (bitmask[6] == 1)
            {

            }
            if (bitmask[6] == 1)
            {

            }
        }

    }
    return data;
}



int main()
{
    //=================================
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "ru");
    system("color F0");
    //=================================
    
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
