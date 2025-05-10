#include "UI_const.h"
#include "Heart_of_program.h"
#include "file_working.h"
#include "NLP.h"


// Старт приложения и создание стартовых процедур
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (!LoadLibraryA("Msftedit.dll")) 
    {
        MessageBoxA(NULL, "Не удалось загрузить Msftedit.dll", "Ошибка", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Главный класс для создания параметров приветственного окна
    WNDCLASS MainWindClass = NewWindClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst,
                                          LoadIcon(NULL, IDI_QUESTION), L"MainClass", SoftwareMainProcedure);

    // Регистрация класса
    if (!RegisterClassW(&MainWindClass))
        return 0;

    // Получение разрешения экрана
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN) - 45;

    // Само создание с заложенными параметрами
    HWND hMainWnd = CreateWindow(L"MainClass", L"ТРПО лабораторная", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                 0, 0, screenWidth, screenHeight, NULL, NULL, NULL, NULL);
    if (!hMainWnd) {
        MessageBox(NULL, L"Не удалось создать главное окно", L"Ошибка", MB_OK | MB_ICONERROR);
        return 0;
    }
    // Устанавливаем окно поверх всех окон
    SetWindowPos(hMainWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_SHOWWINDOW);
    
    // Основной цикл работы проложения
    MSG msg = {0};

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);

        // Проверяем, что окно действительно существует, если hwnd не NULL
        if (msg.hwnd != NULL && !IsWindow(msg.hwnd)) {
            MessageBox(NULL, L"Недействительное окно в сообщении", L"Ошибка", MB_OK | MB_ICONERROR);
            break; // Прерываем цикл, если окно недействительно
        }

        DispatchMessage(&msg);
    }

    return 0;
}

// Обновляем состояние кнопок в зависимости от флагов
void UpdateButtonStatesAndColors()
{
    // Обновляем состояние кнопок в зависимости от флагов
    bool isHomogeneousMode = buttons::ButtonFlags[7]; // Флаг однородного режима
    bool hasInput = GetWindowTextLengthW(buttons::widgets.hEditInputWord) > 0;

    // Обновляем доступность кнопок
    EnableWindow(buttons::widgets.hVerbButton, !isHomogeneousMode || !hasInput);
    EnableWindow(buttons::widgets.hAdverbButton, !isHomogeneousMode || !hasInput);
    EnableWindow(buttons::widgets.hAdjectiveButton, !isHomogeneousMode || !hasInput);
    EnableWindow(buttons::widgets.hNounButton, !isHomogeneousMode || !hasInput);
    EnableWindow(buttons::widgets.hParticipleButton, !isHomogeneousMode || !hasInput);
    EnableWindow(buttons::widgets.hAdverbialButton, !isHomogeneousMode || !hasInput);

    // Перерисовываем кнопки для обновления их цветов
    InvalidateRect(buttons::widgets.hVerbButton, NULL, TRUE);
    InvalidateRect(buttons::widgets.hAdverbButton, NULL, TRUE);
    InvalidateRect(buttons::widgets.hAdjectiveButton, NULL, TRUE);
    InvalidateRect(buttons::widgets.hNounButton, NULL, TRUE);
    InvalidateRect(buttons::widgets.hParticipleButton, NULL, TRUE);
    InvalidateRect(buttons::widgets.hAdverbialButton, NULL, TRUE);
    InvalidateRect(buttons::widgets.hSearchType, NULL, TRUE);

    // Обновляем окно для применения изменений
    UpdateWindow(buttons::widgets.hVerbButton);
    UpdateWindow(buttons::widgets.hAdverbButton);
    UpdateWindow(buttons::widgets.hAdjectiveButton);
    UpdateWindow(buttons::widgets.hNounButton);
    UpdateWindow(buttons::widgets.hParticipleButton);
    UpdateWindow(buttons::widgets.hAdverbialButton);
    UpdateWindow(buttons::widgets.hSearchType);
}

// Создаем новое окно приложение
WNDCLASS NewWindClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure)
{
    WNDCLASS NWC = {0};

    NWC.hIcon = Icon;
    NWC.hCursor = Cursor;
    NWC.hInstance = hInst;
    NWC.lpszClassName = Name;
    NWC.hbrBackground = BGColor;
    NWC.lpfnWndProc = Procedure;

    return NWC;
}

// Проверка что слово зачеркнуто
bool IsRichEditStrikeout(HWND hRichEdit)
{
    CHARFORMAT2 cf = { 0 };
    cf.cbSize = sizeof(CHARFORMAT2);
    if (SendMessage(hRichEdit, EM_GETCHARFORMAT, SCF_ALL, (LPARAM)&cf))
    {
        return (cf.dwMask & CFM_STRIKEOUT) && (cf.dwEffects & CFE_STRIKEOUT);
    }
    return false;
}

