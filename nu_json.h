#ifndef H_NUJ_H

#ifndef NUJ_NO_STDIO
#include <stdio.h>
#endif

#ifndef NUJ_NO_STRING
#include <string.h>
#endif

// TODO: We should #ifdef this.
#define NUJDEF static

typedef struct NUJHandle* NUJHandle;
typedef struct NUJElement NUJElement;

NUJDEF NUJHandle          nuj_init(void* memory, unsigned long long size);
NUJDEF unsigned long long nuj_get_used_size(const NUJHandle handle);
NUJDEF NUJElement*        nuj_create_element(NUJHandle handle, unsigned int type, unsigned int size);
NUJDEF NUJElement*        nuj_create_element_object(NUJHandle handle, unsigned int element_count);
NUJDEF NUJElement*        nuj_create_element_array(NUJHandle handle, unsigned int element_count);
NUJDEF NUJElement*        nuj_create_element_string(NUJHandle handle, const char* string);
NUJDEF NUJElement*        nuj_create_element_integer(NUJHandle handle, long long value);
NUJDEF NUJElement*        nuj_create_element_double(NUJHandle handle, double value);
NUJDEF NUJElement*        nuj_add_element_element(NUJElement* element, const char* name, NUJElement* child);
NUJDEF int                nuj_is_last_object_element(const NUJElement* element);
NUJDEF void               nuj_printf(const NUJElement* element);
NUJDEF void               nuj_print(const NUJElement* element);
NUJDEF NUJElement*        nuj_find_element_by_name(const NUJElement* element, const char* name);
NUJDEF NUJElement*        nuj_parse(NUJHandle handle, const unsigned char* buffer, unsigned long long buffer_size);

#ifdef NU_JSON_IMPLEMENTATION

#define NUJ_ASSERT(x) do { if (!(x)) { *(volatile int*)0; } } while (0)

#define NUJ_INTEGER(x)    ((NUJInteger*)(x))
#define NUJ_DOUBLE(x)     ((NUJDouble*)(x))
#define NUJ_STRING(x)     ((NUJString*)(x))
#define NUJ_OBJECT(x)     ((NUJObject*)(x))
#define NUJ_ARRAY(x)      ((NUJArray*)(x))

#define NUJ_CINTEGER(x)   ((const NUJInteger*)(x))
#define NUJ_CDOUBLE(x)    ((const NUJDouble*)(x))
#define NUJ_CSTRING(x)    ((const NUJString*)(x))
#define NUJ_COBJECT(x)    ((const NUJObject*)(x))
#define NUJ_CARRAY(x)     ((const NUJArray*)(x))

#define NUJ_CREATE_ELEMENT(handle, element)      (element*)(nuj_create_element(handle, element##_##TYPE, sizeof(element)))
#define NUJ_GET_ELEMENT_VALUE(element, t)        (*((t*)(&NUJ_INTEGER(element)->value)))
#define NUJ_GET_ELEMENT_CHILD(element, i)        (NUJ_OBJECT(element)->children[(i)])
#define NUJ_GET_ELEMENT_CHILD_COUNT(element)     (NUJ_OBJECT(element)->child_count)

typedef struct NUJParser  NUJParser;
typedef struct NUJToken   NUJToken;

static void*              nuj__push_size(NUJHandle handle, unsigned int size);
static unsigned int       nuj__get_element_size(const NUJElement* element);
static unsigned long long nuj__get_total_element_object_size(const NUJElement* element);
static void               nuj__print_newline_and_spaces(unsigned int space_count);
static void               nuj__print_primitive_element(const NUJElement* element);
static void               nuj__printf(const NUJElement* element, unsigned int depth);

