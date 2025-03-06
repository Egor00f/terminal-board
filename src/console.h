#ifndef __CONSOLE_H__
#define __CONSOLE_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdarg.h>
#include <stdbool.h>

    typedef unsigned long dword; /* 32-битное беззнаковое целое */
    typedef unsigned short word; /* 16-битное беззнаковое целое */

    /*
     * Инициализация консоли. Вызывается один раз в начале программы.
     */
    extern void __stdcall con_init(dword wnd_width, dword wnd_height,
        dword scr_width, dword scr_height, const char* title) ;

    extern void __stdcall con_exit(bool bCloseWindow);

    /*
     * Очищает экран и переводит курсор в левый верхний угол.
     */
    extern void __stdcall con_cls();

    /*
     * Устанавливает новый заголовок окна консоли.
     */
    void __stdcall con_set_title(const char* newTitle);

    void __stdcall con_write_asciiz(const char* string);

    void __stdcall con_write_string(const char* string, int length);

    /*
     * Стандартная printf из ANSI C.
     */
    int __stdcall con_printf(const char* format, va_list args);

    dword __stdcall con_get_flags(void);

    dword __stdcall con_set_flags(dword new_flags);

    enum CON_COLOR
    {
        CON_COLOR_BLUE = 1,
        CON_COLOR_GREEN = 2,
        CON_COLOR_RED = 4,
        CON_COLOR_BRIGHT = 8
    };

    enum CON_BGR
    {
        CON_BGR_BLUE = 0x10,
        CON_BGR_GREEN = 0x20,
        CON_BGR_RED = 0x40,
        CON_BGR_BRIGHT = 0x80
    };

#define CON_IGNORE_SPECIALS 0x100

#define CON_WINDOW_CLOSED 0x200

    /*
     * Возвращает значение высоты шрифта.
     */
    int __stdcall con_get_font_height(void);

    /*
     * Получает значение высоты курсора.
     */
    int __stdcall con_get_cursor_height(void);

    /*
     * Устанавливает значение высоты курсора. Возвращает старое значение.
     * Попытка установить значение вне корректного интервала (от 0 до font_height-1)
     * игнорируется.
     * Курсор высоты 0 не отображается на экране.
     */
    int __stdcall con_set_cursor_height(int new_height);

    /*
     * Считывает один символ с клавиатуры.
     * Для обычных символов возвращается ASCII-код. Для расширенных символов
     * (например, Fx и стрелочек) первый вызов функции возвращает 0,
     * а повторный вызов возвращает расширенный код (подобно DOS-функциям ввода).
     * после закрытия окна консоли возвращается значение 0.
     */
    int __stdcall con_getch(void);

    /*
     * Считывает один символ с клавиатуры. Младший байт содержит ASCII-код клавиши
     * (0 для расширенных символов), старший - расширенный код
     * (подобно BIOS-функциям ввода).
     */
    word __stdcall con_getch2(void);

    /*
     * Возвращает 1, если какая-то клавиша была нажата, 0 иначе. Для считывания
     * нажатой клавиши предназначены функции con_getch и con_getch2.
     * После закрытия окна консоли всегда возвращает 1.
     */
    int __stdcall con_kbhit(void);

    /*
     * Считывает строку с клавиатуры. Ввод прерывается при поступлении символа
     * новой строки, а также по прочтении n-1 символа (в зависимости от того, что
     * произойдёт раньше). В первом случае символ новой строки также записывается в
     * str. Считанная строка дополняется нулевым символом.
     * Возвращает указатель на введённую
     * строку при успешном чтении и NULL, если окно консоли было закрыто.
     */
    char* __stdcall con_gets(char* str, int n);

    typedef int(__stdcall* con_gets2_callback)(int keycode, char** pstr, int* pn, int* ppos);
    /*
     * Полностью аналогична con_gets за исключением того, что когда пользователь
     * нажимает нераспознанную клавишу, вызывается указанная callback-процедура
     * (которая может, например, обрабатывать up/down для истории ввода и tab для
     * автодополнения). Процедуре передаётся код клавиши и три указателя - на строку,
     * на лимит и на текущую позицию в строке. Процедура может менять содержимое
     * строки и может менять саму строку (например, перераспределить память для
     * увеличения лимита), лимит, позицию в строке - для этого и передаются указатели.
     * Возвращаемое значение: 0=строка не менялась; 1=строка изменилась, необходимо
     * удалить старую и вывести новую; 2=строка изменилась, необходимо её вывести;
     * 3=немедленно выйти из функции.
     * Начиная с версии 6 библиотеки, функция возвращает указатель на введённую
     * строку при успешном чтении и NULL, если окно консоли было закрыто. До версии
     * 6 возвращаемое значение было неопределено.
    */
    char* __stdcall con_gets2(con_gets2_callback callback, char* str, int n);

    void __stdcall con_get_cursor_pos(int* px, int* py);

    /*
     * Устанавливает курсор в позицию с указанными координатами. Если какой-то из
     * параметров выходит за пределы соответствующего диапазона (от 0 до scr_width-1
     * для x, от 0 до scr_height-1 для y, scr_width и scr_height были заданы при
     * вызове con_init), то соответствующая координата курсора не меняется.
     */
    void __stdcall con_set_cursor_pos(int x, int y);

#ifdef __cplusplus
}
#endif

#endif // __CONSOLE_H__
