// при вводе слов, программа должна автоматически по алфавиту выставлять список слов.
// ВНИМАНИЕ!!! Данная реализация поддерживает только однобайтовые кодировки символов.
// Для переходя к многобайтовым символам надо внести изменения в функции:
//		int code_of(char ch)
//		void string_sort(std::vector<std::string> &unsorted, std::vector<std::string> &sorted, int level)

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <clocale>
#include <functional> 
#include <cctype>
#include <locale>

using namespace std;

// Используемый алфавит
// Определяет очерёдность
char alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZабвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" ;

// Сортировать с учётом/без учёта регистра символов
enum t_compare_mode {
	CASEINSENSITIVE = 0,
	CASESENSITIVE = 1
};
t_compare_mode compare_mode = CASEINSENSITIVE;

// Удалять/не удалять дубликаты строк
enum t_duplicate_mode {
	UNIQUE = 0,
	DUPLICATE = 1
};
t_duplicate_mode duplicate_mode = UNIQUE;

// Разбивать/не разбивать строки на слова
enum t_split_mode {
	SPLIT = 0,
	NOSPLIT = 1
};
t_split_mode split_mode = SPLIT;

// Разбивать/не разбивать строки на слова
enum t_trim_mode {
	TRIM = 0,
	NOTRIM = 1
};
t_trim_mode trim_mode = TRIM;


// Определение номера корзины по символу
// с использованием алфавита
int index_of(char ch)
{
	//A pointer to the first occurrence of character in str.
	//If the character is not found, the function returns a null pointer.
	char * pos = strchr (alphabet, ch);
	if(pos == NULL) return -1;
	return (int)(pos-alphabet);
}

// Определение номера корзины по символу
// без использования алфавита
int code_of(char ch)
{
	return (int)(unsigned)ch;
}

char * input_file_name = NULL;
char * output_file_name = NULL;

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::ptr_fun<int, int>(std::isalpha)));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::ptr_fun<int, int>(std::isalpha)).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

void append_to(std::vector<std::string> &sorted, std::string s)
{
	switch(duplicate_mode)
	{
		case UNIQUE:
			{
				if(sorted.size()==0) sorted.push_back(s);
				else {
					std::string last = sorted.back();
					if(s.compare(last)!=0) sorted.push_back(s);
				}
			}
			break;
		default:
			sorted.push_back(s);
	}
}

// Рекурсивная сортировка строк, основанная на алгоритме корзинной сортировки
void string_sort(std::vector<std::string> &unsorted, std::vector<std::string> &sorted, int level)
{
	int n = strlen(alphabet);
	std::vector<std::vector<std::string>> buckets(n+256);

	// Разбрасываем несортированную коллекцию строк по корзинам
	// Номер корзины уже сразу даёт предварительную сортировку
	for(int i=unsorted.size(); i--; )
	{
		std::string s = unsorted.back();
		unsorted.pop_back();

		char ch = s[level];
		if(ch == NULL) {
			append_to(sorted,s);
			continue;
		}

		int index = index_of(ch);
		if(index==-1) index=n+code_of(ch);
		buckets[index].push_back(s);
	}

	// Применяем рекурсивно ко всем корзинам кроме последней
	for(int i=0; i<n+256; i++)
		switch (buckets[i].size())
		{
			case 0:
				break;
			case 1:
				{
					std::string s = buckets[i].back();
					buckets[i].pop_back();
					append_to(sorted,s);
				}
				break;
			default:
				string_sort(buckets[i], sorted, level+1);
				break;
		}
}

int main(int argc, char* argv[])
{
	// Поддержка кириллицы в консоли Windows
	// Функция setlocale() имеет два параметра, первый параметр - тип категории локали, в нашем случае LC_TYPE - набор символов, второй параметр — значение локали. 
	// Вместо второго аргумента можно писать "Russian", или оставлять пустые двойные кавычки, тогда набор символов будет такой же как и в ОС.
    setlocale(LC_ALL, "");

	for(int i=1; i<argc; i++)
		if(strcmp(argv[i],"-help")==0) 
		{
			std::cout << "Преобразование строки по принципу словаря" << std::endl;
			std::cout << "сортировка строк, основанная на алгоритме корзинной сортировки" << std::endl;
			std::cout << "полиминальная сложность имеет оценку O(N*L)" << std::endl;
			std::cout << "где N - число строк, L - средняя длина строки" << std::endl;
			std::cout << "что лучше алгоритма Быстрой сортировки с оценкой O(N*logN*L)" << std::endl;
		}
		else if(strcmp(argv[i],"-cs")==0) compare_mode = CASESENSITIVE;
		else if(strcmp(argv[i],"-cis")==0) compare_mode = CASEINSENSITIVE;
		else if(strcmp(argv[i],"-unique")==0) duplicate_mode = UNIQUE;
		else if(strcmp(argv[i],"-duplicate")==0) duplicate_mode = DUPLICATE;
		else if(strcmp(argv[i],"-split")==0) split_mode = SPLIT;
		else if(strcmp(argv[i],"-nosplit")==0) split_mode = NOSPLIT;
		else if(strcmp(argv[i],"-trim")==0) trim_mode = TRIM;
		else if(strcmp(argv[i],"-notrim")==0) trim_mode = NOTRIM;
		else if(strcmp(argv[i],"-input")==0) input_file_name = argv[++i];
		else if(strcmp(argv[i],"-output")==0) output_file_name = argv[++i];
	

	if(input_file_name!=NULL) freopen(input_file_name,"r",stdin);
	if(output_file_name!=NULL) freopen(output_file_name,"w",stdout);

	std::vector<std::string> unsorted;
	std::vector<std::string> sorted;

	// Буфер для ввода строк
    std::string buffer = "";

    std::cout << "Вводите строки. "
              << "По окончание ввода введите Ctrl+Z" << std::endl;

    while (std::getline(std::cin, buffer))
	{
        if (buffer.size() > 0) {
			
			// https://notfaq.wordpress.com/2007/08/04/cc-convert-string-to-upperlower-case/
			switch (compare_mode)
			{
			case CASEINSENSITIVE:
				std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::toupper);
				break;
			case CASESENSITIVE:
				break;
			}

			switch (split_mode)
			{
			case SPLIT:
				{
					std::istringstream ss(buffer);
					std::string word;
					while(ss>>word)
					{
						if(trim_mode == TRIM) word = trim(word);
						if(word!="") unsorted.push_back(word);
					}
				}
				break;
			case NOSPLIT:
				{
					if(trim_mode == TRIM) buffer = trim(buffer);
					// Добавление элемента в конец вектора
					if(buffer!="") unsorted.push_back(buffer);
				}
				break;
			}
        }
    }

	string_sort(unsorted, sorted, 0);

	// Сохраняем количество элементов вектора 
    unsigned int vector_size = sorted.size();

	// Вывод заполненного вектора на экран
    std::cout << "Ваш словарь ( " << vector_size << " слов )." << std::endl;
    for (int i = 0; i < vector_size; i++) {
        std::cout << sorted[i] << std::endl; 
    }

	getchar();
	getchar();

	return 0;
}