// Установка жирного текста
void SetRichEditBold(HWND hRichEdit, bool bold)
{
    SendMessage(hRichEdit, EM_SETSEL, 0, -1);  // Выделить весь текст

    CHARFORMAT cf = { 0 };
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_BOLD;          // Указываем, что меняем жирность
    cf.dwEffects = bold ? CFE_BOLD : 0; // Включаем/выключаем жирный шрифт

    SendMessage(hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

// Установка зачеркивания
void SetRichEditStrikeout(HWND hRichEdit, bool strikeout)
{
    SendMessage(hRichEdit, EM_SETSEL, 0, -1);  // Выделить весь текст

    CHARFORMAT cf = { 0 };
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_STRIKEOUT;          // Указываем, что меняем зачеркивание
    cf.dwEffects = strikeout ? CFE_STRIKEOUT : 0; // Включаем/выключаем зачеркивание

    SendMessage(hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

// Обновление состояния чекбоксов
void UpdateCheckboxStates()
{
    // Проверка условий
    bool isHomogeneousMode = buttons::ButtonFlags[7];
    bool partOfSpeechSelected;
    int selectedPartsOfSpeech = buttons::ButtonFlags.test(0) + buttons::ButtonFlags.test(1) +
        buttons::ButtonFlags.test(2) + buttons::ButtonFlags.test(3) +
        buttons::ButtonFlags.test(4) + buttons::ButtonFlags.test(5);

    // Получаем текст из поля редактирования
    char buffer[256] = { 0 };
    GetWindowTextA(buttons::widgets.hEditInputWord, buffer, sizeof(buffer));

    if (isHomogeneousMode && strlen(buffer) > 0)
    {
        SetWindowText(buttons::widgets.hStaticCheckBox3Info, L"Однородный режим поиска");
        SetRichEditBold(buttons::widgets.hStaticCheckBox3Info, true);
        SetWindowText(buttons::widgets.hStaticCheckBox1Info, L"Введите слово");
        SetRichEditStrikeout(buttons::widgets.hStaticCheckBox2Info, true);
        partOfSpeechSelected = (selectedPartsOfSpeech == 0);
    }
    else if(isHomogeneousMode && strlen(buffer) == 0)
    {
        SetWindowText(buttons::widgets.hStaticCheckBox3Info, L"Однородный режим поиска");
        SetRichEditBold(buttons::widgets.hStaticCheckBox3Info, true);
        SetWindowText(buttons::widgets.hStaticCheckBox1Info, L"Выберите НЕ МЕНЕЕ ОДНОЙ ч.р.");
        SetRichEditStrikeout(buttons::widgets.hStaticCheckBox2Info, false);
        partOfSpeechSelected = (selectedPartsOfSpeech >= 1);
    }
    else if(!isHomogeneousMode && strlen(buffer) > 0)
    {
        SetWindowText(buttons::widgets.hStaticCheckBox3Info, L"Неоднородный режим поиска");
        SetRichEditBold(buttons::widgets.hStaticCheckBox3Info, true);
        SetWindowText(buttons::widgets.hStaticCheckBox1Info, L"Выберите НЕ МЕНЕЕ ОДНОЙ ч.р.");
        SetRichEditStrikeout(buttons::widgets.hStaticCheckBox2Info, false);
        partOfSpeechSelected = (selectedPartsOfSpeech > 0);
    }
    else if(!isHomogeneousMode && strlen(buffer) == 0)
    {
        SetWindowText(buttons::widgets.hStaticCheckBox3Info, L"Неоднородный режим поиска");
        SetRichEditBold(buttons::widgets.hStaticCheckBox3Info, true);
        SetWindowText(buttons::widgets.hStaticCheckBox1Info, L"Выберите НЕ МЕНЕЕ ДВУХ ч.р.");
        SetRichEditStrikeout(buttons::widgets.hStaticCheckBox2Info, false);
        partOfSpeechSelected = (selectedPartsOfSpeech > 1);
    }

    bool isFileSelected = !filename_str.empty();
    SetRichEditStrikeout(buttons::widgets.hStaticCheckBox2Info, isFileSelected);
    SetRichEditStrikeout(buttons::widgets.hStaticCheckBox1Info, partOfSpeechSelected);

    bool enableSearch = partOfSpeechSelected && isFileSelected;

    EnableWindow(buttons::widgets.hSearch, enableSearch);
    InvalidateRect(buttons::widgets.hSearch, NULL, TRUE);  // Обновим внешний вид

    // Обновляем окна
    UpdateWindow(buttons::widgets.hStaticCheckBox1Info);
    UpdateWindow(buttons::widgets.hStaticCheckBox2Info);
    UpdateWindow(buttons::widgets.hStaticCheckBox3Info);
    UpdateWindow(buttons::widgets.hSearch);
}

// Вывод текста в поле
void OutputTextInfo(const vector<vector<string>>& sentences)
{
    vector<vector<wstring>> Wsentences;

    // Копирование
    for (const vector<string>& sentence : sentences)
    {
        vector<wstring> wsentence;
        for (const string& word : sentence)
        {
            wsentence.push_back(utf8_to_wstring(word)); // Используем лямбда-функцию для преобразования
        }
        Wsentences.push_back(wsentence);
    }
    // Выводим текст в поле
    for (vector<wstring>& sentence : Wsentences)
    {
        bool firstWord = true;
        for (wstring& word : sentence)
        {

            wstring tmp_word = word;
            static const set<wstring> punctuationMarks = { L".", L",", L"!", L"?", L":", L";", L"-", L"(", L")", L"\"", L"'" };

            if (!firstWord && punctuationMarks.find(tmp_word) == punctuationMarks.end())
            {
                // Добавляем пробел перед словом (если это не пунктуация и не первое слово)
                SendMessageW(buttons::widgets.hEditText, EM_REPLACESEL, FALSE, (LPARAM)L" ");
            }

            SendMessageW(buttons::widgets.hEditText, EM_REPLACESEL, FALSE, (LPARAM)tmp_word.c_str());

            firstWord = false;
        }

        // Перенос строки после предложения
        SendMessageW(buttons::widgets.hEditText, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
    }
}

// Вывод рифм в поле
void OutputRhymeInfo(const vector<WordData>& rhymes_data) 
{
    
    // Очищаем поле перед выводом новой информации
    SetWindowTextW(buttons::widgets.hEditRhymes, L"");

    wstring parts_of_speech;

    parts_of_speech += L"Поиск выполнялся по следующим частям речи: " + GetActivePartsOfSpeech(buttons::ButtonFlags) + L"\r\n";
    parts_of_speech += L"Тип поиска: ";
    parts_of_speech += buttons::ButtonFlags.test(7) ? L"Однородный" : L"Неоднородный";
    SendMessageW(buttons::widgets.hEditRhymes, EM_REPLACESEL, FALSE, (LPARAM)parts_of_speech.c_str());

    for (const WordData& output : rhymes_data) {


        // Основная информация о слове
        wstring wordInfo;

            wordInfo = L"\r\n\r\nСлово: ";


        wordInfo += utf8_to_wstring(output.word);

        // Часть речи
        wordInfo += L"\r\nЧасть речи: " + utf8_to_wstring(output.part_of_speech);

        // Количество найденных слов
        wordInfo += L"\r\nКоличество встреч в тексте: " + to_wstring(output.amount);

        // Количество рифм
        //wordInfo += L"\r\nКоличество рифмующихся слов: " + to_wstring(output.rhymed_amount);

        // Добавляем основную информацию
        SendMessageW(buttons::widgets.hEditRhymes, EM_REPLACESEL, FALSE, (LPARAM)wordInfo.c_str());

        // Предложения, где встречается слово
        wstring sentenceInfo = L"\r\nНайдено в предложениях: ";
        if (!output.sentence_counter.empty()) 
        {
            for (size_t i = 0; i < output.sentence_counter.size(); ++i) 
            {
                if (i != 0) sentenceInfo += L", ";
                sentenceInfo += to_wstring(output.sentence_counter[i]);
            }
        }
        else
        {
            sentenceInfo += L"не найдено";
        }
        SendMessageW(buttons::widgets.hEditRhymes, EM_REPLACESEL, FALSE, (LPARAM)sentenceInfo.c_str());

        // Рифмы
        if (!output.rhymed_words.empty()) 
        {
            SendMessageW(buttons::widgets.hEditRhymes, EM_REPLACESEL, FALSE, (LPARAM)L"\r\nРифмующиеся слова:");
            for (const auto& word : output.rhymed_words) 
            {
                wstring rhyme = L"\r\n  • " + utf8_to_wstring(word);
                SendMessageW(buttons::widgets.hEditRhymes, EM_REPLACESEL, FALSE, (LPARAM)rhyme.c_str());
            }
        }
        else
        {
            SendMessageW(buttons::widgets.hEditRhymes, EM_REPLACESEL, FALSE, (LPARAM)L"\r\nРифмующиеся слов нет");
        }
    }

    // Прокручиваем в начало
    SendMessageW(buttons::widgets.hEditRhymes, EM_SETSEL, 0, 0);
    SendMessageW(buttons::widgets.hEditRhymes, EM_SCROLLCARET, 0, 0);
}

wstring GetActivePartsOfSpeech(const bitset<8>& ButtonFlags) 
{
    wstring result;
    bool first = true;

    // Проверяем каждый бит и добавляем соответствующую часть речи
    if (ButtonFlags.test(0)) { // Глагол
        if (!first) result += L", ";
        result += L"Глагол";
        first = false;
    }
    if (ButtonFlags.test(1)) { // Наречие
        if (!first) result += L", ";
        result += L"Наречие";
        first = false;
    }
    if (ButtonFlags.test(2)) { // Прилагательное
        if (!first) result += L", ";
        result += L"Прилагательное";
        first = false;
    }
    if (ButtonFlags.test(3)) { // Существительное
        if (!first) result += L", ";
        result += L"Существительное";
        first = false;
    }
    if (ButtonFlags.test(4)) { // Причастие
        if (!first) result += L", ";
        result += L"Причастие";
        first = false;
    }
    if (ButtonFlags.test(5)) { // Деепричастие
        if (!first) result += L", ";
        result += L"Деепричастие";
        first = false;
    }

    if (result.empty()) {
        result = L"Нет выбранных частей речи";
    }

    return result;
}

// Основной цикл программы
LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static string str_sentences;                        // Строка для хранения текста
    static vector<vector<string>> sentences;    // Вектор текста с разделенными предложениями
    static vector<WordData> rhymes_data;        // Найденные рифмы к данному слову

    switch (msg)
    {
		// Создание окна
    case WM_CREATE:
    {
		// Создание меню
        MainWndAddMenus(hWnd);

		// Создание виджетов
        MainWndAddWidget(hWnd);
        EnableWindow(buttons::widgets.hSaveFile, FALSE);  // Блокировка кнопки
        EnableWindow(buttons::widgets.hSearch, FALSE);  // Блокировка кнопки
        UpdateWindow(buttons::widgets.hSaveFile);
        void UpdateCheckboxStates();

        // Настройка параметров чтения файла
        SetOpenFileParams(hWnd);

		// Сброс флагов кнопок
        buttons::ButtonFlags.reset();

		// Создание фона
        buttons::graphics.hBrush = CreateSolidBrush(RGB(255, 255, 255)); 
        SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)buttons::graphics.hBrush);

        

        // Создание кистей для кнопок
        buttons::graphics.hBrushRed = CreateSolidBrush(RGB(205, 92, 92));
        buttons::graphics.hBrushGreen = CreateSolidBrush(RGB(129, 255, 129));
        buttons::graphics.hBrushGrey = CreateSolidBrush(RGB(178, 178, 178));
        buttons::graphics.hPenBlack = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        buttons::graphics.hBrushNeutral = CreateSolidBrush(RGB(127, 255, 212)); 
		SetWindowTextW(buttons::widgets.hEditInputWord, L"");
        SetWindowTextW(buttons::widgets.hEditText, L"");
        break;
    }

    case WM_COMMAND:
    {
       
        // Проверяем, если уведомление пришло от поля ввода слова
        if (HIWORD(wp) == EN_CHANGE && (HWND)lp == buttons::widgets.hEditInputWord)
        {
            if (buttons::ButtonFlags.test(7) == true)
            {
                buttons::ButtonFlags.reset(0);
                buttons::ButtonFlags.reset(1);
                buttons::ButtonFlags.reset(2);
                buttons::ButtonFlags.reset(3);
                buttons::ButtonFlags.reset(4);
                buttons::ButtonFlags.reset(5);
            }
            UpdateButtonStatesAndColors();
            UpdateCheckboxStates();
        }
		// Нажата кнопка "Выход"
        if (LOWORD(wp) == buttons::buttonIDs.ButExit)
        {
            // Перерисовываем кнопку
            SetWindowText(buttons::widgets.hExitButton, L"");
            InvalidateRect(buttons::widgets.hExitButton, NULL, TRUE); 
            UpdateWindow(buttons::widgets.hExitButton);
            ExitSoftware();
        }
		// Нажата кнопка "Тип поиска"
        else if (LOWORD(wp) == buttons::buttonIDs.ButSearchType)
        {
            buttons::ButtonFlags.flip(7);
            bool hasInput = GetWindowTextLengthW(buttons::widgets.hEditInputWord) > 0;
            if (buttons::ButtonFlags.test(7) == true and hasInput == true)
            {
                buttons::ButtonFlags.reset(0);
                buttons::ButtonFlags.reset(1);
                buttons::ButtonFlags.reset(2);
                buttons::ButtonFlags.reset(3);
                buttons::ButtonFlags.reset(4);
                buttons::ButtonFlags.reset(5);
            }
            // Перерисовываем кнопку
            UpdateButtonStatesAndColors();
            UpdateCheckboxStates();
        }
		// Нажата кнопка "Глагол"
        else if (LOWORD(wp) == buttons::buttonIDs.ButVerb)
        {
            buttons::ButtonFlags.flip(0);
            // Перерисовываем кнопку
            SetWindowText(buttons::widgets.hVerbButton, L"");
            InvalidateRect(buttons::widgets.hVerbButton, NULL, TRUE); // Перерисовываем кнопку
            UpdateWindow(buttons::widgets.hVerbButton);
            UpdateCheckboxStates();
        }
		// Нажата кнопка "Наречие"
        else if (LOWORD(wp) == buttons::buttonIDs.ButAdverb)
        {
            buttons::ButtonFlags.flip(1);
            // Перерисовываем кнопку
            SetWindowText(buttons::widgets.hAdverbButton, L"");
            InvalidateRect(buttons::widgets.hAdverbButton, NULL, TRUE); // Перерисовываем кнопку
            UpdateWindow(buttons::widgets.hAdverbButton);
            UpdateCheckboxStates();
        }
		// Нажата кнопка "Прилагательное"
        else if (LOWORD(wp) == buttons::buttonIDs.ButAdjective)
        {
            buttons::ButtonFlags.flip(2);
            // Перерисовываем кнопку
            SetWindowText(buttons::widgets.hAdjectiveButton, L"");
            InvalidateRect(buttons::widgets.hAdjectiveButton, NULL, TRUE); // Перерисовываем кнопку
            UpdateWindow(buttons::widgets.hAdjectiveButton);
            UpdateCheckboxStates();
        }
		// Нажата кнопка "Существительное"
        else if (LOWORD(wp) == buttons::buttonIDs.ButNoun)
        {
            buttons::ButtonFlags.flip(3);
            // Перерисовываем кнопку
            SetWindowText(buttons::widgets.hNounButton, L"");
            InvalidateRect(buttons::widgets.hNounButton, NULL, TRUE); // Перерисовываем кнопку
            UpdateWindow(buttons::widgets.hNounButton);
            UpdateCheckboxStates();
        }
		// Нажата кнопка "Причастие"
        else if (LOWORD(wp) == buttons::buttonIDs.ButParticiple)
        {
            buttons::ButtonFlags.flip(4);
            // Перерисовываем кнопку
            SetWindowText(buttons::widgets.hParticipleButton, L"");
            InvalidateRect(buttons::widgets.hParticipleButton, NULL, TRUE); // Перерисовываем кнопку
            UpdateWindow(buttons::widgets.hParticipleButton);
            UpdateCheckboxStates();
        }
		// Нажата кнопка "Деепричастие"
        else if (LOWORD(wp) == buttons::buttonIDs.ButAdverbial)
        {
            buttons::ButtonFlags.flip(5);
            // Перерисовываем кнопку
            SetWindowText(buttons::widgets.hAdverbialButton, L"");
            InvalidateRect(buttons::widgets.hAdverbialButton, NULL, TRUE); // Перерисовываем кнопку
            UpdateWindow(buttons::widgets.hAdverbialButton);
            UpdateCheckboxStates();
        }
		// Нажата кнопка "Выход из программы"
        else if (LOWORD(wp) == buttons::buttonIDs.OnExitSoftware)
        {
            ExitSoftware();
        }
		// Нажата кнопка "Поиск"
        else if (LOWORD(wp) == buttons::buttonIDs.Search)
        {
            // Сброс флага поиска слова
			buttons::ButtonFlags[6] = 0; 

			// Получаем текст из поля редактирования
            char WordToSearch[60] = ""; 
            GetWindowTextA(buttons::widgets.hEditInputWord, WordToSearch, 60);

            // слово для поиска по нему рифм
			string word_to_compare = WordToSearch; 
            
			// Проверяем, если поле ввода пустое
			if (word_to_compare.length() != 0)
			{
                buttons::ButtonFlags[6] = true;
			}
			// Проверяем, если кнопки не нажаты
            if (buttons::ButtonFlags.test(0) == 0 and buttons::ButtonFlags.test(1) == 0 and
                buttons::ButtonFlags.test(2) == 0 and buttons::ButtonFlags.test(3) == 0 and
                buttons::ButtonFlags.test(4) == 0 and buttons::ButtonFlags.test(5) == 0 and
                buttons::ButtonFlags.test(6) == 0)
            {
                MessageBoxA(hWnd, "Выберите часть речи или введите слово для поиска", "Ошибка", MB_OK | MB_ICONERROR);
                break;
            }
			// Проверяем, если выбрано несколько частей речи и выбран режим неоднородного поиска
            if (buttons::ButtonFlags.count() < 2 and
                buttons::ButtonFlags.test(7) == 0)
            {
                MessageBoxA(hWnd, "Выберите несколько частей речи или введите слово для поиска", "Ошибка", MB_OK | MB_ICONERROR);
                break;
            }

            // Показываем окно загрузки
            ShowLoadingWindow(hWnd);

            // Получаем найденные рифмы, разделенные предложения и флаги 
            unite_functions(rhymes_data, sentences, str_sentences, word_to_compare, buttons::ButtonFlags);

            // Скрываем окно загрузки
            HideLoadingWindow(hWnd);

            // Очищаем содержимое поля перед добавлением нового текста
            SetWindowTextA(buttons::widgets.hEditRhymes, "");
            SetWindowTextA(buttons::widgets.hEditText, "");
            UpdateWindow(buttons::widgets.hLoadingWnd);

			// Если не найдено рифм, выводим сообщение
            if (rhymes_data.empty())
            {
                MessageBoxA(hWnd, "Не найдено рифм", "Ошибка", MB_OK | MB_ICONERROR);
                HideLoadingWindow(hWnd);
                EnableWindow(buttons::widgets.hSaveFile, FALSE);
                UpdateWindow(buttons::widgets.hSaveFile);
                break;
            }
            else if(sentences.empty())
            {
                MessageBoxA(hWnd, "Не найдено предложений", "Ошибка", MB_OK | MB_ICONERROR);
                EnableWindow(buttons::widgets.hSaveFile, FALSE);
                UpdateWindow(buttons::widgets.hSaveFile);
                break;
            }
            else
            {
                EnableWindow(buttons::widgets.hSaveFile, TRUE);
                UpdateWindow(buttons::widgets.hSaveFile);
            }
            

            // Вывод текста
            OutputTextInfo(sentences);
            
            // Вывод рифм
            OutputRhymeInfo(rhymes_data);

            // Перерисовываем поле текста
            InvalidateRect(buttons::widgets.hEditText, NULL, TRUE);
            UpdateWindow(buttons::widgets.hEditText);
            UpdateWindow(buttons::widgets.hEditRhymes);
            buttons::SaveButtonFlags = buttons::ButtonFlags;
            buttons::ButtonFlags.reset();
            UpdateButtonStatesAndColors();
            UpdateCheckboxStates();


        }
		// Нажата кнопка "Сохранить файл"
		else if (LOWORD(wp) == buttons::buttonIDs.ButSaveFile)
		{
            // ВЫЗОВ ФУНКЦИИ ДЛЯ ВЫВОДА В ФАЙЛ
            string outputFileName_numbered; // имя выходного файла-текста
            string outputFileName_rhymes; // имя выходного файла-рифм
            outputFiles_working(filename_str, outputFileName_numbered, outputFileName_rhymes, sentences, rhymes_data);
            wstring wtext, wrhymes;
            wtext += L"Файл с пронумерованным текстом: " + utf8_to_wstring(ansi_to_utf8(outputFileName_numbered));
            wrhymes += L"Файл с найденными рифмами: " + utf8_to_wstring(ansi_to_utf8(outputFileName_rhymes));
            // Устанавливаем текст в поле "Выбранный файл"
            SetWindowTextW(buttons::widgets.hPathSaveFileData, wtext.c_str());
            SetWindowTextW(buttons::widgets.hPathSaveFileRhymes, wrhymes.c_str());
            UpdateWindow(buttons::widgets.hPathSaveFileData);
            UpdateWindow(buttons::widgets.hPathSaveFileRhymes);
            EnableWindow(buttons::widgets.hSaveFile, FALSE);  // Блокировка кнопки
            UpdateWindow(buttons::widgets.hSaveFile);
		}
        // Нажатие кнопки "чтение файла"
        else if (LOWORD(wp) == buttons::buttonIDs.ButOpenFile)
        {
          
            // Проверка успешности выбора файла
            if (GetOpenFileNameA(&OFN)) // Вызов GetOpenFileNameW
            {
                filename_str = filename;
                pair<bool, string> fromFunct = inputFile_working(filename_str);
                bool fromFunctStatus = fromFunct.first;         // статус проверки файла
                string fromFunctText = fromFunct.second;        // текст в строковой записи или ошибка файла


				if (fromFunctStatus == false)
				{
					MessageBoxA(hWnd, fromFunctText.c_str(), "Ошибка", MB_OK | MB_ICONERROR);
					break;
				}
                
				str_sentences = fromFunctText; // Получаем текст из функции
                SetWindowTextA(buttons::widgets.hOutputStatus, filename_str.c_str());

                // open_file(filename_str);
                UpdateCheckboxStates();
            }
            break;
        }
        // Нажата кнопка "Инфо"
        else if (LOWORD(wp) == buttons::buttonIDs.OnInfoClicked)
        {
            MessageBox(hWnd, L"Информация о программе и ее разработчиках:", L"Инфо", MB_OK | MB_ICONINFORMATION);
        }
        // Нажата кнопка "Справка"
	else if (LOWORD(wp) == buttons::buttonIDs.OnHelp) {
  	  MessageBox(hWnd, 
       		 L"Инструкция по использованию программы:\n\n"
      		 L"1. Для того, чтобы выбрать текстовый файл, в котором нужно провести поиск рифм, нажмите кнопку \"Открыть файл\". В появившимся окне выберите нужный файл в формате txt. Кодировка файла должна быть \"UTF-8\".\n\n"
       		 L"2. Для однородного поиска выставите кнопку \"Режим поиска\" в режим \"однородный\" путём нажатия а неё и выбирите как минимум одну часть речи (если кнопка части речи светится зеленым, значит, соответсвующая часть речи выбрана). Затем нажмите кнопку \"Поиск\".\n\n"
       		 L"3. Для неоднородного поиска выставите кнопку \"Режим поиска\" в режим \"неоднородный\" путем нажатия на нее и выбирите как минимум две части речи. Затем нажмите кнопку \"Поиск\".\n\n"
       		 L"4. Для того, чтобы сохранить файлы рифм и пронумированных предложений, нажмите \"Сохранить файл\". Файлы будут находиться в той же папке, что и исходный файл. Файлы рифм и пронумированных предложений будут иметь приписки \"_rhymes\" и \"_numbered\" соответственно.\n\n"
       		 L"5. Чтобы в тексте искать рифмующиеся слова к определенному слову, введите его в поле \"Слово для поиска рифм\", выберете желаемые настройки (см. выше), и нажмите кнопку \"Поиск\". Все совпадения будут в графе \"Найденные рифмы\".\n\n"
       		 L"6. Чтобы выйти из программы, нажмите кнопку \"Выход\".",
       		 L"Справка", MB_OK | MB_ICONINFORMATION);
	}
        break;
    }
	// Отработка визуализации кнопок
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lp;
        MakeRoundButton(lpDrawItem);
        break;
    }
	// Отработка визуализации картинок и рамок
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Рисуем черную рамку вокруг статических полей

        HPEN hOldPen = (HPEN)SelectObject(hdc, buttons::graphics.hPenBlack);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

        HWND All_hwnd[11] = { buttons::widgets.hOutputStatus, buttons::widgets.hOutputStatusText, 
                             buttons::widgets.hEditRhymes, buttons::widgets.hOutputRhymes, 
                             buttons::widgets.hOutputText, buttons::widgets.hEditText, 
                             buttons::widgets.hInputWord, buttons::widgets.hEditInputWord,
                             buttons::widgets.hPathSaveFileData, buttons::widgets.hPathSaveFileRhymes,
                             buttons::widgets.hPathSaveFileText };

        for (int i = 0; i < 11; i++)
        {
            MakeFrame(hWnd, hdc, All_hwnd[i]);
        }

        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);

        EndPaint(hWnd, &ps);
        break;
    }
	// Отработка визуализации статических полей
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wp;
        HWND hStatic = (HWND)lp;

        if (hStatic == buttons::widgets.hOutputStatus || hStatic == buttons::widgets.hOutputStatusText || hStatic == buttons::widgets.hOutputStatus ||
            hStatic == buttons::widgets.hOutputRhymes || hStatic == buttons::widgets.hOutputText || hStatic == buttons::widgets.hLoadingWnd)
        {
            SetBkColor(hdcStatic, RGB(255, 255, 255)); // Устанавливаем цвет фона (белый)
            SetTextColor(hdcStatic, RGB(0, 0, 0));     // Устанавливаем цвет текста (черный)
            return (INT_PTR)buttons::graphics.hBrush;            // Возвращаем кисть для фона
        }
        break;
    }
	// Отработка визуализации полей ввода
    case WM_CTLCOLOREDIT:
    {
        HDC hdcEdit = (HDC)wp;
        HWND hEdit = (HWND)lp;

        if (hEdit == buttons::widgets.hEditRhymes || hEdit == buttons::widgets.hEditText || hEdit == buttons::widgets.hEditInputWord)
        {
            SetBkColor(hdcEdit, RGB(255, 255, 255)); // Устанавливаем цвет фона (белый)
            SetTextColor(hdcEdit, RGB(0, 0, 0));     // Устанавливаем цвет текста (черный)
            return (INT_PTR)buttons::graphics.hBrush;          // Возвращаем кисть для фона
        }
        break;
    }
    // Отработка завершения работы
    case WM_DESTROY:
    {	// Освобождаем ресурсы
        DeleteObject(buttons::graphics.hBrushRed);
        DeleteObject(buttons::graphics.hBrushGreen);
        DeleteObject(buttons::graphics.hBrushGrey);
        DeleteObject(buttons::graphics.hBrush);
        DeleteObject(buttons::graphics.hBrushNeutral);
        DeleteObject(buttons::graphics.hPenBlack);
        DeleteObject(buttons::graphics.hdcMem);
        ExitSoftware();
        break;
    }
	// Дефолтная обработка сообщений
    default:
        return DefWindowProc(hWnd, msg, wp, lp);
    }
    return 0;
}
// Функция для освобождения памяти, выделенной для LPWSTR
void FreeLPWSTR(LPWSTR lpwstr)
{
    delete[] lpwstr;
}