static void               nuj__parse_error(NUJParser* parser, NUJToken token, char expected);
static inline int         nuj__parse_is_whitespace_char(char character);
static inline void        nuj__parse_skip_all_whitespace_chars(NUJParser* parser);
static inline int         nuj__parse_is_numeric(char character);
static inline void        nuj__parse_skip_all_numerics(NUJParser* parser);
static NUJToken           nuj__parse_get_token(NUJParser* parser);
static inline int         nuj__parse_match_token(NUJToken token, unsigned int match_token_type);
static double             nuj__parse_token_to_double(NUJToken token);
static NUJElement*        nuj__parse_token_to_element(NUJHandle handle, NUJToken token);
static NUJElement*        nuj__parse_element_pair_value(NUJHandle handle, NUJParser* parser);
static inline void        nuj__parse_set_element_pair_name(NUJHandle handle, NUJElement* element, NUJToken token);
static NUJElement*        nuj__parse_element_pair(NUJHandle handle, NUJParser* parser, int in_object);
static inline void        nuj__parse_add_element_element(NUJElement* element, NUJElement* child);
static NUJElement*        nuj__parse_create_element_object_or_array(NUJElement* element, NUJElement* first_element);
static unsigned int       nuj__parse_calculate_element_child_count(NUJParser* parser, unsigned int type);
static NUJElement*        nuj__parse_element_array(NUJHandle handle, NUJParser* parser);
static NUJElement*        nuj__parse_element_object(NUJHandle handle, NUJParser* parser);

typedef struct NUJHandle
{
    unsigned char* buffer;
    unsigned long long buffer_used;
    unsigned long long buffer_size;
} NUJHandleInternal;

enum NUJElementType
{
    NUJNull_TYPE,

    NUJString_TYPE,
    NUJInteger_TYPE,
    NUJDouble_TYPE,
    NUJArray_TYPE,
    NUJObject_TYPE,

    NUJEnd_TYPE,
};

struct NUJElement
{
    unsigned long long type;
    const char* name;
    struct NUJElement* parent;
};

typedef struct NUJObject
{
    struct NUJElement element;
    struct NUJElement** children;
    unsigned int child_count;
    unsigned int max_child_count;
} NUJObject, NUJArray;

typedef struct NUJInteger
{
    struct NUJElement element;
    long long value;
} NUJInteger;

typedef struct NUJDouble
{
    struct NUJElement element;
    double value;
} NUJDouble;

typedef struct NUJString
{
    struct NUJElement element;
    const char* value;
} NUJString;

typedef struct NUJParser
{
    const unsigned char* initial;
    const unsigned char* current;
} NUJParser;

typedef enum NUJTokenType
{
    NUJ_UNKNOWN_TYPE,

    NUJ_OBRACE_TYPE,
    NUJ_OBRACKET_TYPE,

    NUJ_CBRACE_TYPE,
    NUJ_CBRACKET_TYPE,

    NUJ_COLON_TYPE,
    NUJ_COMMA_TYPE,

    NUJ_STRING_TYPE,
    NUJ_NUMBER_TYPE,

    NUJ_EOF_TYPE,
} NUJTokenType;

typedef struct NUJToken
{
    const unsigned char* start;
    unsigned int length;
    unsigned int type;
} NUJToken;

static void* nuj__push_size(NUJHandle handle, unsigned int size)
{
    void* result = 0;

    NUJ_ASSERT(handle->buffer_used + size < handle->buffer_size);
    result = handle->buffer + handle->buffer_used;
    handle->buffer_used += size;

    return result;
}

static unsigned int nuj__get_element_size(const NUJElement* element)
{
    unsigned int size = 0;

    switch (element->type)
    {
        case NUJString_TYPE:
        {
            size = sizeof(NUJString) + (unsigned int)strlen(NUJ_CSTRING(element)->value) + 1;
        }
        break;
        case NUJInteger_TYPE:
        {
            size = sizeof(NUJInteger);
        }
        break;
        case NUJDouble_TYPE:
        {
            size = sizeof(NUJDouble);
        }
        break;
        case NUJArray_TYPE:
        {
            size = sizeof(NUJArray);
        }
        break;
        case NUJObject_TYPE:
        {
            size = sizeof(NUJObject);
        }
        break;
    }

    if (element->name)
    {
        // NOTE: Skip len bytes + null terminator
        size += (unsigned int)strlen(element->name) + 1;
    }

    return size;
}

static unsigned long long nuj__get_total_element_object_size(const NUJElement* element)
{
    unsigned int i = 0;
    unsigned long long size = 0;

    NUJ_ASSERT(element->type == NUJObject_TYPE || element->type == NUJArray_TYPE);

    size += nuj__get_element_size(element);
    size += (sizeof(NUJElement*) * NUJ_COBJECT(element)->child_count);

    for (i = 0; i < NUJ_COBJECT(element)->child_count; ++i)
    {
        const NUJElement* e = NUJ_COBJECT(element)->children[i];

        if (e->type == NUJObject_TYPE || e->type == NUJArray_TYPE)
        {
            size += nuj__get_total_element_object_size(e);
        }
        else
        {
            size += nuj__get_element_size(e);
        }
    }

    return size;
}

