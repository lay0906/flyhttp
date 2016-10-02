#ifndef __FLY_STRING__
#define __FLY_STRING__



#define space(x) isspace((int)x)


/**
 * 判断src跟dest_s...dest_e是否相等
 * 
 * @param src: 源字符串
 * @param dest_s: 目标串起始位置
 * @param dest_e: 目标串终止位置
 *
 * @return: (0: 不相等, 1:相等)
 */
int fly_strsecmp(const char *src, const char *dest_s, const char *dest_ea);

/**
 * 将dest_s...dest_e拷贝到src，并在src后补0
 * 
 * @param src: 源字符串
 * @param dest_s: 目标串起始位置
 * @param dest_e: 目标串终止位置
 */
void fly_strcpy(char *src, const char *dest_s, const char *dest_e);


void fly_tolower(char *src);

#endif