// Функция для преобразования LPWSTR в std::string
std::string ConvertLPWSTRToString(LPWSTR lpwstr)
{
	// Проверяем, что lpwstr не является нулевым указателем
    if (!lpwstr) {
        std::cerr << "Null LPWSTR" << std::endl;
        return std::string();
    }
	// Преобразуем LPWSTR в std::string
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) {
        DWORD error = GetLastError();
        std::cerr << "WideCharToMultiByte failed, error: " << error << std::endl;
        return std::string();
    }

    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, &result[0], size_needed, nullptr, nullptr);

    // Удаляем завершающий нуль
    if (!result.empty() && result.back() == '\0') {
        result.pop_back();
    }

    return result;
}

// Функция для преобразования std::string в LPWSTR
LPWSTR ConvertStringToLPWSTR(const std::string &str)
{
	// Проверяем, что строка не пустая
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    LPWSTR lpwstr = new WCHAR[size_needed];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, lpwstr, size_needed);
    return lpwstr;
}

// Отрисовка рамок статических полей и полей ввода
void MakeFrame(HWND hWnd, HDC hdc, HWND Edit)
{
	// Получаем размеры элемента управления
    RECT rect;
    GetWindowRect(Edit, &rect);
    MapWindowPoints(HWND_DESKTOP, hWnd, (LPPOINT)&rect, 2);

    // Рисуем черную рамку вокруг элемента управления
    HPEN hOldPen = (HPEN)SelectObject(hdc, buttons::graphics.hPenBlack);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, rect.left - 1, rect.top - 1, rect.right + 1, rect.bottom + 1);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
}