static void nuj__print_newline_and_spaces(unsigned int space_count)
{
    unsigned int i = 0;

    printf("\n");

    for (i = 0; i < space_count; ++i)
    {
        printf(" ");
    }
}

static void nuj__print_primitive_element(const NUJElement* element)
{
    switch (element->type)
    {
        case NUJInteger_TYPE:
        {
            printf("%lld", NUJ_CINTEGER(element)->value);
        }
        break;
        case NUJDouble_TYPE:
        {
            // TODO: Parametrize .16f?
            printf("%.16lf", NUJ_CDOUBLE(element)->value);
        }
        break;
        case NUJString_TYPE:
        {
            printf("\"%s\"", NUJ_CSTRING(element)->value);
        }
        break;
    }

    if (!nuj_is_last_object_element(element))
    {
        printf(",");
    }
}

static void nuj__printf(const NUJElement* element, unsigned int depth)
{
    if (!element)
        return;

    if (element->parent && (element->parent->type == NUJObject_TYPE || element->parent->type == NUJArray_TYPE))
    {
        nuj__print_newline_and_spaces(depth * 3);
    }

    if (element->name)
    {
        printf("\"%s\": ", element->name);
    }

    if (element->type == NUJObject_TYPE)
    {
        printf("{");
    }
    else if (element->type == NUJArray_TYPE)
    {
        printf("[");
    }
    else
    {
        nuj__print_primitive_element(element);
    }

    if (element->type == NUJObject_TYPE || element->type == NUJArray_TYPE)
    {
        unsigned int i = 0;

        for (i = 0; i < NUJ_COBJECT(element)->child_count; ++i)
        {
            nuj__printf(NUJ_COBJECT(element)->children[i], depth + 1);
        }

        if (!element->parent || (element->parent->type == NUJObject_TYPE || element->parent->type == NUJArray_TYPE))
        {
            nuj__print_newline_and_spaces(depth * 3);
        }
        printf("%s", element->type == NUJObject_TYPE ? "}" : element->type == NUJArray_TYPE ? "]" : 0);

        if (!nuj_is_last_object_element(element))
        {
            printf(",");
        }
    }
}

static void nuj__parse_error(NUJParser* parser, NUJToken token, char expected)
{
    const unsigned char* initial = parser->initial;
    unsigned int line_count = 1;
    unsigned int char_count = 0;

    while (initial != token.start)
    {
        ++char_count;

        if (*initial++ == '\n')
        {
            ++line_count;
            char_count = 0;
        }
    }

    // NOTE: Subtract (") quote?
    if (token.type == NUJ_STRING_TYPE)
    {
        --char_count;
    }

    fprintf(stderr, "Syntax error: '%c' expected but found (%.*s) at line (%u, %u)!\n", expected, token.length, token.start, line_count, char_count);
    NUJ_ASSERT(0);
}

static inline int nuj__parse_is_whitespace_char(char character)
{
    int result = ((character == ' ')  ||
                  (character == '\t') ||
                  (character == '\v') ||
                  (character == '\f') ||
                  (character == '\n') ||
                  (character == '\r'));

    return result;
}

static inline void nuj__parse_skip_all_whitespace_chars(NUJParser* parser)
{
    while (nuj__parse_is_whitespace_char(*parser->current))
    {
        ++parser->current;
    }
}

static inline int nuj__parse_is_numeric(char character)
{
    int result = ((character >= '0' && character <= '9') ||
                  (character == '.'));

    return result;
}

static inline void nuj__parse_skip_all_numerics(NUJParser* parser)
{
    while (nuj__parse_is_numeric(*parser->current))
    {
        ++parser->current;
    }
}

