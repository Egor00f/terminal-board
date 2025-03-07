#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ksys.h>
#include <kolibri_libini.h>

char debug_readc()
{
    bool empty;
    char byte;

    asm_inline(
        "int $0x40"
        : "=a"(byte), "=b"(empty)
        : "a"(63), "b"(2)
    );


    if (empty)
        return byte;
    else
        return 0;
}

void debug_reads(char* s, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        s[i] = debug_readc();
        if (!s[i])
            break;
    }
}

inline char GetColorCode(const char* name)
{
    char code = '\0';

    if (strcmp(name, "Black"))
    {
        code = '0';
    }
    else if (strcmp(name, "Red"))
    {
        code = '1';
    }
    else if (strcmp(name, "Green"))
    {
        code = '2';
    }
    else if (strcmp(name, "Brown"))
    {
        code = '3';
    }
    else if (strcmp(name, "Blue"))
    {
        code = '4';
    }
    else if (strcmp(name, "Purple"))
    {
        code = '5';
    }
    else if (strcmp(name, "Tortoise"))
    {
        code = '6';
    }
    else if (strcmp(name, "White"))
    {
        code = '7';
    }

    return code;
}

#define COLOR_NAME_LEN 16

const char* LogFile = "/tmp0/1/Board.log";
const char* SettingFile = "/tmp0/1/Board.ini";

const char DefaultNormalChars[COLOR_NAME_LEN] = "White";
const char DefaultNormalBackgorund[COLOR_NAME_LEN] = "Black";
const char DefaultKernelChars[COLOR_NAME_LEN] = "Purple";
const char DefaultKernelBackgorund[COLOR_NAME_LEN] = "Black";

char line[4096];    // current line
size_t i = 0;
FILE* f;

char* Kernel;
char* Normal;

void LoadSetting()
{
    printf("LoadSettings\n");

    char Chars[COLOR_NAME_LEN];
    char Background[COLOR_NAME_LEN];

    LIBINI_get_str(SettingFile, "Normal", "Chars", Chars, COLOR_NAME_LEN, DefaultNormalChars);
    LIBINI_get_str(SettingFile, "Normal", "Background", Background, COLOR_NAME_LEN, DefaultNormalBackgorund);

    sprintf(Normal, "\033[3%c;4%cm", GetColorCode(Chars), GetColorCode(Background));

    LIBINI_get_str(SettingFile, "Kernel", "Chars", Chars, COLOR_NAME_LEN, DefaultNormalChars);
    LIBINI_get_str(SettingFile, "Kernel", "Background", Background, COLOR_NAME_LEN, DefaultNormalBackgorund);

    sprintf(Kernel, "\033[3%c;4%cm", GetColorCode(Chars), GetColorCode(Background));
}

void print()
{
    if (memcmp(line, "K : ", 4))
    {
        fputs(Kernel, stdout);
    }

    line[i + 1] = '\0';
    printf(line);
    fprintf(f, line);

    fputs(Normal, stdout);
    memset(line, '\0', i);
    i = 0;
}

int main()
{
    if (kolibri_libini_init())
    {
        _ksys_debug_puts("Failed load libini\n");
        exit(1);
    }

    memset(line, 0, 4096);

    if (!access(LogFile, F_OK))
        _ksys_file_create(LogFile);

    if (!access(SettingFile, F_OK))
    {
        _ksys_file_create(SettingFile);

        printf("Create settings file\n");

        LIBINI_set_str(SettingFile, "Normal", "Chars", DefaultNormalChars, COLOR_NAME_LEN);
        LIBINI_set_str(SettingFile, "Normal", "Background", DefaultNormalBackgorund, COLOR_NAME_LEN);
        LIBINI_set_str(SettingFile, "Kernel", "Chars", DefaultKernelChars, COLOR_NAME_LEN);
        LIBINI_set_str(SettingFile, "Kernel", "Background", DefaultKernelBackgorund, COLOR_NAME_LEN);
    }

    LoadSetting();

    f = fopen(LogFile, "w");

    fputs(Normal, stdout);

    while (true)
    {
        char byte = debug_readc();
        if (byte)
        {
            line[i] = byte;
            i++;

            if (byte == '\n')
                print();
        }
        else
        {
            _ksys_thread_yield();
        }
    }

    fclose(f);

    return 0;
}