// Функция для создания закругленной кнопки
BOOL MakeRoundButton(LPDRAWITEMSTRUCT lpDrawItem)
{
    HDC hdc = lpDrawItem->hDC;
    RECT rect = lpDrawItem->rcItem;

    // Создаем кисть цвета фона окна (чтобы скрыть старые границы)
    HBRUSH hBgBrush = CreateSolidBrush(RGB(255, 255, 255)); // Цвет фона окна
    FillRect(hdc, &rect, hBgBrush);
    DeleteObject(hBgBrush); // Удаляем кисть после использования

    // Выбираем нужную кисть для кнопки
    HBRUSH hBrushes;
    BOOL isActive = FALSE;
    const char *buttonText;
	// Определяем текст кнопки в зависимости от ее идентификатора
	// Кнопка "Выход"
    if (lpDrawItem->hwndItem == buttons::widgets.hExitButton)
    {
        hBrushes = buttons::graphics.hBrushRed;
        buttonText = "Выход";
    }
	// Кнопка "Глагол"
    else if (lpDrawItem->hwndItem == buttons::widgets.hVerbButton)
    {
        isActive = buttons::ButtonFlags.test(0);
        hBrushes = isActive ? buttons::graphics.hBrushGreen : buttons::graphics.hBrushGrey;
        buttonText = "Глагол";
    }

	// Кнопка "Наречие"
    else if (lpDrawItem->hwndItem == buttons::widgets.hAdverbButton)
    {
        isActive = buttons::ButtonFlags.test(1);
        hBrushes = isActive ? buttons::graphics.hBrushGreen : buttons::graphics.hBrushGrey;
        buttonText = "Наречие";
    }

	// Кнопка "Прилагательное"
    else if (lpDrawItem->hwndItem == buttons::widgets.hAdjectiveButton)
    {
        isActive = buttons::ButtonFlags.test(2);
        hBrushes = isActive ? buttons::graphics.hBrushGreen : buttons::graphics.hBrushGrey;
        buttonText = "Прилагательное";
    }

	// Кнопка "Существительное"
    else if (lpDrawItem->hwndItem == buttons::widgets.hNounButton)
    {
        isActive = buttons::ButtonFlags.test(3);
        hBrushes = isActive ? buttons::graphics.hBrushGreen : buttons::graphics.hBrushGrey;
        buttonText = "Существительное";
    }

	// Кнопка "Причастие"
    else if (lpDrawItem->hwndItem == buttons::widgets.hParticipleButton)
    {
        isActive = buttons::ButtonFlags.test(4);
        hBrushes = isActive ? buttons::graphics.hBrushGreen : buttons::graphics.hBrushGrey;
        buttonText = "Причастие";
    }

	// Кнопка "Деепричастие"
    else if (lpDrawItem->hwndItem == buttons::widgets.hAdverbialButton)
    {
        isActive = buttons::ButtonFlags.test(5);
        hBrushes = isActive ? buttons::graphics.hBrushGreen : buttons::graphics.hBrushGrey;
        buttonText = "Деепричастие";
    }

	// Кнопка "Режим поиска"
    else if (lpDrawItem->hwndItem == buttons::widgets.hSearchType)
    {
        isActive = buttons::ButtonFlags.test(7);
        hBrushes = buttons::graphics.hBrushNeutral;
        buttonText = isActive ? "Режим поиска: однородный" : "Режим поиска: неоднородный";
    }

	// Кнопка "Поиск"
    else if (lpDrawItem->hwndItem == buttons::widgets.hSearch)
    {
        BOOL enabled = IsWindowEnabled(buttons::widgets.hSearch);
        hBrushes = enabled ? buttons::graphics.hBrushNeutral : buttons::graphics.hBrushGrey;
        buttonText = "Поиск";
    }
    // Кнопка "Открыть файл"
    else if (lpDrawItem->hwndItem == buttons::widgets.hOpenFile)
    {
        hBrushes = buttons::graphics.hBrushNeutral;
        buttonText = "Открыть файл";
    }
    // Кнопка "Сохранить файл"
    else if (lpDrawItem->hwndItem == buttons::widgets.hSaveFile)
    {
        // Проверяем, активна ли кнопка "Сохранить файл"
        if (IsWindowEnabled(lpDrawItem->hwndItem)) {
            hBrushes = buttons::graphics.hBrushNeutral; // Голубой (если активна)
        }
        else {
            hBrushes = buttons::graphics.hBrushGrey; // Серый (если заблокирована)
        }
        buttonText = "Сохранить файл";
    }

	// Иное значение кнопки
    else
    {
        return FALSE; // Неизвестная кнопка
    }

    HPEN hPen = NULL;
    HPEN hOldPen = NULL;
    HBRUSH hOldBrush = NULL;
     
	// Создаем обрамление для кнопки
    if (lpDrawItem->hwndItem == buttons::widgets.hExitButton ||
        lpDrawItem->hwndItem == buttons::widgets.hSearch ||
        lpDrawItem->hwndItem == buttons::widgets.hSearchType ||
        lpDrawItem->hwndItem == buttons::widgets.hOpenFile ||
        lpDrawItem->hwndItem == buttons::widgets.hSaveFile)
    {
        hPen = CreatePen(PS_SOLID, 1, RGB(0, 210, 210)); // Синее обрамление для голубой кнопки
    }
    else
    {
        hPen = CreatePen(PS_SOLID, 1, isActive ? RGB(0, 200, 0) : RGB(200, 0, 0));
    }

	// Устанавливаем цвет текста и кисть для кнопки
    hOldPen = (HPEN)SelectObject(hdc, hPen);
    hOldBrush = (HBRUSH)SelectObject(hdc, hBrushes);
    SetTextColor(hdc, RGB(0, 0, 0));

    // Если кнопка нажата, смещаем прямоугольник и изменяем цвет фона
    if (lpDrawItem->itemState & ODS_SELECTED)
    {
        OffsetRect(&rect, 4, 4);
        rect.right -= 4;
        rect.bottom -= 4;
    }

    // Рисуем закругленный прямоугольник
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 25, 25);

    // Отключаем стандартный фон и устанавливаем цвет текста
    SetBkMode(hdc, TRANSPARENT);

    // Центрируем текст внутри кнопки
    DrawTextA(hdc, buttonText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Очищаем ненужные ресурсы GDI
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    return TRUE;
}