static NUJToken nuj__parse_get_token(NUJParser* parser)
{
    NUJToken token = { 0 };
    unsigned char current = 0;

    nuj__parse_skip_all_whitespace_chars(parser);

    token.start = parser->current;
    token.length = 1;

    current = *parser->current;
    ++parser->current;

    switch (current)
    {
        case '[':  { token.type = NUJ_OBRACKET_TYPE; } break;
        case '{':  { token.type = NUJ_OBRACE_TYPE;   } break;

        case ']':  { token.type = NUJ_CBRACKET_TYPE; } break;
        case '}':  { token.type = NUJ_CBRACE_TYPE;   } break;

        case ':':  { token.type = NUJ_COLON_TYPE;    } break;
        case ',':  { token.type = NUJ_COMMA_TYPE;    } break;

        case '\0': { token.type = NUJ_EOF_TYPE;      } break;

        case '"':
        {
            token.type = NUJ_STRING_TYPE;
            token.start = parser->current;

            while (parser->current && *parser->current != '\0' && *parser->current != '"')
            {
                ++parser->current;
            }

            token.length = (unsigned int)(parser->current - token.start);

            if (*parser->current == '"')
            {
                ++parser->current;
            }
            else
            {
                token.type = NUJ_UNKNOWN_TYPE;
            }
        }
        break;

        default:
        {
            token.type = NUJ_UNKNOWN_TYPE;

            --parser->current;

            if (*parser->current == '-' || *parser->current == '+')
            {
                token.start = parser->current++;
                token.type = NUJ_NUMBER_TYPE;
                nuj__parse_skip_all_numerics(parser);
                token.length = (unsigned int)(parser->current - token.start);
            }
            else if (nuj__parse_is_numeric(*parser->current))
            {
                token.start = parser->current;
                token.type = NUJ_NUMBER_TYPE;
                nuj__parse_skip_all_numerics(parser);
                token.length = (unsigned int)(parser->current - token.start);
            }
        }
        break;
    }

    return token;
}

static inline int nuj__parse_match_token(NUJToken token, unsigned int match_token_type)
{
    int result = 1;

    if (token.type != match_token_type)
    {
        result = 0;
    }

    return result;
}

// NOTE: This is far from being perfect string to double converter but
// we don't need that much accuracy.  At least, it is much faster than
// copying and converting.
static double nuj__parse_token_to_double(NUJToken token)
{
    const unsigned char* current = token.start;
    double number = 0;
    unsigned char digit = 0;
    int negative = 1;
    double coef = 1.0 / 10.0;

    if (*current == '-' || *current == '+')
    {
        if (*current == '-')
        {
            negative = -1;
        }

        ++current;
    }

    while ((digit = *current++ - '0') < 10)
    {
        number = 10.0 * number + digit;
    }

    NUJ_ASSERT(*(current - 1) == '.');

    while ((digit = *current++ - '0') < 10)
    {
        number = number + coef * digit;
        coef *= 1.0 / 10.0;
    }

    number = number * negative;

    return number;
}

static NUJElement* nuj__parse_token_to_element(NUJHandle handle, NUJToken token)
{
    NUJElement* element = 0;

    switch (token.type)
    {
        case NUJ_NUMBER_TYPE:
        {
            double value = 0;

            value = nuj__parse_token_to_double(token);
            element = nuj_create_element_double(handle, value);
        }
        break;
        case NUJ_STRING_TYPE:
        {
            char* svalue = 0;

            element = nuj_create_element_string(handle, 0);

            svalue = nuj__push_size(handle, token.length + 1);
            memcpy((char*)svalue, token.start, token.length);
            svalue[token.length] = '\0';
            NUJ_STRING(element)->value = svalue;
        }
        break;
    }

    return element;
}

static NUJElement* nuj__parse_element_pair_value(NUJHandle handle, NUJParser* parser)
{
    NUJElement* element = 0;
    NUJToken token = nuj__parse_get_token(parser);

    switch (token.type)
    {
        case NUJ_NUMBER_TYPE:
        {
            element = nuj__parse_token_to_element(handle, token);
        }
        break;
        case NUJ_STRING_TYPE:
        {
            element = nuj__parse_token_to_element(handle, token);
        }
        break;
        case NUJ_OBRACKET_TYPE:
        {
            element = nuj__parse_element_array(handle, parser);
        }
        break;
        case NUJ_OBRACE_TYPE:
        {
            element = nuj__parse_element_object(handle, parser);
        }
        break;
    }

    return element;
}

