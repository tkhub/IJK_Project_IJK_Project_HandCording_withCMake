/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2021     */
/*------------------------------------------------------------------------*/

#ifndef XPRINTF_DEF
#define XPRINTF_DEF
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define     XF_USE____      1
#define     XF_NOT_USE      0

#define     XF_USE_OUTPUT   XF_USE____  /* 1: Enable output functions */
#define     XF_CRLF         XF_NOT_USE  /* 1: Convert \n ==> \r\n in the output char */
#define     XF_USE_DUMP     XF_NOT_USE  /* 1: Enable put_dump function */
#define     XF_USE_LLI      XF_NOT_USE  /* 1: Enable long long integer in size prefix ll */
#define     XF_USE_FP       XF_USE____  /* 1: Enable support for floating point in type e and f */
#define     XF_DPC          '.'         /* Decimal separator for floating point */
#define     XF_USE_INPUT    XF_NOT_USE  /* 1: Enable input functions */
#define     XF_INPUT_ECHO   XF_NOT_USE  /* 1: Echo back input chars in xgets function */

#if defined(__GNUC__) && __GNUC__ >= 10
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

#if XF_USE_OUTPUT

/**
 * @brief Set the output device function.
 * @param func Pointer to the output device function ( void output_func(int chr) ).
 */
#define xdev_out(func) xfunc_output = (void(*)(int))(func)

/**
 * @brief Pointer to the output device function. Macro : xedev_out is used to set this pointer.
 */
extern void (*xfunc_output)(int);

/**
 * @brief Output single character to the default device which is set by xdev_out function.
 * @param char chr output character 
 */
void xputc (int chr);

/**
 * @brief Output single character to output device function.
 * @param [in,out] func Pointer of output device function ( void output_func(int chr) ).
 * @param chr output character
 */
void xfputc (void (*func)(int), int chr);

/**
 * @brief Output string to the default device which is set by xdev_out function.
 * @param [in] char*  Pointer to the string
 */
void xputs (const char* str);

/**
 * @brief Output string to output device function.
 * @param [in,out] func Pointer of output device function ( void output_func(int chr) ).
 * @param [in] str Pointer of output string
 */
void xfputs (void (*func)(int), const char* str);

/**
 * @brief Output formatted string to the default device which is set by xdev_out function.
 * @param [in] fmt Pointer to the format string
 */
void xprintf (const char* fmt, ...);

/**
 * @brief Output formatted string to a buffer.
 * @param [out] buff Pointer to the output buffer
 * @param [in] fmt Pointer to the format string
 */
void xsprintf (char* buff, const char* fmt, ...);

/**
 * @brief Output formatted string to a buffer with length limit.
 * @param [out] buff Pointer to the output buffer
 * @param [in] len Length of the output buffer
 * @param [in] fmt Pointer to the format string
 */
void xsnprintf (char* buff, unsigned int len, const char* fmt, ...);

/**
 * @brief Output formatted string to output device function.
 * @param [in,out] func Pointer of output device function ( void output_func(int chr) ).
 * @param [in] fmt Pointer to the format string
 */
void xfprintf (void (*func)(int), const char* fmt, ...);

/**
 * @brief Output memory dump to the default device which is set by xdev_out function.
 * @param [in] buff Pointer to the memory buffer
 * @param [in] addr Address of the memory buffer
 * @param [in] len Length of the memory buffer
 * @param [in] width Width of the dump
 */
void put_dump (const void* buff, unsigned long addr, int len, size_t width);

#endif /* XF_USE_OUTPUT */

#if XF_USE_INPUT
/**
 * @brief Set the input device function.
 * @param func Pointer to the input device function ( int input_func(void) ).
 */
#define xdev_in(func) xfunc_input = (int(*)(void))(func)

/**
 * @brief Pointer to the input device function. Macro : xedev_in is used to set this pointer.
 */
extern int (*xfunc_input)(void);

/**
 * @brief Get a line from the default input device which is set by xdev_in functionl.
 * @param [out] buff Pointer to the buffer to store the input line.
 * @param len Length of the buffer.
 * @return 0: End of streem, 1: A line arrived. 
 * @details '\r' or '\n' is EOL. If "XF_INPUT_ECHO" is "XF_USE____", echo back to output device.
 */
int xgets (char* buff, int len);

/**
 * @brief Convert a string to a long integer.
 * @param [in, out] str Pointer to the pointer of the string to convert.
 * @param [out] res Pointer to the variable to store the converted long integer.
 * @return 0: Failed, 1: Success.
 */
int xatoi (char** str, long* res);

/**
 * @brief Convert a string to a double.
 * @param [in, out] str Pointer to the pointer of the string to convert.
 * @param [out] res Pointer to the variable to store the converted double.
 * @return 0: Failed, 1: Success.
 */
int xatof (char** str, double* res);

#endif /* XF_USE_INPUT */

#ifdef __cplusplus
}
#endif

#endif