// Завершение работы приложения
void ExitSoftware()
{
    PostQuitMessage(0);
}

// Создание пунктов меню
void MainWndAddMenus(HWND hWnd)
{
    HMENU RootMenu = CreateMenu();
    HMENU SubMenu = CreateMenu();

    // Создание основного меню
    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"Файл");

    AppendMenu(RootMenu, MF_STRING, buttons::buttonIDs.OnHelp, L"Справка"); // Пункт справки

    // Создание подменю Файл

    AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(SubMenu, MF_STRING, buttons::buttonIDs.OnExitSoftware, L"Завершить работу");

    SetMenu(hWnd, RootMenu);
}

// Создание виджетов
void MainWndAddWidget(HWND hWnd)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN) - 45;

    // Пропорции для кнопок и виджетов
    int buttonWidth = screenWidth * 15 / 100;  // 15% от ширины экрана
    int buttonHeight = screenHeight * 4 / 100; // 5% от высоты экрана
    int marginX = screenWidth * 2 / 100;       // 2% от ширины экрана
    int marginY = screenHeight * 2 / 100;      // 2% от высоты экрана

    // Кнопки
    buttons::widgets.hExitButton = CreateButton("Выход", marginX, screenHeight - buttonHeight - marginY-80, buttonWidth, buttonHeight, 
        hWnd, buttons::buttonIDs.ButExit);
    buttons::widgets.hVerbButton = CreateButton("Глагол", marginX, marginY + buttonHeight + marginY + 20, buttonWidth, buttonHeight, 
        hWnd, buttons::buttonIDs.ButVerb);
    buttons::widgets.hAdjectiveButton = CreateButton("Прилагательное", marginX, marginY + 2 * (buttonHeight + marginY) + 20, buttonWidth, buttonHeight, 
        hWnd, buttons::buttonIDs.ButAdjective);
    buttons::widgets.hNounButton = CreateButton("Существительное", marginX, marginY + 3 * (buttonHeight + marginY) + 20, buttonWidth, buttonHeight, 
        hWnd, buttons::buttonIDs.ButNoun);
    buttons::widgets.hAdverbialButton = CreateButton("Деепричастие", marginX, marginY + 4 * (buttonHeight + marginY) + 20, buttonWidth, buttonHeight, 
        hWnd, buttons::buttonIDs.ButAdverbial);
    buttons::widgets.hParticipleButton = CreateButton("Причастие", marginX, marginY + 5 * (buttonHeight + marginY) + 20, buttonWidth, buttonHeight, 
        hWnd, buttons::buttonIDs.ButParticiple);
    buttons::widgets.hAdverbButton = CreateButton("Наречие", marginX, marginY + 6 * (buttonHeight + marginY) + 20, buttonWidth, buttonHeight, 
        hWnd, buttons::buttonIDs.ButAdverb);
    buttons::widgets.hSearchType = CreateButton("Тип поиска", marginX, marginY + 7 * (buttonHeight + marginY) + 20, buttonWidth, buttonHeight, 
        hWnd, buttons::buttonIDs.ButSearchType);
    buttons::widgets.hSearch = CreateButton("Поиск", marginX, screenHeight - 2 * (buttonHeight + marginY) - 100, buttonWidth, buttonHeight, 
        hWnd, buttons::buttonIDs.Search);
    buttons::widgets.hOpenFile = CreateButton("Открыть файл", 2 * marginX + buttonWidth, marginY-10, buttonWidth, 30,
        hWnd, buttons::buttonIDs.ButOpenFile);
    buttons::widgets.hSaveFile = CreateButton("Сохранить файл", 2 * marginX + buttonWidth, marginY+30, buttonWidth, 30,
        hWnd, buttons::buttonIDs.ButSaveFile);
    

    // Статические элементы
    buttons::widgets.hOutputStatusText = CreateRichEdit(L"Открыт файл: ", 
        10*marginX + buttonWidth, marginY-10, buttonWidth-80, 30, hWnd, true);
    buttons::widgets.hPathSaveFileText = CreateRichEdit(L"Сохранения: ",
        10 * marginX + buttonWidth, marginY+30, buttonWidth - 80, 30, hWnd, true);
    buttons::widgets.hInputWord = CreateRichEdit(L"Слово для поиска рифм", 
        marginX, marginY + 7 * (buttonHeight + marginY) + 20 + buttonHeight + 11, buttonWidth, 30, hWnd, true);
    buttons::widgets.hOutputRhymes = CreateRichEdit(L"Найденные рифмы", 
        marginX + buttonWidth + marginX, marginY + buttonHeight + marginY+50, 
        (screenWidth - (2 * marginX + buttonWidth + marginX)) / 2 - 1, buttonHeight, hWnd, true);
    buttons::widgets.hOutputText = CreateRichEdit(L"Текст с найденными рифмами", 
        marginX + buttonWidth + marginX + (screenWidth - (2 * marginX + buttonWidth + marginX)) / 2, marginY + buttonHeight + marginY+50, 
        (screenWidth - (2 * marginX + buttonWidth + marginX)) / 2, buttonHeight, hWnd, true);


    // Поля редактирования
    buttons::widgets.hOutputStatus = CreateRichEdit(L"",10*marginX + buttonWidth + buttonWidth - 79, 
        marginY-10, buttonWidth*3, 30, hWnd, true);
    buttons::widgets.hPathSaveFileData = CreateRichEdit(L"", 10 * marginX + buttonWidth + buttonWidth - 79,
        marginY+30, buttonWidth * 3, 30, hWnd, true);
    buttons::widgets.hPathSaveFileRhymes = CreateRichEdit(L"", 10 * marginX + buttonWidth + buttonWidth - 79,
        marginY + 61, buttonWidth * 3, 30, hWnd, true);
    buttons::widgets.hEditInputWord = CreateRichEdit(L"",marginX,
        marginY + 7 * (buttonHeight + marginY) + 30 + buttonHeight + 32, buttonWidth, 60, hWnd);
    SendMessage(buttons::widgets.hEditInputWord, EM_SETEVENTMASK, 0, ENM_CHANGE);

    // Верхняя граница для полей редактирования (вплотную с hOutputRhymes и hOutputText)
    int editTopMargin = marginY + buttonHeight + marginY + buttonHeight + 1; 

    // Высота полей с учетом зазоров
    int editHeight = screenHeight - editTopMargin - 2 * (buttonHeight + marginY) - 1; 
    
    buttons::widgets.hEditRhymes = CreateRichEdit(L"",marginX + buttonWidth + marginX, editTopMargin + 50,
        (screenWidth - (2 * marginX + buttonWidth + marginX)) / 2 - 1, editHeight-50, hWnd, true);
    buttons::widgets.hEditText = CreateRichEdit(L"text",marginX + buttonWidth + marginX + (screenWidth - (2 * marginX + buttonWidth + marginX)) / 2, editTopMargin + 50,
        (screenWidth - (2 * marginX + buttonWidth + marginX)) / 2, editHeight-50, hWnd, true);
    

    
    buttons::widgets.hStaticCheckBox1Info = CreateRichEdit(L"Выберите НЕ МЕНЕЕ ДВУХ ч.р.",
        marginX, screenHeight - 2 * (buttonHeight + marginY) - 143,
        buttonWidth, 30, hWnd, true); 

    buttons::widgets.hStaticCheckBox2Info = CreateRichEdit(L"Выберите файл",
        marginX, screenHeight - 2 * (buttonHeight + marginY) - 173,
        buttonWidth, 30, hWnd, true);

    buttons::widgets.hStaticCheckBox3Info = CreateRichEdit(L"Неоднородный режим поиска",
        marginX, screenHeight - 2 * (buttonHeight + marginY) - 203,
        buttonWidth, 30, hWnd, true);

}