static inline void nuj__parse_set_element_pair_name(NUJHandle handle, NUJElement* element, NUJToken token)
{
    char* name = 0;

    name = nuj__push_size(handle, token.length + 1);
    memcpy(name, token.start, token.length);
    name[token.length] = '\0';
    element->name = name;
}

static NUJElement* nuj__parse_element_pair(NUJHandle handle, NUJParser* parser, int in_object)
{
    NUJElement* element = 0;

    if (in_object)
    {
        NUJToken string_token = nuj__parse_get_token(parser);
        NUJToken token = { 0 };

        if (nuj__parse_match_token(string_token, NUJ_STRING_TYPE))
        {
            token = nuj__parse_get_token(parser);
        }
        else
        {
            nuj__parse_error(parser, string_token, '\"');
        }

        if (nuj__parse_match_token(token, NUJ_COLON_TYPE))
        {
            element = nuj__parse_element_pair_value(handle, parser);
            nuj__parse_set_element_pair_name(handle, element, string_token);
        }
        else
        {
            nuj__parse_error(parser, token, ':');
        }
    }
    else
    {
        element = nuj__parse_element_pair_value(handle, parser);
    }

    return element;
}

static inline void nuj__parse_add_element_element(NUJElement* element, NUJElement* child)
{
    NUJObject* nuj_object = NUJ_OBJECT(element);

    NUJ_ASSERT(nuj_object->child_count < nuj_object->max_child_count);

    nuj_object->children[nuj_object->child_count++] = child;
    child->parent = element;
}

static NUJElement* nuj__parse_create_element_object_or_array(NUJElement* element, NUJElement* first_element)
{
    NUJElement* el = 0;
    unsigned long long element_offset = 0;
    unsigned int i = 0;

    NUJ_ASSERT(element->type == NUJObject_TYPE || element->type == NUJArray_TYPE);

    for (i = 0; i < NUJ_OBJECT(element)->max_child_count; ++i)
    {
        el = (NUJElement*)((unsigned char*)first_element + element_offset);

        nuj__parse_add_element_element(element, el);

        switch (el->type)
        {
            case NUJObject_TYPE:
            case NUJArray_TYPE:
            {
                element_offset += nuj__get_total_element_object_size(el);
            }
            break;
            case NUJDouble_TYPE:
            case NUJString_TYPE:
            {
                element_offset += nuj__get_element_size(el);
            }
            break;
            default:
            {
                NUJ_ASSERT(!"Unexpected type!");
            }
            break;
        }
    }

    return element;
}

static unsigned int nuj__parse_calculate_element_child_count(NUJParser* parser, unsigned int type)
{
    unsigned int level = 0;
    NUJParser current_parser = *parser;
    NUJToken token = { 0 };
    NUJTokenType last_token_type = type == NUJObject_TYPE ? NUJ_OBRACE_TYPE : NUJ_OBRACKET_TYPE;
    unsigned int element_count = 0;
    int done = 0;

    // TODO: Maybe it is not good idea to do this with tokens?
    // We could just operate on raw strings but does it matter?

    NUJ_ASSERT(type == NUJObject_TYPE || type == NUJArray_TYPE);

    while (!done)
    {
        token = nuj__parse_get_token(&current_parser);

        switch (token.type)
        {
            // NOTE: Error?
            case NUJ_EOF_TYPE:
            case NUJ_UNKNOWN_TYPE:
            {
                done = 1;
                element_count = 0;
            }
            break;
            case NUJ_OBRACE_TYPE:
            case NUJ_OBRACKET_TYPE:
            {
                ++level;
            }
            break;
            case NUJ_COMMA_TYPE:
            {
                if (level == 0)
                {
                    ++element_count;
                }
            }
            break;
            case NUJ_CBRACE_TYPE:
            case NUJ_CBRACKET_TYPE:
            {
                if (level == 0)
                {
                    if (last_token_type == NUJ_OBRACE_TYPE || last_token_type == NUJ_OBRACKET_TYPE)
                    {
                        element_count = 0;
                    }
                    else
                    {
                        ++element_count;
                    }

                    done = 1;
                }
                else
                {
                    --level;
                }
            }
            break;
        }

        last_token_type = token.type;
    }

    return element_count;
}

