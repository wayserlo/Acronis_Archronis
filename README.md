# Архиватор

## Как работает

Архиватор использует статический алгоритм Хаффмана для сжатия данных.

### Кодировщик:

Программа парсит данные и объединяет файлы в один массив в формате ```[files number] [file_1 content size] [file_1 content] ... [file_n content size] [file_n content]```. После этого этот массив кодируется алгоритмом Хаффмана:

- Собирается статистика по частоте встречаемости символов в массиве
- По статистике строится оптимальное кодирующее дерево
- По дереву кодируется массив

После этого закодированный массив сохраняется в файл

### Декодировщик

Программа парсит данные из файлика. После этого эти данные декодируются при помощи кодирующего дерева зашитого в этот файлик. Дальше массив декодированных данных парсится по формату кодировщика и каждый файл сохраняется отдельно.

---

## Пример использования

### Сборка

```bash
mkdir build
cd build/
cmake .. && make
```

После этого в директории ```/bin``` появится исполняемый файл ```Archiver```

### Запуск

Чтобы запустить кодировщик нужно запустить ```Archiver``` так:

```bash
./Archiver -e <file_1> <file_2> ... <file_n>
```

После этого появится файлик ```archive.myzip``` - архив с данными

Чтобы запустить декодировщик нужно запустить ```Archiver``` так:

```bash
./Archiver -d <path to archive>
```

После этого в этой директории появится n закодированных ранее файлов