HWND CreateRichEdit(LPCWSTR text, int x, int y, int width, int height, HWND hParent, bool readonly)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN) - 45;

    // Пропорции для кнопок и виджетов
    int buttonWidth = screenWidth * 15 / 100;  // 15% от ширины экрана
    int buttonHeight = screenHeight * 4 / 100; // 5% от высоты экрана
    int marginX = screenWidth * 2 / 100;       // 2% от ширины экрана
    int marginY = screenHeight * 2 / 100;      // 2% от высоты экрана
    // Верхняя граница для полей редактирования (вплотную с hOutputRhymes и hOutputText)
    int editTopMargin = marginY + buttonHeight + marginY + buttonHeight + 1;

    // Высота полей с учетом зазоров
    int editHeight = screenHeight - editTopMargin - 2 * (buttonHeight + marginY) - 1;

    // Убедись, что библиотека загружена
    static bool richEditLoaded = false;
    if (!richEditLoaded) {
        LoadLibrary(TEXT("Msftedit.dll"));
        richEditLoaded = true;
    }
    DWORD style = WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL | WS_BORDER;
    if (readonly)
    {
        style |= ES_READONLY;
    }
    if (text == L"text")
    {
        style |= ES_LEFT;
    }
    else
    {
        style |= ES_CENTER;
    }
    HWND hRich = CreateWindowEx(
        0,
        MSFTEDIT_CLASS,  // "RichEdit50W"
        text,
        style,
        x, y, width, height,
        hParent, NULL, GetModuleHandle(NULL), NULL
    );

    if (!hRich) return NULL;

    // Если нужно сделать текст жирным
    if (text == L"Неоднородный режим поиска" or text == L"Найденные рифмы" or text == L"Текст с найденными рифмами"
        or text == L"Открыт файл: " or text == L"Сохранения: " or text == L"Слово для поиска рифм")
    {
        CHARFORMAT2 cf = { 0 };
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_BOLD;  // Указываем, что меняем жирность
        cf.dwEffects = CFE_BOLD; // Включаем жирный шрифт

        // Выделяем весь текст и применяем форматирование
        SendMessage(hRich, EM_SETSEL, 0, -1);
        SendMessage(hRich, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    }

    return hRich;
}