static NUJElement* nuj__parse_element_array(NUJHandle handle, NUJParser* parser)
{
    NUJToken token = { 0 };
    int done = 0;
    NUJElement* array_element = 0;
    NUJElement* first_element = 0;
    unsigned int element_count = nuj__parse_calculate_element_child_count(parser, NUJArray_TYPE);

    array_element = nuj_create_element_array(handle, element_count);

    if (element_count)
    {
        first_element = nuj__parse_element_pair(handle, parser, 0);
    }

    while (!done)
    {
        token = nuj__parse_get_token(parser);

        switch (token.type)
        {
            case NUJ_COMMA_TYPE:
            {
                nuj__parse_element_pair(handle, parser, 0);
            }
            break;
            case NUJ_CBRACKET_TYPE:
            {
                done = 1;
            }
            break;
            default:
            {
                nuj__parse_error(parser, token, ',');
            }
            break;
        }
    }

    if (element_count)
    {
        array_element = nuj__parse_create_element_object_or_array(array_element, first_element);
    }

    return array_element;
}

static NUJElement* nuj__parse_element_object(NUJHandle handle, NUJParser* parser)
{
    NUJToken token = { 0 };
    int done = 0;
    NUJElement* object_element = 0;
    NUJElement* first_element = 0;
    unsigned int element_count = nuj__parse_calculate_element_child_count(parser, NUJObject_TYPE);

    object_element = nuj_create_element_object(handle, element_count);

    // NOTE: If it is object with 0 child skip this and find '}' token.
    if (element_count)
    {
        first_element = nuj__parse_element_pair(handle, parser, 1);
    }

    while (!done)
    {
        token = nuj__parse_get_token(parser);

        switch (token.type)
        {
            case NUJ_COMMA_TYPE:
            {
                nuj__parse_element_pair(handle, parser, 1);
            }
            break;
            case NUJ_CBRACE_TYPE:
            {
                done = 1;
            }
            break;
            default:
            {
                nuj__parse_error(parser, token, ',');
            }
            break;
        }
    }

    if (element_count)
    {
        object_element = nuj__parse_create_element_object_or_array(object_element, first_element);
    }

    return object_element;
}

// NOTE: Pre-allocated buffer by size.
NUJDEF NUJHandle nuj_init(void* memory, unsigned long long size)
{
    NUJHandle nuj_handle = (NUJHandle)memory;

    nuj_handle->buffer = (unsigned char*)memory + sizeof(NUJHandleInternal);
    nuj_handle->buffer_used = sizeof(NUJHandleInternal);
    nuj_handle->buffer_size = size - sizeof(NUJHandleInternal);

    return nuj_handle;
}

NUJDEF unsigned long long nuj_get_used_size(const NUJHandle handle)
{
    return handle->buffer_used;
}

NUJDEF NUJElement* nuj_create_element(NUJHandle handle, unsigned int type, unsigned int size)
{
    NUJElement* element = nuj__push_size(handle, size);

    element->type = (unsigned long long)type;
    element->parent = 0;

    return element;
}

NUJDEF NUJElement* nuj_create_element_object(NUJHandle handle, unsigned int element_count)
{
    NUJObject* nuj_object = NUJ_CREATE_ELEMENT(handle, NUJObject);

    nuj_object->child_count = 0;
    nuj_object->max_child_count = element_count;
    nuj_object->children = nuj__push_size(handle, element_count * sizeof(NUJElement*));

    return &nuj_object->element;
}

NUJDEF NUJElement* nuj_create_element_array(NUJHandle handle, unsigned int element_count)
{
    NUJArray* nuj_array = NUJ_CREATE_ELEMENT(handle, NUJArray);

    nuj_array->child_count = 0;
    nuj_array->max_child_count = element_count;
    nuj_array->children = nuj__push_size(handle, element_count * sizeof(NUJElement*));

    return &nuj_array->element;
}

NUJDEF NUJElement* nuj_create_element_string(NUJHandle handle, const char* string)
{
    NUJString* nuj_string = NUJ_CREATE_ELEMENT(handle, NUJString);

    nuj_string->value = string;

    return &nuj_string->element;
}

NUJDEF NUJElement* nuj_create_element_integer(NUJHandle handle, long long value)
{
    NUJInteger* nuj_integer = NUJ_CREATE_ELEMENT(handle, NUJInteger);

    nuj_integer->value = value;

    return &nuj_integer->element;
}

