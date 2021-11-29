#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <locale.h>
#include <malloc.h>
#include <string.h>
//заголовог тега
struct id3header
{
	char identifier[3];
	char version[2];
	char flags;
	unsigned char size[4];
};
//заголовок фрейма
struct id3frame
{
	char id[4];
	unsigned char size[4];
	char flags[2];
};
//Преобразовать размер из набора байт в число
unsigned int decode_size(unsigned char *ph)
{
	unsigned int ha = ph[0];
	unsigned int hb = ph[1];
	unsigned int hc = ph[2];
	unsigned int hd = ph[3];
	hc <<= 7;
	hb <<= 14;
	ha <<= 21;
	return ha | hb | hc | hd;
}
//преобразовать размер в набор байт
void encode_size(unsigned int sz, unsigned char *ph)
{
	ph[3] = sz & 0x7f;
	ph[2] = (sz>>7) & 0x7f;
	ph[1] = (sz >> 14) & 0x7f;
	ph[9] = (sz >> 21) & 0x7f;
}
//преобразовать из utf-16 в cp1251
char tocp1251(wchar_t bom, wchar_t c)
{
	if (bom == 0xfffe)	// если Byte Order Mark перевернутый (не как у intel)
	{//перевернуть байты
		c = ((c & 0xff) << 8) | ((c & 0xff00) >> 8);
	}
	if (c < 128)	//если обычный ASCII-символ
	{
		return c;	//вернуть его
	}
	if (c >= 0x410 && c <= 0x44f)	//если русские буквы
	{
		c -= 0x410;
		c += 0xc0;
		return c;		//преобразовать к кодам cp1251
	}
	if (c == 0x401)	//ё и Ё стоят особнаяком, их тоже преобразовать
		return 'Ё';
	if (c == 0x451)
		return 'ё';
	return '?';		//все остальные символы заменить на знаки вопроса
}
//вывести фрейм на экран
void show_frame(struct id3frame *frame,char* framedata)
{
	unsigned int fsize= decode_size(frame->size);	//вычислить размер фрейма
	for (int i = 0; i < sizeof(frame->id); i++)	//вывести название фрейма
	{
		printf("%c", frame->id[i]);
	}
	printf(": ");
	int ts;
	if (framedata[0] == 1)	//если фрейм в кодировке UTF-16
	{
		ts = (fsize - 3) / 2;	//вычислить кол-во символов текста 
		wchar_t *txt = (wchar_t*)(framedata + 1);	//начало текста
		if (frame->id[0] == 'C'&&frame->id[1] == 'O'&&frame->id[2] == 'M'&&frame->id[3] == 'M')	//если это комментарий
		{
			ts -= 2;							//коррекция длины текста
			txt = (wchar_t*)(framedata + 5);	//и его начала (пропуск eng)
		}
		wchar_t bom = *txt++;		//взять Byte Order Mark

		for (int i = 0; i < ts; i++)	//вывести текст
		{
			printf("%c", tocp1251(bom, txt[i]));
		}
	}
	else//в ASCII
	{
		ts = fsize - 1;		//вычислить кол-во символов текста 
		char *txt = framedata + 1;	//начало текста
		if (frame->id[0] == 'C'&&frame->id[1] == 'O'&&frame->id[2] == 'M'&&frame->id[3] == 'M')	//если это комментарий
		{
			ts -= 4;	//коррекция длины текста
			txt = framedata + 5;	//и его начала (пропуск eng)
		}

		for (int i = 0; i < ts; i++)	//вывести текст
		{
			printf("%c", txt[i]);
		}
	}
	printf("\n");
}
//вывести определенное поле
void get(FILE *f, char *prop_name)
{
	struct id3header header;
	struct id3frame frame;
	fread(&header, sizeof(struct id3header), 1, f);		//прочитать заголовой тега
	int hsize = decode_size(header.size);				//размер тега
	unsigned int fsize;
	unsigned char* framedata;
	int fnd = 0;				//флаг что нужный фрейм найден
	while (ftell(f) < hsize)							//перебор фреймов
	{
		fread(&frame, sizeof(struct id3frame), 1, f);	//прочитать заголовок фрейма
		fsize = decode_size(frame.size);				//размер фрейма
		if (frame.id[0] == 0 && frame.id[1] == 0 && frame.id[2] == 0 && frame.id[3] == 0)	//если пустое место
			break;								//то закончить
		framedata = malloc(fsize);				//выделить память под фрейм
		fread(framedata, 1, fsize, f);			//прочитать данные фрейма
		if (memcmp(prop_name, frame.id, 4)==0)	//если он совпал с заданным
		{
			show_frame(&frame, framedata);		//вывести
			fnd = 1;							//нашли нужный
		}
		free(framedata);						//освободить память
	}
	if (!fnd)
	{
		printf("Property not found.\n");
	}
}
//https://ru.wikipedia.org/wiki/Windows-1251
//преобразование из cp1251 в utf-16
wchar_t cp1251towchar(unsigned char c)
{
	if (c < 128)		//ascii сиволы возвращать без изменения
		return c;
	if (c == 'ё')		//ё и Ё стоят особняком
		return 0x451;
	if (c == 'Ё')
		return 0x401;
	if (c >= 0xc0 && c <= 0xff)	//русские перекодировать
		return c - 0xc0 + 0x410;
	return '?';		//неизвестные заменить вопросами
}
//истановить поле
void set(FILE *f, char *prop_name, unsigned char *prop_value)
{
	struct id3header header;
	struct id3frame frame;
	fread(&header, sizeof(struct id3header), 1, f);	//прочитать заголовой тега
	int hsize = decode_size(header.size);			//размер тега
	unsigned char* headerdata = malloc(hsize+ sizeof(struct id3header));	//выделить память под пормируемые фреймы
	memset(headerdata, 0, hsize + sizeof(struct id3header));			//обнулить ее
	unsigned char* ptr = headerdata + sizeof(struct id3header);		//указатель на начало фреймов
	unsigned int fsize;
	unsigned char* framedata;
	while (ftell(f) < hsize)							//перебор фреймов
	{
		fread(&frame, sizeof(struct id3frame), 1, f);	//прочитать заголовок фрейма
		fsize = decode_size(frame.size);				//размер фрейма
		if (frame.id[0] == 0 && frame.id[1] == 0 && frame.id[2] == 0 && frame.id[3] == 0)	//если пустое место
			break;									//то закончить
		framedata = malloc(fsize);					//выделить память под фрейм
		fread(framedata, 1, fsize, f);				//прочитать данные фрейма
		if (memcmp(prop_name, frame.id, 4) != 0)	//если он не совпал с заданным
		{
			memcpy(ptr, &frame, sizeof(struct id3frame));	//сохранить весь фрейм в памяти
			ptr += sizeof(struct id3frame);
			memcpy(ptr, framedata, fsize);
			ptr += fsize;
		}
		free(framedata);		//освободить память
	}
	//старый фрейм с нужным id, если был удален
	//и в конец фреймов дописываем уже новый с нужным id
	int asciionly = 1;	//флаг что в тексте только ASCII
	for (int i = 0; prop_value[i]; i++)	//проверка что в тексте только ASCII
	{
		if (prop_value[i] >= 0x80)
		{
			asciionly = 0;
			break;
		}
	}
	int framesize;
	if (asciionly)	//если в тексте только ASCII
	{
		framesize = strlen(prop_value)+1;	//размер фрейма будет такой
	}
	else//если не только ASCII, будет кодироваться в utf-16
	{
		framesize = strlen(prop_value) * 2 + 3;	//и размер будет такой
	}
	if (strcmp(prop_name, "COMM") == 0)	//если это комментарить
	{
		framesize += 4;	//добавить место под eng
	}
	if ((ptr- headerdata) + framesize+ sizeof(struct id3frame) > hsize + sizeof(struct id3header))	//проверка чтоб фрейм поместился
	{
		printf("Not enough room to save property\n");
		return;
	}
	for (int i = 0; i < 4; i++)	//скопировать id тега
	{
		*ptr++ = prop_name[i];
	}
	encode_size(framesize, ptr);	//добавить размер тега
	ptr += 4;
	*ptr++ = 0;		//флаги
	*ptr++ = 0;
	if (asciionly)		//если только ascii
	{
		*ptr++ = 0;
		if (strcmp(prop_name, "COMM") == 0)	//если комментарий, пропихнуть eng
		{
			*ptr++ = 'e';
			*ptr++ = 'n';
			*ptr++ = 'g';
			*ptr++ = 0;
		}
		memcpy(ptr, prop_value, strlen(prop_value));	//просто скопировать текст
		ptr += strlen(prop_value);
	}
	else
	{
		*ptr++ = 1;
		if (strcmp(prop_name, "COMM") == 0)	//если комментарий, пропихнуть eng
		{
			*ptr++ = 'e';
			*ptr++ = 'n';
			*ptr++ = 'g';
			*ptr++ = 0;
		}

		*ptr++ = 0xff;	//записать BOM
		*ptr++ = 0xfe;
		for (int i = 0; prop_value[i]; i++)	//и сформировать текст
		{
			wchar_t *wptr = (wchar_t *)ptr;
			*wptr = cp1251towchar(prop_value[i]);
			ptr += 2;
		}

	}
	fseek(f, 0, SEEK_SET);	//вернуться в нчало файла
	memcpy(headerdata, &header, sizeof(struct id3header));	//взять заголовок тега
	fwrite(headerdata, 1, hsize + sizeof(struct id3header), f);	//перезаписать тег в файле
}
//вывод тегов
void show(FILE *f)
{
	struct id3header header;
	struct id3frame frame;
	fread(&header, sizeof(struct id3header), 1, f);	//прочитать заголовой тега
	int hsize = decode_size(header.size);			//размер тега
	unsigned int fsize;
	unsigned char* framedata;
	while (ftell(f) < hsize)							//перебор фреймов
	{
		fread(&frame, sizeof(struct id3frame), 1, f);	//прочитать заголовок фрейма
		fsize = decode_size(frame.size);				//размер фрейма
		if (frame.id[0] == 0 && frame.id[1] == 0 && frame.id[2] == 0 && frame.id[3] == 0)	//если пустое место
			break;						//то закончить
		framedata = malloc(fsize);		//выделить память под фрейм
		fread(framedata, 1, fsize, f);	//прочитать данные фрейма
		show_frame(&frame, framedata);	//вывести фрейм
		free(framedata);
	}
}
int main(int argc, char *argv[]) 
{
	setlocale(LC_ALL, "Russian");//русская локаль
	char filepath[128] = {0};
	int cmd=-1;	
	char prop_name[32] = { 0 };
	char prop_value[32] = { 0 };
	if (argc < 3)
	{
		printf("Wrong parameters.\n");
		return 0;
	}
	for (int i = 1; i < argc; i++)		//разбор командной строки
	{
		if (memcmp(argv[i], "--filepath=", 11) == 0)	//если найден путь
		{
			strcpy(filepath, &argv[i][11]);
		}
		if (strcmp(argv[i], "--show") == 0)	//если найдена команда
		{
			cmd = 0;						//задать команду
		}
		if (memcmp(argv[i], "--get=", 6) == 0)	//если найдена команда
		{
			strcpy(prop_name, &argv[i][6]);	//запомнить имя
			cmd = 1;						//задать команду
		}
		if (memcmp(argv[i], "--set=", 6) == 0)	//если найдена команда
		{
			strcpy(prop_name, &argv[i][6]);	//запомнить имя
			cmd = 2;						//задать команду
		}
		if (memcmp(argv[i], "--value=", 8) == 0)	//если найдено значение
		{
			strcpy(prop_value, &argv[i][8]);	//запомнить
		}
	}
	if (filepath[0] == 0)
	{
		printf("No input file.\n");
		return 0;
	}
	FILE *f = fopen(filepath, "r+b");
	if (f == NULL)
	{
		printf("Can't open file %s\n",filepath);
		return 0;
	}

	switch (cmd)
	{
	case 0:
		show(f);
		break;
	case 1:
		if (prop_name[0] == 0)
		{
			printf("No property.\n");
		}
		else
		{
			get(f, prop_name);
		}
		break;
	case 2:
		if (prop_name[0] == 0)
		{
			printf("No property.\n");
			break;
		}
		if (prop_value[0] == 0)
		{
			printf("No value.\n");
			break;
		}
		set(f, prop_name, prop_value);
		break;
	}
	fclose(f);
	return 0;
}