// Функция для создания кнопки
HWND CreateButton(const char* text, int x, int y, int width, int height, HWND hWnd, int id)
{
	// Создаем кнопку с заданными параметрами
    HWND hButton = CreateWindowA(
        "BUTTON", text, WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        x, y, width, height, hWnd, (HMENU)id, NULL, NULL);
	// Проверяем, была ли кнопка успешно создана
    if (!hButton)
    {
        MessageBoxA(hWnd, "Не удалось создать кнопку", "Ошибка", MB_OK | MB_ICONERROR);
    }

    return hButton;
}

// Функция для создания статического текста
HWND CreateStatic(const char* text, int x, int y, int width, int height, HWND hWnd)
{
	// Создаем статический текст с заданными параметрами
    HWND hStatic = 0;
	// Устанавливаем стиль текста в зависимости от текста
    if (text == "Открыт файл: " or text == "Сохранения: " or text == "Выберите НЕ МЕНЕЕ ДВУХ ч.р." or text == "Выберите файл" or text == "Неоднородный режим поиска")
    {
        hStatic = CreateWindowA(
            "static", text, WS_VISIBLE | WS_CHILD | ES_LEFT,
            x, y, width, height, hWnd, NULL, NULL, NULL);
    }
	// Если текст не соответствует заданным условиям, создаем обычный статический текст
    else
    {
        hStatic = CreateWindowA(
            "static", text, WS_VISIBLE | WS_CHILD | ES_CENTER,
            x, y, width, height, hWnd, NULL, NULL, NULL);
    }
	// Проверяем, был ли статический текст успешно создан
    if (!hStatic)
    {
        MessageBoxA(hWnd, "Не удалось создать статический текст", "Ошибка", MB_OK | MB_ICONERROR);
    }

    return hStatic;
}