NUJDEF NUJElement* nuj_create_element_double(NUJHandle handle, double value)
{
    NUJDouble* nuj_double = NUJ_CREATE_ELEMENT(handle, NUJDouble);

    nuj_double->value = value;

    return &nuj_double->element;
}

NUJDEF NUJElement* nuj_add_element_element(NUJElement* element, const char* name, NUJElement* child)
{
    NUJObject* nuj_object = 0;

    NUJ_ASSERT(element->type == NUJObject_TYPE || element->type == NUJArray_TYPE);

    nuj_object = NUJ_OBJECT(element);

    NUJ_ASSERT(nuj_object->child_count < nuj_object->max_child_count);

    child->name = name;
    nuj_object->children[nuj_object->child_count++] = child;

    if (!child->parent)
    {
        child->parent = element;
    }

    return child;
}

NUJDEF int nuj_is_last_object_element(const NUJElement* element)
{
    NUJElement* parent = element->parent;
    NUJObject* parent_object = NUJ_OBJECT(parent);
    int result = (!parent ||
                  ((parent->type == NUJObject_TYPE || parent->type == NUJArray_TYPE) &&
                   parent_object->children[parent_object->child_count - 1] == element));

    return result;
}

NUJDEF void nuj_printf(const NUJElement* element)
{
    nuj__printf(element, 0);
}

NUJDEF void nuj_print(const NUJElement* element)
{
    if (!element)
        return;

    if (element->name)
    {
        printf("\"%s\":", element->name);
    }

    if (element->type == NUJObject_TYPE)
    {
        printf("{");
    }
    else if (element->type == NUJArray_TYPE)
    {
        printf("[");
    }
    else
    {
        nuj__print_primitive_element(element);
    }

    if (element->type == NUJObject_TYPE || element->type == NUJArray_TYPE)
    {
        unsigned int i = 0;

        for (i = 0; i < NUJ_COBJECT(element)->child_count; ++i)
        {
            nuj_print(NUJ_COBJECT(element)->children[i]);
        }

        printf("%s", element->type == NUJObject_TYPE ? "}" : element->type == NUJArray_TYPE ? "]" : 0);

        if (!nuj_is_last_object_element(element))
        {
            printf(",");
        }
    }
}

// TODO: Probably, it is not good idea to iterate all elements until we find it.
// Hash or something similar would be better but how we should do it?
NUJDEF NUJElement* nuj_find_element_by_name(const NUJElement* element, const char* name)
{
    NUJElement* found = 0;
    NUJElement* el = 0;
    unsigned int i = 0;

    NUJ_ASSERT(element->type == NUJObject_TYPE || element->type == NUJArray_TYPE);

    for (i = 0; !found && i < NUJ_COBJECT(element)->child_count; ++i)
    {
        el = NUJ_OBJECT(element)->children[i];

        if (el->name && !strcmp(el->name, name))
        {
            found = el;
        }
    }

    if (!found)
    {
        for (i = 0; !found && i < NUJ_COBJECT(element)->child_count; ++i)
        {
            el = NUJ_OBJECT(element)->children[i];

            if (el->type == NUJObject_TYPE || el->type == NUJArray_TYPE)
            {
                found = nuj_find_element_by_name(el, name);
            }
        }
    }

    return found;
}

NUJDEF NUJElement* nuj_parse(NUJHandle handle, const unsigned char* buffer, unsigned long long buffer_size)
{
    NUJParser parser = { .initial = buffer, .current = buffer };
    NUJElement* element = 0;
    int parsing = 1;
    NUJToken token = nuj__parse_get_token(&parser);

    // TODO: Probably we should use this.
    (void)buffer_size;

    if (nuj__parse_match_token(token, NUJ_OBRACE_TYPE))
    {
        element = nuj__parse_element_object(handle, &parser);

        while (parsing)
        {
            token = nuj__parse_get_token(&parser);

            switch (token.type)
            {
                case NUJ_EOF_TYPE:
                case NUJ_CBRACE_TYPE:
                {
                    parsing = 0;
                }
                break;

                default:
                {
                    nuj__parse_error(&parser, token, '}');
                }
                break;
            }
        }
    }
    else
    {
        nuj__parse_error(&parser, token, '{');
    }

    return element;
}

#endif // NU_JSON_IMPLEMENTATION

#define H_NUJ_H
#endif
