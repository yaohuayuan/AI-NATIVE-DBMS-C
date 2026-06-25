#include "aidb/string_utils.h"

#include "aidb/memory.h"

#include <stddef.h>
#include <string.h>
#include <stdint.h>

char *aidb_string_duplicate(const char *text){
    if(text == NULL){
        return NULL;
    }
    size_t  len = strlen(text);
    if(len == SIZE_MAX){
        return NULL;
    }
    char *result = aidb_malloc(len+1);
    if(result == NULL){
        return NULL;
    }
    memcpy(result, text, len + 1);
    return result;
}

char *aidb_string_duplicate_n(const char *text, size_t length){
    if(text == NULL){
        return NULL;
    }
    int size = strlen(text);
    if(size <= length){
        return aidb_string_duplicate(text);
    }
    int index = 0;
    char*results = aidb_malloc(sizeof(char)*(length+1));
    memcpy(results,text,length);
    results[length] = '\0';
    return results;
}

int aidb_string_equal(const char *left, const char *right){
    if(left == NULL || right == NULL){
        return NULL;
    }
    return strcmp(left,right) == 0;
}

int aidb_string_starts_with(const char *text, const char *prefix){
    if(text == NULL || prefix == NULL){
        return NULL;
    }
    int size = strlen(prefix);
    return strncmp(text,prefix,size) == 0;
}

int aidb_string_ends_with(const char *text, const char *suffix){
    if(text == NULL || suffix == NULL){
        return NULL;
    }
    int text_len = strlen(text),suffix_len = strlen(suffix);
    return memcmp(text + text_len - suffix_len, suffix, suffix_len) == 0;
}

void aidb_string_to_lower_ascii(char *text){
    if(text == NULL){
        return NULL;
    }
    for(int i = 0;i < strlen(text);i++){
        if(text[i]>='A' &&text[i]<='Z'){
            text[i] = text[i] + ('a'-'A');
        }
    }
}

void aidb_string_to_upper_ascii(char *text){
    if(text == NULL){
        return NULL;
    }
    for(int i = 0;i < strlen(text);i++){
        if(text[i]>='a' &&text[i]<='z'){
            text[i] = text[i] + ('A'-'a');
        }
    }
}

enum aidb_status aidb_string_concat(const char *left,const char *right,char **out_text){
    if(left == NULL || right == NULL){
        return AIDB_ERROR_INVALID_ARGUMENT;
    }
    int left_len = strlen(left), right_len = strlen(right);
    char *result = aidb_malloc(left_len+right_len+1);
    for(int i=0;i<left_len;i++){
        result[i] = left[i];
    }
    for(int i = 0;i<right_len;i++){
        result[i+left_len] = right[i];
    }
    result[left_len+right_len] = '\0';
    *out_text = result;
    return AIDB_OK;
}