// Функция для создания поля редактирования
HWND CreateEdit(int x, int y, int width, int height, HWND hWnd, bool readOnly)
{
    LoadLibraryA("Msftedit.dll");
	// Создаем поле редактирования с заданными параметрами
    DWORD style = WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL;
	// Если поле редактирования только для чтения, добавляем соответствующий стиль
    if (readOnly)
    {
        style |= ES_READONLY;
    }
	// Создаем поле редактирования
    HWND hEdit = CreateWindowExA(
        0, "RICHEDIT50W", "", style,
        x, y, width, height,
        hWnd, NULL, GetModuleHandle(NULL), NULL);
	// Проверяем, было ли поле редактирования успешно создано
    if (!hEdit)
    {
        MessageBoxA(hWnd, "Не удалось создать поле редактирования", "Ошибка", MB_OK | MB_ICONERROR);
    }

    // Снимаем лимит, RichEdit и так поддерживает большие объёмы, но можно явно задать
    SendMessageA(hEdit, EM_EXLIMITTEXT, 0, (WPARAM)-1); // -1 = максимально возможное

    return hEdit;
}


// Установка начальных параметров на открытие файлов для чтения и записи
void SetOpenFileParams(HWND hWnd)
{
	// Устанавливаем начальные параметры для открытия файла
    ZeroMemory(&OFN, sizeof(OFN));
    OFN.lStructSize = sizeof(OFN);
    OFN.hwndOwner = hWnd;
    OFN.lpstrFile = filename;
    OFN.nMaxFile = sizeof(filename);
    OFN.lpstrFilter = "*.txt";
    OFN.lpstrFileTitle = NULL;
    OFN.nMaxFileTitle = 0;
    OFN.lpstrInitialDir = "D:\\";
    OFN.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

// Записать в статус программы
void SetWinStatus(string status)
{
    SetWindowTextA(buttons::widgets.hOutputStatus, status.c_str());
}

void ShowLoadingWindow(HWND hWnd) {

    // Отключаем все кнопки, кроме кнопок тулбара
    EnableWindow(buttons::widgets.hVerbButton, FALSE);
    EnableWindow(buttons::widgets.hAdjectiveButton, FALSE);
    EnableWindow(buttons::widgets.hNounButton, FALSE);
    EnableWindow(buttons::widgets.hAdverbialButton, FALSE);
    EnableWindow(buttons::widgets.hParticipleButton, FALSE);
    EnableWindow(buttons::widgets.hAdverbButton, FALSE);
    EnableWindow(buttons::widgets.hSearchType, FALSE);
    EnableWindow(buttons::widgets.hSearch, FALSE);
    EnableWindow(buttons::widgets.hOpenFile, FALSE);

    // Получаем координаты окон "Найденные рифмы" и "Текст с найденными рифмами"
    RECT rectRhymes, rectText;
    GetWindowRect(buttons::widgets.hEditRhymes, &rectRhymes);
    GetWindowRect(buttons::widgets.hEditText, &rectText);

    int centerX = (rectRhymes.left + rectRhymes.right) - 350;
    int centerY = rectRhymes.top + 300;
    centerX -= 150; centerY -= 50;

    // Получаем координаты окон "Найденные рифмы" и "Текст с найденными рифмами"
    if (buttons::widgets.hLoadingWnd == NULL) {
        buttons::widgets.hLoadingWnd = CreateWindowEx(
            WS_EX_TOPMOST,
            L"STATIC",
            L"Поиск рифм... Пожалуйста, подождите.",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            CW_USEDEFAULT, CW_USEDEFAULT, 300, 100,
            hWnd,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );

        RECT rectInputWord;
        GetWindowRect(buttons::widgets.hEditInputWord, &rectInputWord);
        SetWindowPos(buttons::widgets.hLoadingWnd, HWND_TOP, centerX, centerY, 300, 100, SWP_SHOWWINDOW);
        SetWindowPos(buttons::widgets.hLoadingWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW);
    }
    else {
        ShowWindow(buttons::widgets.hLoadingWnd, SW_SHOW);
    }
    UpdateWindow(buttons::widgets.hLoadingWnd);

}


void HideLoadingWindow(HWND hWnd) {
    if (buttons::widgets.hLoadingWnd != NULL) {
        ShowWindow(buttons::widgets.hLoadingWnd, SW_HIDE);
        // Включаем все кнопки
        EnableWindow(buttons::widgets.hVerbButton, TRUE);
        EnableWindow(buttons::widgets.hAdjectiveButton, TRUE);
        EnableWindow(buttons::widgets.hNounButton, TRUE);
        EnableWindow(buttons::widgets.hAdverbialButton, TRUE);
        EnableWindow(buttons::widgets.hParticipleButton, TRUE);
        EnableWindow(buttons::widgets.hAdverbButton, TRUE);
        EnableWindow(buttons::widgets.hSearchType, TRUE);
        EnableWindow(buttons::widgets.hSearch, TRUE);
        EnableWindow(buttons::widgets.hOpenFile, TRUE);

        SetFocus(hWnd);
    }
}

