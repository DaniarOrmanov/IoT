#include <stdio.h>
#include <unistd.h>             //библиотека для обработки аргументов\
                                                       командной строки
#include "temp_functions.h"     //файл с функциями

#define LEN_NAME 10             //строка для названий месяцев

int main(int argc, char *argv[]) {
	
	FILE *f;             //указатель на файл данных
	FILE *f_error;       //указатель на файл ошибок
	FILE *f_ds;          //указатель на файл с описание программы
	sensor tmp;          //структура данных
	short key = 0;         //ключ, введенный при запуске программы
	short month_number = -2;  //номер обрабатываемого месяца. -2 - номер\
	                           не был введен, -1 - ошибка ввода месяца,\
	                           0 - расчет ведем за весь год, от 1 до 12 -\
	                                 расчет ведем по выбронному месяцу
	char* p_name;         //указатель на имя введенного файла данных
	char ref_ext[] = ".csv";          //образец расширения файла данных
	short begin_month, end_month;     //начальный и конечный номера месяцев,\
	                                         которые будем обрабатывать
	short flag_f = 0;   //если 0 - значит ключ "f" не вводился,\
	                           1 - расширение неверно,\
	                           2 - файл не найден,\
	                           3 - файл найден и открыт    
	short flag_enter = 0;  //0 - введен хотя бы один неизвестный ключ,\
	                                         или не введен ни один ключ
	char month[13][LEN_NAME]={"Year","January","February","March","April",\
		"May","June","July","August","September","October","November","December"};
	char* month_name;                     //название выбранного месяца
	unsigned long int row;   //количество строк в данных об одном месяце
	float sum_tmp[2] = {0};  //суммарное занчение температуры за месяц / за год
	float ave_tmp[13];       //средние значения температура за каждый месяц /за год
	short max_tmp[13];       //максимальные значения температуры за каждый месяц / за год
	short min_tmp[13];       //мимимальные значения температуры за каждый месяц / за год
	short count = 0;         //счетчик месяцев, в которых есть данные
	char simbol;             //символ из файла с описанием программы
	
	f_error = fopen("temp_error.csv", "w+");   //открываем файл ошибок\
	                                                   на редактирование
	opterr = 0;               //чтобы не выводились ошибки ввода ключей
		
	while ((key = getopt(argc, argv, "hf:m:")) != -1) {  //считываем ключи,\
		                                     введенные при запуске программы
		flag_enter = 1;		                //введен хотя бы один ключ
		
		if (key == 'h') {                   //если "h", то 
			f_ds = fopen("datasheet.txt", "r");   //открываем файл\
			                                       с описанием программы
			while ((fscanf(f_ds, "%c", &simbol)) == 1) {
				printf("%c", simbol);       //выводим описание программы
				}
				
			fclose(f_ds);                   //закрываем файл
			exit(0);                        //выходим из программы
			}
			
		else if (key == 'f') {         //если "f", то 
			p_name = if_f(key, optarg);   //функция возвращает имя файла.\
			                              если имя файла неверное, то\
			                              первый символ заменяется на "|" или ":"
			
			if (*p_name == '|') {     //если первый символ == "|", то
				flag_f = 1;          //флаг того, что не то расширение
				}
			else if (*p_name == ':') {       //если первый символ == ":", то
				flag_f = 2;                 //флаг того, что файл не найден
				}
			else {                        //если имя файла введено верно, то
				f = fopen(p_name, "r");  //открываем файл
				flag_f = 3;
				}
			}	
		
		else if (key == 'm') {                  //если "m", то 
			month_number = if_m(key, optarg);   //функция возвращает номер месяца или\
			                                    0 если обрабатываем целый год
			month_name = month[month_number];   //записываем название месяца или слово "year"
			}		
	
		else if (key == '?') {                 //если ключ введен неверно, то
			flag_enter = 0;                    //был введен неизвестный ключ
			break;
			}
		}
	
	//функция выводит сообщения о введенных данных перед основными расчетами	
	month_number = comments(flag_enter, flag_f, month_number, ref_ext, p_name, month_name);	
	
	//Формируем данные для цикла	
	if (month_number == 0) {       //если номер месяца равен 0, то
		begin_month = 1;           //обрабатываем в цикле весь год
		end_month = 13;
		}
	else {                               //если номер месяца не равен 0, то
		begin_month = month_number;      //обрабатываем только этот месяц
		end_month = month_number + 1;
		}
	
	max_tmp[12] = -100;         //задаем начальное максимальное значение\
	                                                  температуры в году
	min_tmp[12] = 100;          //задаем начальное минимальное значение\
	                                                 температуры в году
	for (short i = begin_month; i < end_month; i++) {    //обрабатываем\
		                                    каждый месяц по отдельности
		sum_tmp[0] = 0;             //обнуляем сумму значений температуры\
		                                                в каждом месяце
		max_tmp[i - 1] = -100;      //задаем начальное максимальное\
		                           значение температуры в текущем месяце
		min_tmp[i - 1] = 100;       //задаем начальное минимальное\
		                          значение температуры в текущем месяце  
		
		//функция считывает строки из файла и заполняет структуру данными\
		только из правильных строк. Возвращает номер правильной строки структуры
		row = file_to_struct(&tmp, f, f_error, i); 
		
		if (row == 0) {         //если вернулся 0, то 
			continue;           //пропускаем данный месяц. в нем нет данных
			}
		else {                //если вернулся не ноль, то
			count++;          //инкремент счетчика месяцев, в которых есть данные 
			}
		//выполняем расчеты для данных структуры текущего месяца
		for (int j = 0; j < row; j++) {        //цикл по строкам структуры\
			                                             текущего месяца
			sum_tmp[0] += tmp.temperature[j];  //считаем суммарную температуру
			if (max_tmp[i - 1] < tmp.temperature[j]) {  //находим максимальную\
				                                 температуру в текущем месяце
				max_tmp[i - 1] = tmp.temperature[j];
				}
			if (min_tmp[i - 1] > tmp.temperature[j]) {  //находим минимальную\
				                                 температуру в текущем месяце
				min_tmp[i - 1] = tmp.temperature[j];
				}
			}
		ave_tmp[i - 1] = sum_tmp[0] / row;   //средняя температура за текущий месяц
		sum_tmp[1] += ave_tmp[i - 1];        //сумма средних значений температуры\
		                                                         за каждый месяц
		if (max_tmp[12] < max_tmp[i - 1]) {  //находим максимальную температуру в году     
			max_tmp[12] = max_tmp[i - 1];
			}
			
		if (min_tmp[12] > min_tmp[i - 1]) {   //находим минимальную температуру в году
			min_tmp[12] = min_tmp[i - 1];
		}
		//выводим на экран рассчитанные значения для текущего месяца	
		printf("\nThe average temperature for %s %.1f deg", month[i], ave_tmp[i - 1]);
		printf("\nThe minimum temperature for %s = %d deg", month[i], min_tmp[i - 1]);
		printf("\nThe maximum temperature for %s = %d deg", month[i], max_tmp[i - 1]);
		printf("\n");
		}
		
		//выводим на экран рассчитанные значения за год
	if (month_number == 0) {
		ave_tmp[12] = sum_tmp[1] / count;
		printf("\nData for the %d year:", tmp.year[0]);
		printf("\nThe average temperature for the year %.1f deg", ave_tmp[12]);
		printf("\nThe minimum temperature for the year = %d deg", min_tmp[12]);
		printf("\nThe maximum temperature for the year = %d deg", max_tmp[12]);
		printf("\n");
		}
    
    //закрываем файлы
    fclose(f);
    fclose(f_error);
    
    return 0;
}
