Программа предназначена для обработки данных датчика температуры, записанных построчно в файле формата .csv .
Программа работает из консоли.
Для сборки файлов и компиляции необходимо в консоли прописать команду "make".
Для запуска программы необходимо прописать команду "prog" и необходимый ключ:
-h - вывод данных о программе
-f (название файла.csv) - ключ открывает файл с таким именем для обработки или выводит сообщение об ошибке. Данный ключ и название файла обязательны для проведения расчета
-m (номер месяца) - для обработки только одного введенного месяца. Если данный ключ не введен, или значение после данного ключа не соответствует натуральному числу от 1 до 12 включительно, то расчет будет произведен для всего года