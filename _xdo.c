#define _CFFI_
#include <Python.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

/* See doc/misc/parse_c_type.rst in the source of CFFI for more information */

typedef void *_cffi_opcode_t;

#define _CFFI_OP(opcode, arg)   (_cffi_opcode_t)(opcode | (((uintptr_t)(arg)) << 8))
#define _CFFI_GETOP(cffi_opcode)    ((unsigned char)(uintptr_t)cffi_opcode)
#define _CFFI_GETARG(cffi_opcode)   (((intptr_t)cffi_opcode) >> 8)

#define _CFFI_OP_PRIMITIVE       1
#define _CFFI_OP_POINTER         3
#define _CFFI_OP_ARRAY           5
#define _CFFI_OP_OPEN_ARRAY      7
#define _CFFI_OP_STRUCT_UNION    9
#define _CFFI_OP_ENUM           11
#define _CFFI_OP_FUNCTION       13
#define _CFFI_OP_FUNCTION_END   15
#define _CFFI_OP_NOOP           17
#define _CFFI_OP_BITFIELD       19
#define _CFFI_OP_TYPENAME       21
#define _CFFI_OP_CPYTHON_BLTN_V 23   // varargs
#define _CFFI_OP_CPYTHON_BLTN_N 25   // noargs
#define _CFFI_OP_CPYTHON_BLTN_O 27   // O  (i.e. a single arg)
#define _CFFI_OP_CONSTANT       29
#define _CFFI_OP_CONSTANT_INT   31
#define _CFFI_OP_GLOBAL_VAR     33
#define _CFFI_OP_DLOPEN_FUNC    35
#define _CFFI_OP_DLOPEN_CONST   37
#define _CFFI_OP_GLOBAL_VAR_F   39

#define _CFFI_PRIM_VOID          0
#define _CFFI_PRIM_BOOL          1
#define _CFFI_PRIM_CHAR          2
#define _CFFI_PRIM_SCHAR         3
#define _CFFI_PRIM_UCHAR         4
#define _CFFI_PRIM_SHORT         5
#define _CFFI_PRIM_USHORT        6
#define _CFFI_PRIM_INT           7
#define _CFFI_PRIM_UINT          8
#define _CFFI_PRIM_LONG          9
#define _CFFI_PRIM_ULONG        10
#define _CFFI_PRIM_LONGLONG     11
#define _CFFI_PRIM_ULONGLONG    12
#define _CFFI_PRIM_FLOAT        13
#define _CFFI_PRIM_DOUBLE       14
#define _CFFI_PRIM_LONGDOUBLE   15

#define _CFFI_PRIM_WCHAR        16
#define _CFFI_PRIM_INT8         17
#define _CFFI_PRIM_UINT8        18
#define _CFFI_PRIM_INT16        19
#define _CFFI_PRIM_UINT16       20
#define _CFFI_PRIM_INT32        21
#define _CFFI_PRIM_UINT32       22
#define _CFFI_PRIM_INT64        23
#define _CFFI_PRIM_UINT64       24
#define _CFFI_PRIM_INTPTR       25
#define _CFFI_PRIM_UINTPTR      26
#define _CFFI_PRIM_PTRDIFF      27
#define _CFFI_PRIM_SIZE         28
#define _CFFI_PRIM_SSIZE        29
#define _CFFI_PRIM_INT_LEAST8   30
#define _CFFI_PRIM_UINT_LEAST8  31
#define _CFFI_PRIM_INT_LEAST16  32
#define _CFFI_PRIM_UINT_LEAST16 33
#define _CFFI_PRIM_INT_LEAST32  34
#define _CFFI_PRIM_UINT_LEAST32 35
#define _CFFI_PRIM_INT_LEAST64  36
#define _CFFI_PRIM_UINT_LEAST64 37
#define _CFFI_PRIM_INT_FAST8    38
#define _CFFI_PRIM_UINT_FAST8   39
#define _CFFI_PRIM_INT_FAST16   40
#define _CFFI_PRIM_UINT_FAST16  41
#define _CFFI_PRIM_INT_FAST32   42
#define _CFFI_PRIM_UINT_FAST32  43
#define _CFFI_PRIM_INT_FAST64   44
#define _CFFI_PRIM_UINT_FAST64  45
#define _CFFI_PRIM_INTMAX       46
#define _CFFI_PRIM_UINTMAX      47

#define _CFFI__NUM_PRIM         48
#define _CFFI__UNKNOWN_PRIM           (-1)
#define _CFFI__UNKNOWN_FLOAT_PRIM     (-2)
#define _CFFI__UNKNOWN_LONG_DOUBLE    (-3)


struct _cffi_global_s {
    const char *name;
    void *address;
    _cffi_opcode_t type_op;
    void *size_or_direct_fn;  // OP_GLOBAL_VAR: size, or 0 if unknown
                              // OP_CPYTHON_BLTN_*: addr of direct function
};

struct _cffi_getconst_s {
    unsigned long long value;
    const struct _cffi_type_context_s *ctx;
    int gindex;
};

struct _cffi_struct_union_s {
    const char *name;
    int type_index;          // -> _cffi_types, on a OP_STRUCT_UNION
    int flags;               // _CFFI_F_* flags below
    size_t size;
    int alignment;
    int first_field_index;   // -> _cffi_fields array
    int num_fields;
};
#define _CFFI_F_UNION         0x01   // is a union, not a struct
#define _CFFI_F_CHECK_FIELDS  0x02   // complain if fields are not in the
                                     // "standard layout" or if some are missing
#define _CFFI_F_PACKED        0x04   // for CHECK_FIELDS, assume a packed struct
#define _CFFI_F_EXTERNAL      0x08   // in some other ffi.include()
#define _CFFI_F_OPAQUE        0x10   // opaque

struct _cffi_field_s {
    const char *name;
    size_t field_offset;
    size_t field_size;
    _cffi_opcode_t field_type_op;
};

struct _cffi_enum_s {
    const char *name;
    int type_index;          // -> _cffi_types, on a OP_ENUM
    int type_prim;           // _CFFI_PRIM_xxx
    const char *enumerators; // comma-delimited string
};

struct _cffi_typename_s {
    const char *name;
    int type_index;   /* if opaque, points to a possibly artificial
                         OP_STRUCT which is itself opaque */
};

struct _cffi_type_context_s {
    _cffi_opcode_t *types;
    const struct _cffi_global_s *globals;
    const struct _cffi_field_s *fields;
    const struct _cffi_struct_union_s *struct_unions;
    const struct _cffi_enum_s *enums;
    const struct _cffi_typename_s *typenames;
    int num_globals;
    int num_struct_unions;
    int num_enums;
    int num_typenames;
    const char *const *includes;
    int num_types;
    int flags;      /* future extension */
};

struct _cffi_parse_info_s {
    const struct _cffi_type_context_s *ctx;
    _cffi_opcode_t *output;
    unsigned int output_size;
    size_t error_location;
    const char *error_message;
};

#ifdef _CFFI_INTERNAL
static int parse_c_type(struct _cffi_parse_info_s *info, const char *input);
static int search_in_globals(const struct _cffi_type_context_s *ctx,
                             const char *search, size_t search_len);
static int search_in_struct_unions(const struct _cffi_type_context_s *ctx,
                                   const char *search, size_t search_len);
#endif

/* this block of #ifs should be kept exactly identical between
   c/_cffi_backend.c, cffi/vengine_cpy.py, cffi/vengine_gen.py
   and cffi/_cffi_include.h */
#if defined(_MSC_VER)
# include <malloc.h>   /* for alloca() */
# if _MSC_VER < 1600   /* MSVC < 2010 */
   typedef __int8 int8_t;
   typedef __int16 int16_t;
   typedef __int32 int32_t;
   typedef __int64 int64_t;
   typedef unsigned __int8 uint8_t;
   typedef unsigned __int16 uint16_t;
   typedef unsigned __int32 uint32_t;
   typedef unsigned __int64 uint64_t;
   typedef __int8 int_least8_t;
   typedef __int16 int_least16_t;
   typedef __int32 int_least32_t;
   typedef __int64 int_least64_t;
   typedef unsigned __int8 uint_least8_t;
   typedef unsigned __int16 uint_least16_t;
   typedef unsigned __int32 uint_least32_t;
   typedef unsigned __int64 uint_least64_t;
   typedef __int8 int_fast8_t;
   typedef __int16 int_fast16_t;
   typedef __int32 int_fast32_t;
   typedef __int64 int_fast64_t;
   typedef unsigned __int8 uint_fast8_t;
   typedef unsigned __int16 uint_fast16_t;
   typedef unsigned __int32 uint_fast32_t;
   typedef unsigned __int64 uint_fast64_t;
   typedef __int64 intmax_t;
   typedef unsigned __int64 uintmax_t;
# else
#  include <stdint.h>
# endif
# if _MSC_VER < 1800   /* MSVC < 2013 */
   typedef unsigned char _Bool;
# endif
#else
# include <stdint.h>
# if (defined (__SVR4) && defined (__sun)) || defined(_AIX) || defined(__hpux)
#  include <alloca.h>
# endif
#endif

#ifdef __GNUC__
# define _CFFI_UNUSED_FN  __attribute__((unused))
#else
# define _CFFI_UNUSED_FN  /* nothing */
#endif

/**********  CPython-specific section  **********/
#ifndef PYPY_VERSION


#if PY_MAJOR_VERSION >= 3
# define PyInt_FromLong PyLong_FromLong
#endif

#define _cffi_from_c_double PyFloat_FromDouble
#define _cffi_from_c_float PyFloat_FromDouble
#define _cffi_from_c_long PyInt_FromLong
#define _cffi_from_c_ulong PyLong_FromUnsignedLong
#define _cffi_from_c_longlong PyLong_FromLongLong
#define _cffi_from_c_ulonglong PyLong_FromUnsignedLongLong

#define _cffi_to_c_double PyFloat_AsDouble
#define _cffi_to_c_float PyFloat_AsDouble

#define _cffi_from_c_int(x, type)                                        \
    (((type)-1) > 0 ? /* unsigned */                                     \
        (sizeof(type) < sizeof(long) ?                                   \
            PyInt_FromLong((long)x) :                                    \
         sizeof(type) == sizeof(long) ?                                  \
            PyLong_FromUnsignedLong((unsigned long)x) :                  \
            PyLong_FromUnsignedLongLong((unsigned long long)x)) :        \
        (sizeof(type) <= sizeof(long) ?                                  \
            PyInt_FromLong((long)x) :                                    \
            PyLong_FromLongLong((long long)x)))

#define _cffi_to_c_int(o, type)                                          \
    ((type)(                                                             \
     sizeof(type) == 1 ? (((type)-1) > 0 ? (type)_cffi_to_c_u8(o)        \
                                         : (type)_cffi_to_c_i8(o)) :     \
     sizeof(type) == 2 ? (((type)-1) > 0 ? (type)_cffi_to_c_u16(o)       \
                                         : (type)_cffi_to_c_i16(o)) :    \
     sizeof(type) == 4 ? (((type)-1) > 0 ? (type)_cffi_to_c_u32(o)       \
                                         : (type)_cffi_to_c_i32(o)) :    \
     sizeof(type) == 8 ? (((type)-1) > 0 ? (type)_cffi_to_c_u64(o)       \
                                         : (type)_cffi_to_c_i64(o)) :    \
     (Py_FatalError("unsupported size for type " #type), (type)0)))

#define _cffi_to_c_i8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[1])
#define _cffi_to_c_u8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[2])
#define _cffi_to_c_i16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[3])
#define _cffi_to_c_u16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[4])
#define _cffi_to_c_i32                                                   \
                 ((int(*)(PyObject *))_cffi_exports[5])
#define _cffi_to_c_u32                                                   \
                 ((unsigned int(*)(PyObject *))_cffi_exports[6])
#define _cffi_to_c_i64                                                   \
                 ((long long(*)(PyObject *))_cffi_exports[7])
#define _cffi_to_c_u64                                                   \
                 ((unsigned long long(*)(PyObject *))_cffi_exports[8])
#define _cffi_to_c_char                                                  \
                 ((int(*)(PyObject *))_cffi_exports[9])
#define _cffi_from_c_pointer                                             \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[10])
#define _cffi_to_c_pointer                                               \
    ((char *(*)(PyObject *, CTypeDescrObject *))_cffi_exports[11])
#define _cffi_get_struct_layout                                          \
    not used any more
#define _cffi_restore_errno                                              \
    ((void(*)(void))_cffi_exports[13])
#define _cffi_save_errno                                                 \
    ((void(*)(void))_cffi_exports[14])
#define _cffi_from_c_char                                                \
    ((PyObject *(*)(char))_cffi_exports[15])
#define _cffi_from_c_deref                                               \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[16])
#define _cffi_to_c                                                       \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[17])
#define _cffi_from_c_struct                                              \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[18])
#define _cffi_to_c_wchar_t                                               \
    ((wchar_t(*)(PyObject *))_cffi_exports[19])
#define _cffi_from_c_wchar_t                                             \
    ((PyObject *(*)(wchar_t))_cffi_exports[20])
#define _cffi_to_c_long_double                                           \
    ((long double(*)(PyObject *))_cffi_exports[21])
#define _cffi_to_c__Bool                                                 \
    ((_Bool(*)(PyObject *))_cffi_exports[22])
#define _cffi_prepare_pointer_call_argument                              \
    ((Py_ssize_t(*)(CTypeDescrObject *, PyObject *, char **))_cffi_exports[23])
#define _cffi_convert_array_from_object                                  \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[24])
#define _CFFI_NUM_EXPORTS 25

typedef struct _ctypedescr CTypeDescrObject;

static void *_cffi_exports[_CFFI_NUM_EXPORTS];

#define _cffi_type(index)   (                           \
    assert((((uintptr_t)_cffi_types[index]) & 1) == 0), \
    (CTypeDescrObject *)_cffi_types[index])

static PyObject *_cffi_init(const char *module_name, Py_ssize_t version,
                            const struct _cffi_type_context_s *ctx)
{
    PyObject *module, *o_arg, *new_module;
    void *raw[] = {
        (void *)module_name,
        (void *)version,
        (void *)_cffi_exports,
        (void *)ctx,
    };

    module = PyImport_ImportModule("_cffi_backend");
    if (module == NULL)
        goto failure;

    o_arg = PyLong_FromVoidPtr((void *)raw);
    if (o_arg == NULL)
        goto failure;

    new_module = PyObject_CallMethod(
        module, (char *)"_init_cffi_1_0_external_module", (char *)"O", o_arg);

    Py_DECREF(o_arg);
    Py_DECREF(module);
    return new_module;

  failure:
    Py_XDECREF(module);
    return NULL;
}

_CFFI_UNUSED_FN
static PyObject **_cffi_unpack_args(PyObject *args_tuple, Py_ssize_t expected,
                                    const char *fnname)
{
    if (PyTuple_GET_SIZE(args_tuple) != expected) {
        PyErr_Format(PyExc_TypeError,
                     "%.150s() takes exactly %zd arguments (%zd given)",
                     fnname, expected, PyTuple_GET_SIZE(args_tuple));
        return NULL;
    }
    return &PyTuple_GET_ITEM(args_tuple, 0);   /* pointer to the first item,
                                                  the others follow */
}

#endif
/**********  end CPython-specific section  **********/


#define _cffi_array_len(array)   (sizeof(array) / sizeof((array)[0]))

#define _cffi_prim_int(size, sign)                                      \
    ((size) == 1 ? ((sign) ? _CFFI_PRIM_INT8  : _CFFI_PRIM_UINT8)  :    \
     (size) == 2 ? ((sign) ? _CFFI_PRIM_INT16 : _CFFI_PRIM_UINT16) :    \
     (size) == 4 ? ((sign) ? _CFFI_PRIM_INT32 : _CFFI_PRIM_UINT32) :    \
     (size) == 8 ? ((sign) ? _CFFI_PRIM_INT64 : _CFFI_PRIM_UINT64) :    \
     _CFFI__UNKNOWN_PRIM)

#define _cffi_prim_float(size)                                          \
    ((size) == sizeof(float) ? _CFFI_PRIM_FLOAT :                       \
     (size) == sizeof(double) ? _CFFI_PRIM_DOUBLE :                     \
     (size) == sizeof(long double) ? _CFFI__UNKNOWN_LONG_DOUBLE :       \
     _CFFI__UNKNOWN_FLOAT_PRIM)

#define _cffi_check_int(got, got_nonpos, expected)      \
    ((got_nonpos) == (expected <= 0) &&                 \
     (got) == (unsigned long long)expected)

#ifdef __cplusplus
}
#endif

/************************************************************/


#include <xdo.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xmd.h>


/************************************************************/

static void *_cffi_types[] = {
/*  0 */ _CFFI_OP(_CFFI_OP_FUNCTION, 222), // char const * *()(void)
/*  1 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  2 */ _CFFI_OP(_CFFI_OP_FUNCTION, 36), // char const *()(void)
/*  3 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  4 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t *, int)
/*  5 */ _CFFI_OP(_CFFI_OP_POINTER, 231), // xdo_t *
/*  6 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7), // int
/*  7 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/*  8 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t *, unsigned int *, unsigned int *, int)
/*  9 */ _CFFI_OP(_CFFI_OP_NOOP, 5),
/* 10 */ _CFFI_OP(_CFFI_OP_POINTER, 127), // unsigned int *
/* 11 */ _CFFI_OP(_CFFI_OP_NOOP, 10),
/* 12 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 13 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 14 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window *)
/* 15 */ _CFFI_OP(_CFFI_OP_POINTER, 231), // xdo_t const *
/* 16 */ _CFFI_OP(_CFFI_OP_POINTER, 20), // Window *
/* 17 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 18 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window)
/* 19 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 20 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 5), // Window
/* 21 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 22 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, Window *, int)
/* 23 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 24 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 25 */ _CFFI_OP(_CFFI_OP_NOOP, 16),
/* 26 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 27 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 28 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, Window)
/* 29 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 30 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 31 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 32 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 33 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, char const *, char const *)
/* 34 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 35 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 36 */ _CFFI_OP(_CFFI_OP_POINTER, 223), // char const *
/* 37 */ _CFFI_OP(_CFFI_OP_NOOP, 36),
/* 38 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 39 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, char const *, unsigned char * *, long *, Atom *, int *)
/* 40 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 41 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 42 */ _CFFI_OP(_CFFI_OP_NOOP, 36),
/* 43 */ _CFFI_OP(_CFFI_OP_POINTER, 226), // unsigned char * *
/* 44 */ _CFFI_OP(_CFFI_OP_POINTER, 109), // long *
/* 45 */ _CFFI_OP(_CFFI_OP_POINTER, 191), // Atom *
/* 46 */ _CFFI_OP(_CFFI_OP_POINTER, 6), // int *
/* 47 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 48 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, char const *, useconds_t)
/* 49 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 50 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 51 */ _CFFI_OP(_CFFI_OP_NOOP, 36),
/* 52 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 7), // useconds_t
/* 53 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 54 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, charcodemap_t *, int)
/* 55 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 56 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 57 */ _CFFI_OP(_CFFI_OP_POINTER, 224), // charcodemap_t *
/* 58 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 59 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 60 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, charcodemap_t *, int, int, int *, useconds_t)
/* 61 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 62 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 63 */ _CFFI_OP(_CFFI_OP_NOOP, 57),
/* 64 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 65 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 66 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 67 */ _CFFI_OP(_CFFI_OP_NOOP, 52),
/* 68 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 69 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, int *, int *, Screen * *)
/* 70 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 71 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 72 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 73 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 74 */ _CFFI_OP(_CFFI_OP_POINTER, 217), // Screen * *
/* 75 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 76 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, int)
/* 77 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 78 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 79 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 80 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 81 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, int, int)
/* 82 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 83 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 84 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 85 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 86 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 87 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, int, int, int)
/* 88 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 89 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 90 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 91 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 92 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 93 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 94 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, int, int, useconds_t)
/* 95 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 96 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 97 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 98 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 99 */ _CFFI_OP(_CFFI_OP_NOOP, 52),
/* 100 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 101 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, long *)
/* 102 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 103 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 104 */ _CFFI_OP(_CFFI_OP_NOOP, 44),
/* 105 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 106 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, long)
/* 107 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 108 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 109 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 9), // long
/* 110 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 111 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, unsigned char * *, int *, int *)
/* 112 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 113 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 114 */ _CFFI_OP(_CFFI_OP_NOOP, 43),
/* 115 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 116 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 117 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 118 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, unsigned int *, unsigned int *)
/* 119 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 120 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 121 */ _CFFI_OP(_CFFI_OP_NOOP, 10),
/* 122 */ _CFFI_OP(_CFFI_OP_NOOP, 10),
/* 123 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 124 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, unsigned int, unsigned int, int, int)
/* 125 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 126 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 127 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 8), // unsigned int
/* 128 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 8),
/* 129 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 130 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 131 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 132 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, Window, unsigned int, unsigned int, unsigned int *, unsigned int *)
/* 133 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 134 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 135 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 8),
/* 136 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 8),
/* 137 */ _CFFI_OP(_CFFI_OP_NOOP, 10),
/* 138 */ _CFFI_OP(_CFFI_OP_NOOP, 10),
/* 139 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 140 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, charcodemap_t * *, int *)
/* 141 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 142 */ _CFFI_OP(_CFFI_OP_POINTER, 57), // charcodemap_t * *
/* 143 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 144 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 145 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, int *, int *)
/* 146 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 147 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 148 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 149 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 150 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, int *, int *, int *)
/* 151 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 152 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 153 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 154 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 155 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 156 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, int *, int *, int *, Window *)
/* 157 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 158 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 159 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 160 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 161 */ _CFFI_OP(_CFFI_OP_NOOP, 16),
/* 162 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 163 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, int, int)
/* 164 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 165 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 166 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 167 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 168 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, int, int, int)
/* 169 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 170 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 171 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 172 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 173 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 174 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, long *)
/* 175 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 176 */ _CFFI_OP(_CFFI_OP_NOOP, 44),
/* 177 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 178 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, long)
/* 179 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 180 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 9),
/* 181 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 182 */ _CFFI_OP(_CFFI_OP_FUNCTION, 6), // int()(xdo_t const *, xdo_search_t const *, Window * *, unsigned int *)
/* 183 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 184 */ _CFFI_OP(_CFFI_OP_POINTER, 230), // xdo_search_t const *
/* 185 */ _CFFI_OP(_CFFI_OP_POINTER, 16), // Window * *
/* 186 */ _CFFI_OP(_CFFI_OP_NOOP, 10),
/* 187 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 188 */ _CFFI_OP(_CFFI_OP_FUNCTION, 226), // unsigned char *()(xdo_t const *, Window, Atom, long *, Atom *, int *)
/* 189 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 190 */ _CFFI_OP(_CFFI_OP_NOOP, 20),
/* 191 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 0), // Atom
/* 192 */ _CFFI_OP(_CFFI_OP_NOOP, 44),
/* 193 */ _CFFI_OP(_CFFI_OP_NOOP, 45),
/* 194 */ _CFFI_OP(_CFFI_OP_NOOP, 46),
/* 195 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 196 */ _CFFI_OP(_CFFI_OP_FUNCTION, 127), // unsigned int()(xdo_t const *)
/* 197 */ _CFFI_OP(_CFFI_OP_NOOP, 15),
/* 198 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 199 */ _CFFI_OP(_CFFI_OP_FUNCTION, 228), // void()(xdo_t *)
/* 200 */ _CFFI_OP(_CFFI_OP_NOOP, 5),
/* 201 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 202 */ _CFFI_OP(_CFFI_OP_FUNCTION, 228), // void()(xdo_t *, int)
/* 203 */ _CFFI_OP(_CFFI_OP_NOOP, 5),
/* 204 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 205 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 206 */ _CFFI_OP(_CFFI_OP_FUNCTION, 5), // xdo_t *()(Display *, char const *, int)
/* 207 */ _CFFI_OP(_CFFI_OP_POINTER, 214), // Display *
/* 208 */ _CFFI_OP(_CFFI_OP_NOOP, 36),
/* 209 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 7),
/* 210 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 211 */ _CFFI_OP(_CFFI_OP_FUNCTION, 5), // xdo_t *()(char const *)
/* 212 */ _CFFI_OP(_CFFI_OP_NOOP, 36),
/* 213 */ _CFFI_OP(_CFFI_OP_FUNCTION_END, 0),
/* 214 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 1), // Display
/* 215 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 2), // KeyCode
/* 216 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 3), // KeySym
/* 217 */ _CFFI_OP(_CFFI_OP_POINTER, 218), // Screen *
/* 218 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 4), // Screen
/* 219 */ _CFFI_OP(_CFFI_OP_ENUM, 1), // XDO_FEATURES
/* 220 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 6), // XID
/* 221 */ _CFFI_OP(_CFFI_OP_POINTER, 223), // char *
/* 222 */ _CFFI_OP(_CFFI_OP_POINTER, 36), // char const * *
/* 223 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 2), // char
/* 224 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 8), // charcodemap_t
/* 225 */ _CFFI_OP(_CFFI_OP_ENUM, 0), // enum $1
/* 226 */ _CFFI_OP(_CFFI_OP_POINTER, 227), // unsigned char *
/* 227 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 4), // unsigned char
/* 228 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 0), // void
/* 229 */ _CFFI_OP(_CFFI_OP_PRIMITIVE, 16), // wchar_t
/* 230 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 10), // xdo_search_t
/* 231 */ _CFFI_OP(_CFFI_OP_STRUCT_UNION, 9), // xdo_t
};

static int _cffi_const_SEARCH_ANY(unsigned long long *o)
{
  int n = (SEARCH_ANY) <= 0;
  *o = (unsigned long long)((SEARCH_ANY) << 0);  /* check that SEARCH_ANY is an integer */
  return n;
}

static int _cffi_const_SEARCH_ALL(unsigned long long *o)
{
  int n = (SEARCH_ALL) <= 0;
  *o = (unsigned long long)((SEARCH_ALL) << 0);  /* check that SEARCH_ALL is an integer */
  return n;
}

static int _cffi_const_XDO_FEATURE_XTEST(unsigned long long *o)
{
  int n = (XDO_FEATURE_XTEST) <= 0;
  *o = (unsigned long long)((XDO_FEATURE_XTEST) << 0);  /* check that XDO_FEATURE_XTEST is an integer */
  return n;
}

static int _cffi_d_xdo_activate_window(xdo_t const * x0, Window x1)
{
  return xdo_activate_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_activate_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_activate_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_activate_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_activate_window(xdo_t const * x0, Window *x1)
{
  int result;
  { result = xdo_activate_window(x0, *x1); }
  return result;
}
#endif

static int _cffi_d_xdo_clear_active_modifiers(xdo_t const * x0, Window x1, charcodemap_t * x2, int x3)
{
  return xdo_clear_active_modifiers(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_clear_active_modifiers(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  charcodemap_t * x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_clear_active_modifiers");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(57), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (charcodemap_t *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(57), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_clear_active_modifiers(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_clear_active_modifiers(xdo_t const * x0, Window *x1, charcodemap_t * x2, int x3)
{
  int result;
  { result = xdo_clear_active_modifiers(x0, *x1, x2, x3); }
  return result;
}
#endif

static int _cffi_d_xdo_click_window(xdo_t const * x0, Window x1, int x2)
{
  return xdo_click_window(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_click_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_click_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_click_window(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_click_window(xdo_t const * x0, Window *x1, int x2)
{
  int result;
  { result = xdo_click_window(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_click_window_multiple(xdo_t const * x0, Window x1, int x2, int x3, useconds_t x4)
{
  return xdo_click_window_multiple(x0, x1, x2, x3, x4);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_click_window_multiple(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  int x3;
  useconds_t x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 5, "xdo_click_window_multiple");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  if (_cffi_to_c((char *)&x4, _cffi_type(52), arg4) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_click_window_multiple(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_click_window_multiple(xdo_t const * x0, Window *x1, int x2, int x3, useconds_t *x4)
{
  int result;
  { result = xdo_click_window_multiple(x0, *x1, x2, x3, *x4); }
  return result;
}
#endif

static void _cffi_d_xdo_disable_feature(xdo_t * x0, int x1)
{
  xdo_disable_feature(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_disable_feature(PyObject *self, PyObject *args)
{
  xdo_t * x0;
  int x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_disable_feature");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(5), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(5), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { xdo_disable_feature(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_xdo_disable_feature _cffi_d_xdo_disable_feature
#endif

static void _cffi_d_xdo_enable_feature(xdo_t * x0, int x1)
{
  xdo_enable_feature(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_enable_feature(PyObject *self, PyObject *args)
{
  xdo_t * x0;
  int x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_enable_feature");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(5), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(5), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { xdo_enable_feature(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_xdo_enable_feature _cffi_d_xdo_enable_feature
#endif

static int _cffi_d_xdo_enter_text_window(xdo_t const * x0, Window x1, char const * x2, useconds_t x3)
{
  return xdo_enter_text_window(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_enter_text_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  char const * x2;
  useconds_t x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_enter_text_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (char const *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(36), arg2) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x3, _cffi_type(52), arg3) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_enter_text_window(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_enter_text_window(xdo_t const * x0, Window *x1, char const * x2, useconds_t *x3)
{
  int result;
  { result = xdo_enter_text_window(x0, *x1, x2, *x3); }
  return result;
}
#endif

static int _cffi_d_xdo_find_window_client(xdo_t const * x0, Window x1, Window * x2, int x3)
{
  return xdo_find_window_client(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_find_window_client(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Window * x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_find_window_client");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (Window *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(16), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_find_window_client(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_find_window_client(xdo_t const * x0, Window *x1, Window * x2, int x3)
{
  int result;
  { result = xdo_find_window_client(x0, *x1, x2, x3); }
  return result;
}
#endif

static int _cffi_d_xdo_focus_window(xdo_t const * x0, Window x1)
{
  return xdo_focus_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_focus_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_focus_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_focus_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_focus_window(xdo_t const * x0, Window *x1)
{
  int result;
  { result = xdo_focus_window(x0, *x1); }
  return result;
}
#endif

static void _cffi_d_xdo_free(xdo_t * x0)
{
  xdo_free(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_free(PyObject *self, PyObject *arg0)
{
  xdo_t * x0;
  Py_ssize_t datasize;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(5), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(5), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { xdo_free(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  Py_INCREF(Py_None);
  return Py_None;
}
#else
#  define _cffi_f_xdo_free _cffi_d_xdo_free
#endif

static int _cffi_d_xdo_get_active_modifiers(xdo_t const * x0, charcodemap_t * * x1, int * x2)
{
  return xdo_get_active_modifiers(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_active_modifiers(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  charcodemap_t * * x1;
  int * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_get_active_modifiers");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(142), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (charcodemap_t * *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(142), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (int *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(46), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_active_modifiers(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_active_modifiers _cffi_d_xdo_get_active_modifiers
#endif

static int _cffi_d_xdo_get_active_window(xdo_t const * x0, Window * x1)
{
  return xdo_get_active_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_active_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_get_active_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (Window *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(16), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_active_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_active_window _cffi_d_xdo_get_active_window
#endif

static int _cffi_d_xdo_get_current_desktop(xdo_t const * x0, long * x1)
{
  return xdo_get_current_desktop(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_current_desktop(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  long * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_get_current_desktop");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(44), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (long *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(44), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_current_desktop(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_current_desktop _cffi_d_xdo_get_current_desktop
#endif

static int _cffi_d_xdo_get_desktop_for_window(xdo_t const * x0, Window x1, long * x2)
{
  return xdo_get_desktop_for_window(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_desktop_for_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  long * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_get_desktop_for_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(44), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (long *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(44), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_desktop_for_window(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_get_desktop_for_window(xdo_t const * x0, Window *x1, long * x2)
{
  int result;
  { result = xdo_get_desktop_for_window(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_get_desktop_viewport(xdo_t const * x0, int * x1, int * x2)
{
  return xdo_get_desktop_viewport(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_desktop_viewport(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  int * x1;
  int * x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_get_desktop_viewport");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (int *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(46), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (int *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(46), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_desktop_viewport(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_desktop_viewport _cffi_d_xdo_get_desktop_viewport
#endif

static int _cffi_d_xdo_get_focused_window(xdo_t const * x0, Window * x1)
{
  return xdo_get_focused_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_focused_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_get_focused_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (Window *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(16), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_focused_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_focused_window _cffi_d_xdo_get_focused_window
#endif

static int _cffi_d_xdo_get_focused_window_sane(xdo_t const * x0, Window * x1)
{
  return xdo_get_focused_window_sane(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_focused_window_sane(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_get_focused_window_sane");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (Window *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(16), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_focused_window_sane(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_focused_window_sane _cffi_d_xdo_get_focused_window_sane
#endif

static unsigned int _cffi_d_xdo_get_input_state(xdo_t const * x0)
{
  return xdo_get_input_state(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_input_state(PyObject *self, PyObject *arg0)
{
  xdo_t const * x0;
  Py_ssize_t datasize;
  unsigned int result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_input_state(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, unsigned int);
}
#else
#  define _cffi_f_xdo_get_input_state _cffi_d_xdo_get_input_state
#endif

static int _cffi_d_xdo_get_mouse_location(xdo_t const * x0, int * x1, int * x2, int * x3)
{
  return xdo_get_mouse_location(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_mouse_location(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  int * x1;
  int * x2;
  int * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_get_mouse_location");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (int *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(46), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (int *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(46), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (int *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(46), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_mouse_location(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_mouse_location _cffi_d_xdo_get_mouse_location
#endif

static int _cffi_d_xdo_get_mouse_location2(xdo_t const * x0, int * x1, int * x2, int * x3, Window * x4)
{
  return xdo_get_mouse_location2(x0, x1, x2, x3, x4);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_mouse_location2(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  int * x1;
  int * x2;
  int * x3;
  Window * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 5, "xdo_get_mouse_location2");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (int *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(46), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (int *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(46), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (int *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(46), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = (Window *)alloca((size_t)datasize);
    memset((void *)x4, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(16), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_mouse_location2(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_mouse_location2 _cffi_d_xdo_get_mouse_location2
#endif

static int _cffi_d_xdo_get_number_of_desktops(xdo_t const * x0, long * x1)
{
  return xdo_get_number_of_desktops(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_number_of_desktops(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  long * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_get_number_of_desktops");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(44), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (long *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(44), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_number_of_desktops(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_number_of_desktops _cffi_d_xdo_get_number_of_desktops
#endif

static int _cffi_d_xdo_get_pid_window(xdo_t const * x0, Window x1)
{
  return xdo_get_pid_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_pid_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_get_pid_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_pid_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_get_pid_window(xdo_t const * x0, Window *x1)
{
  int result;
  { result = xdo_get_pid_window(x0, *x1); }
  return result;
}
#endif

static char const * * _cffi_d_xdo_get_symbol_map(void)
{
  return xdo_get_symbol_map();
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_symbol_map(PyObject *self, PyObject *noarg)
{
  char const * * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_symbol_map(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  (void)noarg; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(222));
}
#else
#  define _cffi_f_xdo_get_symbol_map _cffi_d_xdo_get_symbol_map
#endif

static int _cffi_d_xdo_get_viewport_dimensions(xdo_t * x0, unsigned int * x1, unsigned int * x2, int x3)
{
  return xdo_get_viewport_dimensions(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_viewport_dimensions(PyObject *self, PyObject *args)
{
  xdo_t * x0;
  unsigned int * x1;
  unsigned int * x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_get_viewport_dimensions");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(5), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(5), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(10), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (unsigned int *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(10), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(10), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (unsigned int *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(10), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_viewport_dimensions(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_viewport_dimensions _cffi_d_xdo_get_viewport_dimensions
#endif

static int _cffi_d_xdo_get_window_at_mouse(xdo_t const * x0, Window * x1)
{
  return xdo_get_window_at_mouse(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_window_at_mouse(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_get_window_at_mouse");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (Window *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(16), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_window_at_mouse(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_get_window_at_mouse _cffi_d_xdo_get_window_at_mouse
#endif

static int _cffi_d_xdo_get_window_location(xdo_t const * x0, Window x1, int * x2, int * x3, Screen * * x4)
{
  return xdo_get_window_location(x0, x1, x2, x3, x4);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_window_location(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int * x2;
  int * x3;
  Screen * * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 5, "xdo_get_window_location");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (int *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(46), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (int *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(46), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(74), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = (Screen * *)alloca((size_t)datasize);
    memset((void *)x4, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(74), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_window_location(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_get_window_location(xdo_t const * x0, Window *x1, int * x2, int * x3, Screen * * x4)
{
  int result;
  { result = xdo_get_window_location(x0, *x1, x2, x3, x4); }
  return result;
}
#endif

static int _cffi_d_xdo_get_window_name(xdo_t const * x0, Window x1, unsigned char * * x2, int * x3, int * x4)
{
  return xdo_get_window_name(x0, x1, x2, x3, x4);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_window_name(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  unsigned char * * x2;
  int * x3;
  int * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 5, "xdo_get_window_name");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (unsigned char * *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(43), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (int *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(46), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = (int *)alloca((size_t)datasize);
    memset((void *)x4, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(46), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_window_name(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_get_window_name(xdo_t const * x0, Window *x1, unsigned char * * x2, int * x3, int * x4)
{
  int result;
  { result = xdo_get_window_name(x0, *x1, x2, x3, x4); }
  return result;
}
#endif

static int _cffi_d_xdo_get_window_property(xdo_t const * x0, Window x1, char const * x2, unsigned char * * x3, long * x4, Atom * x5, int * x6)
{
  return xdo_get_window_property(x0, x1, x2, x3, x4, x5, x6);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_window_property(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  char const * x2;
  unsigned char * * x3;
  long * x4;
  Atom * x5;
  int * x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 7, "xdo_get_window_property");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];
  arg5 = aa[5];
  arg6 = aa[6];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (char const *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(36), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(43), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (unsigned char * *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(43), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(44), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = (long *)alloca((size_t)datasize);
    memset((void *)x4, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(44), arg4) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(45), arg5, (char **)&x5);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x5 = (Atom *)alloca((size_t)datasize);
    memset((void *)x5, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x5, _cffi_type(45), arg5) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = (int *)alloca((size_t)datasize);
    memset((void *)x6, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(46), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_window_property(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_get_window_property(xdo_t const * x0, Window *x1, char const * x2, unsigned char * * x3, long * x4, Atom * x5, int * x6)
{
  int result;
  { result = xdo_get_window_property(x0, *x1, x2, x3, x4, x5, x6); }
  return result;
}
#endif

static unsigned char * _cffi_d_xdo_get_window_property_by_atom(xdo_t const * x0, Window x1, Atom x2, long * x3, Atom * x4, int * x5)
{
  return xdo_get_window_property_by_atom(x0, x1, x2, x3, x4, x5);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_window_property_by_atom(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Atom x2;
  long * x3;
  Atom * x4;
  int * x5;
  Py_ssize_t datasize;
  unsigned char * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 6, "xdo_get_window_property_by_atom");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];
  arg5 = aa[5];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  if (_cffi_to_c((char *)&x2, _cffi_type(191), arg2) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(44), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (long *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(44), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(45), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = (Atom *)alloca((size_t)datasize);
    memset((void *)x4, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(45), arg4) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg5, (char **)&x5);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x5 = (int *)alloca((size_t)datasize);
    memset((void *)x5, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x5, _cffi_type(46), arg5) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_window_property_by_atom(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(226));
}
#else
static unsigned char * _cffi_f_xdo_get_window_property_by_atom(xdo_t const * x0, Window *x1, Atom *x2, long * x3, Atom * x4, int * x5)
{
  unsigned char * result;
  { result = xdo_get_window_property_by_atom(x0, *x1, *x2, x3, x4, x5); }
  return result;
}
#endif

static int _cffi_d_xdo_get_window_size(xdo_t const * x0, Window x1, unsigned int * x2, unsigned int * x3)
{
  return xdo_get_window_size(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_get_window_size(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  unsigned int * x2;
  unsigned int * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_get_window_size");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(10), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (unsigned int *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(10), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(10), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (unsigned int *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(10), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_get_window_size(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_get_window_size(xdo_t const * x0, Window *x1, unsigned int * x2, unsigned int * x3)
{
  int result;
  { result = xdo_get_window_size(x0, *x1, x2, x3); }
  return result;
}
#endif

static int _cffi_d_xdo_has_feature(xdo_t * x0, int x1)
{
  return xdo_has_feature(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_has_feature(PyObject *self, PyObject *args)
{
  xdo_t * x0;
  int x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_has_feature");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(5), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(5), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_has_feature(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_has_feature _cffi_d_xdo_has_feature
#endif

static int _cffi_d_xdo_kill_window(xdo_t const * x0, Window x1)
{
  return xdo_kill_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_kill_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_kill_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_kill_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_kill_window(xdo_t const * x0, Window *x1)
{
  int result;
  { result = xdo_kill_window(x0, *x1); }
  return result;
}
#endif

static int _cffi_d_xdo_map_window(xdo_t const * x0, Window x1)
{
  return xdo_map_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_map_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_map_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_map_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_map_window(xdo_t const * x0, Window *x1)
{
  int result;
  { result = xdo_map_window(x0, *x1); }
  return result;
}
#endif

static int _cffi_d_xdo_minimize_window(xdo_t const * x0, Window x1)
{
  return xdo_minimize_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_minimize_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_minimize_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_minimize_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_minimize_window(xdo_t const * x0, Window *x1)
{
  int result;
  { result = xdo_minimize_window(x0, *x1); }
  return result;
}
#endif

static int _cffi_d_xdo_mouse_down(xdo_t const * x0, Window x1, int x2)
{
  return xdo_mouse_down(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_mouse_down(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_mouse_down");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_mouse_down(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_mouse_down(xdo_t const * x0, Window *x1, int x2)
{
  int result;
  { result = xdo_mouse_down(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_mouse_up(xdo_t const * x0, Window x1, int x2)
{
  return xdo_mouse_up(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_mouse_up(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_mouse_up");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_mouse_up(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_mouse_up(xdo_t const * x0, Window *x1, int x2)
{
  int result;
  { result = xdo_mouse_up(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_move_mouse(xdo_t const * x0, int x1, int x2, int x3)
{
  return xdo_move_mouse(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_move_mouse(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  int x1;
  int x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_move_mouse");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_move_mouse(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_move_mouse _cffi_d_xdo_move_mouse
#endif

static int _cffi_d_xdo_move_mouse_relative(xdo_t const * x0, int x1, int x2)
{
  return xdo_move_mouse_relative(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_move_mouse_relative(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  int x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_move_mouse_relative");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_move_mouse_relative(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_move_mouse_relative _cffi_d_xdo_move_mouse_relative
#endif

static int _cffi_d_xdo_move_mouse_relative_to_window(xdo_t const * x0, Window x1, int x2, int x3)
{
  return xdo_move_mouse_relative_to_window(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_move_mouse_relative_to_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_move_mouse_relative_to_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_move_mouse_relative_to_window(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_move_mouse_relative_to_window(xdo_t const * x0, Window *x1, int x2, int x3)
{
  int result;
  { result = xdo_move_mouse_relative_to_window(x0, *x1, x2, x3); }
  return result;
}
#endif

static int _cffi_d_xdo_move_window(xdo_t const * x0, Window x1, int x2, int x3)
{
  return xdo_move_window(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_move_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_move_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_move_window(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_move_window(xdo_t const * x0, Window *x1, int x2, int x3)
{
  int result;
  { result = xdo_move_window(x0, *x1, x2, x3); }
  return result;
}
#endif

static xdo_t * _cffi_d_xdo_new(char const * x0)
{
  return xdo_new(x0);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_new(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  xdo_t * result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (char const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(36), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_new(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(5));
}
#else
#  define _cffi_f_xdo_new _cffi_d_xdo_new
#endif

static xdo_t * _cffi_d_xdo_new_with_opened_display(Display * x0, char const * x1, int x2)
{
  return xdo_new_with_opened_display(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_new_with_opened_display(PyObject *self, PyObject *args)
{
  Display * x0;
  char const * x1;
  int x2;
  Py_ssize_t datasize;
  xdo_t * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_new_with_opened_display");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(207), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (Display *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(207), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (char const *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(36), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_new_with_opened_display(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(5));
}
#else
#  define _cffi_f_xdo_new_with_opened_display _cffi_d_xdo_new_with_opened_display
#endif

static int _cffi_d_xdo_raise_window(xdo_t const * x0, Window x1)
{
  return xdo_raise_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_raise_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_raise_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_raise_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_raise_window(xdo_t const * x0, Window *x1)
{
  int result;
  { result = xdo_raise_window(x0, *x1); }
  return result;
}
#endif

static int _cffi_d_xdo_reparent_window(xdo_t const * x0, Window x1, Window x2)
{
  return xdo_reparent_window(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_reparent_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Window x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_reparent_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  if (_cffi_to_c((char *)&x2, _cffi_type(20), arg2) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_reparent_window(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_reparent_window(xdo_t const * x0, Window *x1, Window *x2)
{
  int result;
  { result = xdo_reparent_window(x0, *x1, *x2); }
  return result;
}
#endif

static int _cffi_d_xdo_search_windows(xdo_t const * x0, xdo_search_t const * x1, Window * * x2, unsigned int * x3)
{
  return xdo_search_windows(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_search_windows(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  xdo_search_t const * x1;
  Window * * x2;
  unsigned int * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_search_windows");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(184), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (xdo_search_t const *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(184), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(185), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (Window * *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(185), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(10), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (unsigned int *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(10), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_search_windows(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_search_windows _cffi_d_xdo_search_windows
#endif

static int _cffi_d_xdo_select_window_with_click(xdo_t const * x0, Window * x1)
{
  return xdo_select_window_with_click(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_select_window_with_click(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_select_window_with_click");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(16), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = (Window *)alloca((size_t)datasize);
    memset((void *)x1, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(16), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_select_window_with_click(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_select_window_with_click _cffi_d_xdo_select_window_with_click
#endif

static int _cffi_d_xdo_send_keysequence_window(xdo_t const * x0, Window x1, char const * x2, useconds_t x3)
{
  return xdo_send_keysequence_window(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_send_keysequence_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  char const * x2;
  useconds_t x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_send_keysequence_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (char const *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(36), arg2) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x3, _cffi_type(52), arg3) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_send_keysequence_window(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_send_keysequence_window(xdo_t const * x0, Window *x1, char const * x2, useconds_t *x3)
{
  int result;
  { result = xdo_send_keysequence_window(x0, *x1, x2, *x3); }
  return result;
}
#endif

static int _cffi_d_xdo_send_keysequence_window_down(xdo_t const * x0, Window x1, char const * x2, useconds_t x3)
{
  return xdo_send_keysequence_window_down(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_send_keysequence_window_down(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  char const * x2;
  useconds_t x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_send_keysequence_window_down");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (char const *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(36), arg2) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x3, _cffi_type(52), arg3) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_send_keysequence_window_down(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_send_keysequence_window_down(xdo_t const * x0, Window *x1, char const * x2, useconds_t *x3)
{
  int result;
  { result = xdo_send_keysequence_window_down(x0, *x1, x2, *x3); }
  return result;
}
#endif

static int _cffi_d_xdo_send_keysequence_window_list_do(xdo_t const * x0, Window x1, charcodemap_t * x2, int x3, int x4, int * x5, useconds_t x6)
{
  return xdo_send_keysequence_window_list_do(x0, x1, x2, x3, x4, x5, x6);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_send_keysequence_window_list_do(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  charcodemap_t * x2;
  int x3;
  int x4;
  int * x5;
  useconds_t x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 7, "xdo_send_keysequence_window_list_do");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];
  arg5 = aa[5];
  arg6 = aa[6];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(57), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (charcodemap_t *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(57), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_int(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(46), arg5, (char **)&x5);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x5 = (int *)alloca((size_t)datasize);
    memset((void *)x5, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x5, _cffi_type(46), arg5) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x6, _cffi_type(52), arg6) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_send_keysequence_window_list_do(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_send_keysequence_window_list_do(xdo_t const * x0, Window *x1, charcodemap_t * x2, int x3, int x4, int * x5, useconds_t *x6)
{
  int result;
  { result = xdo_send_keysequence_window_list_do(x0, *x1, x2, x3, x4, x5, *x6); }
  return result;
}
#endif

static int _cffi_d_xdo_send_keysequence_window_up(xdo_t const * x0, Window x1, char const * x2, useconds_t x3)
{
  return xdo_send_keysequence_window_up(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_send_keysequence_window_up(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  char const * x2;
  useconds_t x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_send_keysequence_window_up");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (char const *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(36), arg2) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x3, _cffi_type(52), arg3) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_send_keysequence_window_up(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_send_keysequence_window_up(xdo_t const * x0, Window *x1, char const * x2, useconds_t *x3)
{
  int result;
  { result = xdo_send_keysequence_window_up(x0, *x1, x2, *x3); }
  return result;
}
#endif

static int _cffi_d_xdo_set_active_modifiers(xdo_t const * x0, Window x1, charcodemap_t * x2, int x3)
{
  return xdo_set_active_modifiers(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_active_modifiers(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  charcodemap_t * x2;
  int x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_set_active_modifiers");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(57), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (charcodemap_t *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(57), arg2) < 0)
      return NULL;
  }

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_active_modifiers(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_set_active_modifiers(xdo_t const * x0, Window *x1, charcodemap_t * x2, int x3)
{
  int result;
  { result = xdo_set_active_modifiers(x0, *x1, x2, x3); }
  return result;
}
#endif

static int _cffi_d_xdo_set_current_desktop(xdo_t const * x0, long x1)
{
  return xdo_set_current_desktop(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_current_desktop(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  long x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_set_current_desktop");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_current_desktop(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_set_current_desktop _cffi_d_xdo_set_current_desktop
#endif

static int _cffi_d_xdo_set_desktop_for_window(xdo_t const * x0, Window x1, long x2)
{
  return xdo_set_desktop_for_window(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_desktop_for_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  long x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_set_desktop_for_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, long);
  if (x2 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_desktop_for_window(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_set_desktop_for_window(xdo_t const * x0, Window *x1, long x2)
{
  int result;
  { result = xdo_set_desktop_for_window(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_set_desktop_viewport(xdo_t const * x0, int x1, int x2)
{
  return xdo_set_desktop_viewport(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_desktop_viewport(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  int x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_set_desktop_viewport");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_desktop_viewport(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_set_desktop_viewport _cffi_d_xdo_set_desktop_viewport
#endif

static int _cffi_d_xdo_set_number_of_desktops(xdo_t const * x0, long x1)
{
  return xdo_set_number_of_desktops(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_number_of_desktops(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  long x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_set_number_of_desktops");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, long);
  if (x1 == (long)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_number_of_desktops(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_set_number_of_desktops _cffi_d_xdo_set_number_of_desktops
#endif

static int _cffi_d_xdo_set_window_class(xdo_t const * x0, Window x1, char const * x2, char const * x3)
{
  return xdo_set_window_class(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_window_class(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  char const * x2;
  char const * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_set_window_class");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (char const *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(36), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (char const *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(36), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_window_class(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_set_window_class(xdo_t const * x0, Window *x1, char const * x2, char const * x3)
{
  int result;
  { result = xdo_set_window_class(x0, *x1, x2, x3); }
  return result;
}
#endif

static int _cffi_d_xdo_set_window_override_redirect(xdo_t const * x0, Window x1, int x2)
{
  return xdo_set_window_override_redirect(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_window_override_redirect(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_set_window_override_redirect");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_window_override_redirect(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_set_window_override_redirect(xdo_t const * x0, Window *x1, int x2)
{
  int result;
  { result = xdo_set_window_override_redirect(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_set_window_property(xdo_t const * x0, Window x1, char const * x2, char const * x3)
{
  return xdo_set_window_property(x0, x1, x2, x3);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_window_property(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  char const * x2;
  char const * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 4, "xdo_set_window_property");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = (char const *)alloca((size_t)datasize);
    memset((void *)x2, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(36), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(36), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = (char const *)alloca((size_t)datasize);
    memset((void *)x3, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(36), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_window_property(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_set_window_property(xdo_t const * x0, Window *x1, char const * x2, char const * x3)
{
  int result;
  { result = xdo_set_window_property(x0, *x1, x2, x3); }
  return result;
}
#endif

static int _cffi_d_xdo_set_window_size(xdo_t const * x0, Window x1, int x2, int x3, int x4)
{
  return xdo_set_window_size(x0, x1, x2, x3, x4);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_window_size(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  int x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 5, "xdo_set_window_size");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_int(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_int(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_window_size(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_set_window_size(xdo_t const * x0, Window *x1, int x2, int x3, int x4)
{
  int result;
  { result = xdo_set_window_size(x0, *x1, x2, x3, x4); }
  return result;
}
#endif

static int _cffi_d_xdo_set_window_urgency(xdo_t const * x0, Window x1, int x2)
{
  return xdo_set_window_urgency(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_set_window_urgency(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_set_window_urgency");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_set_window_urgency(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_set_window_urgency(xdo_t const * x0, Window *x1, int x2)
{
  int result;
  { result = xdo_set_window_urgency(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_translate_window_with_sizehint(xdo_t const * x0, Window x1, unsigned int x2, unsigned int x3, unsigned int * x4, unsigned int * x5)
{
  return xdo_translate_window_with_sizehint(x0, x1, x2, x3, x4, x5);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_translate_window_with_sizehint(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  unsigned int x2;
  unsigned int x3;
  unsigned int * x4;
  unsigned int * x5;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 6, "xdo_translate_window_with_sizehint");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];
  arg5 = aa[5];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, unsigned int);
  if (x2 == (unsigned int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_int(arg3, unsigned int);
  if (x3 == (unsigned int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(10), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = (unsigned int *)alloca((size_t)datasize);
    memset((void *)x4, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(10), arg4) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(10), arg5, (char **)&x5);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x5 = (unsigned int *)alloca((size_t)datasize);
    memset((void *)x5, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x5, _cffi_type(10), arg5) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_translate_window_with_sizehint(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_translate_window_with_sizehint(xdo_t const * x0, Window *x1, unsigned int x2, unsigned int x3, unsigned int * x4, unsigned int * x5)
{
  int result;
  { result = xdo_translate_window_with_sizehint(x0, *x1, x2, x3, x4, x5); }
  return result;
}
#endif

static int _cffi_d_xdo_unmap_window(xdo_t const * x0, Window x1)
{
  return xdo_unmap_window(x0, x1);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_unmap_window(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 2, "xdo_unmap_window");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_unmap_window(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_unmap_window(xdo_t const * x0, Window *x1)
{
  int result;
  { result = xdo_unmap_window(x0, *x1); }
  return result;
}
#endif

static char const * _cffi_d_xdo_version(void)
{
  return xdo_version();
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_version(PyObject *self, PyObject *noarg)
{
  char const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_version(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  (void)noarg; /* unused */
  return _cffi_from_c_pointer((char *)result, _cffi_type(36));
}
#else
#  define _cffi_f_xdo_version _cffi_d_xdo_version
#endif

static int _cffi_d_xdo_wait_for_mouse_move_from(xdo_t const * x0, int x1, int x2)
{
  return xdo_wait_for_mouse_move_from(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_wait_for_mouse_move_from(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  int x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_wait_for_mouse_move_from");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_wait_for_mouse_move_from(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_wait_for_mouse_move_from _cffi_d_xdo_wait_for_mouse_move_from
#endif

static int _cffi_d_xdo_wait_for_mouse_move_to(xdo_t const * x0, int x1, int x2)
{
  return xdo_wait_for_mouse_move_to(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_wait_for_mouse_move_to(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  int x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_wait_for_mouse_move_to");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_int(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_wait_for_mouse_move_to(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
#  define _cffi_f_xdo_wait_for_mouse_move_to _cffi_d_xdo_wait_for_mouse_move_to
#endif

static int _cffi_d_xdo_wait_for_window_active(xdo_t const * x0, Window x1, int x2)
{
  return xdo_wait_for_window_active(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_wait_for_window_active(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_wait_for_window_active");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_wait_for_window_active(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_wait_for_window_active(xdo_t const * x0, Window *x1, int x2)
{
  int result;
  { result = xdo_wait_for_window_active(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_wait_for_window_focus(xdo_t const * x0, Window x1, int x2)
{
  return xdo_wait_for_window_focus(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_wait_for_window_focus(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_wait_for_window_focus");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_wait_for_window_focus(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_wait_for_window_focus(xdo_t const * x0, Window *x1, int x2)
{
  int result;
  { result = xdo_wait_for_window_focus(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_wait_for_window_map_state(xdo_t const * x0, Window x1, int x2)
{
  return xdo_wait_for_window_map_state(x0, x1, x2);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_wait_for_window_map_state(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 3, "xdo_wait_for_window_map_state");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_wait_for_window_map_state(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_wait_for_window_map_state(xdo_t const * x0, Window *x1, int x2)
{
  int result;
  { result = xdo_wait_for_window_map_state(x0, *x1, x2); }
  return result;
}
#endif

static int _cffi_d_xdo_wait_for_window_size(xdo_t const * x0, Window x1, unsigned int x2, unsigned int x3, int x4, int x5)
{
  return xdo_wait_for_window_size(x0, x1, x2, x3, x4, x5);
}
#ifndef PYPY_VERSION
static PyObject *
_cffi_f_xdo_wait_for_window_size(PyObject *self, PyObject *args)
{
  xdo_t const * x0;
  Window x1;
  unsigned int x2;
  unsigned int x3;
  int x4;
  int x5;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject **aa;

  aa = _cffi_unpack_args(args, 6, "xdo_wait_for_window_size");
  if (aa == NULL)
    return NULL;
  arg0 = aa[0];
  arg1 = aa[1];
  arg2 = aa[2];
  arg3 = aa[3];
  arg4 = aa[4];
  arg5 = aa[5];

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(15), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = (xdo_t const *)alloca((size_t)datasize);
    memset((void *)x0, 0, (size_t)datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(15), arg0) < 0)
      return NULL;
  }

  if (_cffi_to_c((char *)&x1, _cffi_type(20), arg1) < 0)
    return NULL;

  x2 = _cffi_to_c_int(arg2, unsigned int);
  if (x2 == (unsigned int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_int(arg3, unsigned int);
  if (x3 == (unsigned int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_int(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_int(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = xdo_wait_for_window_size(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  (void)self; /* unused */
  return _cffi_from_c_int(result, int);
}
#else
static int _cffi_f_xdo_wait_for_window_size(xdo_t const * x0, Window *x1, unsigned int x2, unsigned int x3, int x4, int x5)
{
  int result;
  { result = xdo_wait_for_window_size(x0, *x1, x2, x3, x4, x5); }
  return result;
}
#endif

static int _cffi_const_CURRENTWINDOW(unsigned long long *o)
{
  int n = (CURRENTWINDOW) <= 0;
  *o = (unsigned long long)((CURRENTWINDOW) << 0);  /* check that CURRENTWINDOW is an integer */
  return n;
}

static int _cffi_const_SEARCH_CLASS(unsigned long long *o)
{
  int n = (SEARCH_CLASS) <= 0;
  *o = (unsigned long long)((SEARCH_CLASS) << 0);  /* check that SEARCH_CLASS is an integer */
  return n;
}

static int _cffi_const_SEARCH_CLASSNAME(unsigned long long *o)
{
  int n = (SEARCH_CLASSNAME) <= 0;
  *o = (unsigned long long)((SEARCH_CLASSNAME) << 0);  /* check that SEARCH_CLASSNAME is an integer */
  return n;
}

static int _cffi_const_SEARCH_DESKTOP(unsigned long long *o)
{
  int n = (SEARCH_DESKTOP) <= 0;
  *o = (unsigned long long)((SEARCH_DESKTOP) << 0);  /* check that SEARCH_DESKTOP is an integer */
  return n;
}

static int _cffi_const_SEARCH_NAME(unsigned long long *o)
{
  int n = (SEARCH_NAME) <= 0;
  *o = (unsigned long long)((SEARCH_NAME) << 0);  /* check that SEARCH_NAME is an integer */
  return n;
}

static int _cffi_const_SEARCH_ONLYVISIBLE(unsigned long long *o)
{
  int n = (SEARCH_ONLYVISIBLE) <= 0;
  *o = (unsigned long long)((SEARCH_ONLYVISIBLE) << 0);  /* check that SEARCH_ONLYVISIBLE is an integer */
  return n;
}

static int _cffi_const_SEARCH_PID(unsigned long long *o)
{
  int n = (SEARCH_PID) <= 0;
  *o = (unsigned long long)((SEARCH_PID) << 0);  /* check that SEARCH_PID is an integer */
  return n;
}

static int _cffi_const_SEARCH_SCREEN(unsigned long long *o)
{
  int n = (SEARCH_SCREEN) <= 0;
  *o = (unsigned long long)((SEARCH_SCREEN) << 0);  /* check that SEARCH_SCREEN is an integer */
  return n;
}

static int _cffi_const_SEARCH_TITLE(unsigned long long *o)
{
  int n = (SEARCH_TITLE) <= 0;
  *o = (unsigned long long)((SEARCH_TITLE) << 0);  /* check that SEARCH_TITLE is an integer */
  return n;
}

static int _cffi_const_SIZE_FROM(unsigned long long *o)
{
  int n = (SIZE_FROM) <= 0;
  *o = (unsigned long long)((SIZE_FROM) << 0);  /* check that SIZE_FROM is an integer */
  return n;
}

static int _cffi_const_SIZE_TO(unsigned long long *o)
{
  int n = (SIZE_TO) <= 0;
  *o = (unsigned long long)((SIZE_TO) << 0);  /* check that SIZE_TO is an integer */
  return n;
}

static int _cffi_const_SIZE_USEHINTS(unsigned long long *o)
{
  int n = (SIZE_USEHINTS) <= 0;
  *o = (unsigned long long)((SIZE_USEHINTS) << 0);  /* check that SIZE_USEHINTS is an integer */
  return n;
}

static int _cffi_const_SIZE_USEHINTS_X(unsigned long long *o)
{
  int n = (SIZE_USEHINTS_X) <= 0;
  *o = (unsigned long long)((SIZE_USEHINTS_X) << 0);  /* check that SIZE_USEHINTS_X is an integer */
  return n;
}

static int _cffi_const_SIZE_USEHINTS_Y(unsigned long long *o)
{
  int n = (SIZE_USEHINTS_Y) <= 0;
  *o = (unsigned long long)((SIZE_USEHINTS_Y) << 0);  /* check that SIZE_USEHINTS_Y is an integer */
  return n;
}

static int _cffi_const_XDO_ERROR(unsigned long long *o)
{
  int n = (XDO_ERROR) <= 0;
  *o = (unsigned long long)((XDO_ERROR) << 0);  /* check that XDO_ERROR is an integer */
  return n;
}

static int _cffi_const_XDO_FIND_CHILDREN(unsigned long long *o)
{
  int n = (XDO_FIND_CHILDREN) <= 0;
  *o = (unsigned long long)((XDO_FIND_CHILDREN) << 0);  /* check that XDO_FIND_CHILDREN is an integer */
  return n;
}

static int _cffi_const_XDO_FIND_PARENTS(unsigned long long *o)
{
  int n = (XDO_FIND_PARENTS) <= 0;
  *o = (unsigned long long)((XDO_FIND_PARENTS) << 0);  /* check that XDO_FIND_PARENTS is an integer */
  return n;
}

static int _cffi_const_XDO_SUCCESS(unsigned long long *o)
{
  int n = (XDO_SUCCESS) <= 0;
  *o = (unsigned long long)((XDO_SUCCESS) << 0);  /* check that XDO_SUCCESS is an integer */
  return n;
}

_CFFI_UNUSED_FN
static void _cffi_checkfld__charcodemap_t(charcodemap_t *p)
{
  /* only to generate compile-time warnings or errors */
  (void)p;
  { wchar_t *tmp = &p->key; (void)tmp; }
  { KeyCode *tmp = &p->code; (void)tmp; }
  { KeySym *tmp = &p->symbol; (void)tmp; }
  (void)((p->group) << 1);  /* check that 'charcodemap_t.group' is an integer */
  (void)((p->modmask) << 1);  /* check that 'charcodemap_t.modmask' is an integer */
  (void)((p->needs_binding) << 1);  /* check that 'charcodemap_t.needs_binding' is an integer */
}
struct _cffi_align__charcodemap_t { char x; charcodemap_t y; };

_CFFI_UNUSED_FN
static void _cffi_checkfld__xdo_t(xdo_t *p)
{
  /* only to generate compile-time warnings or errors */
  (void)p;
  { Display * *tmp = &p->xdpy; (void)tmp; }
  { char * *tmp = &p->display_name; (void)tmp; }
  { charcodemap_t * *tmp = &p->charcodes; (void)tmp; }
  (void)((p->charcodes_len) << 1);  /* check that 'xdo_t.charcodes_len' is an integer */
  (void)((p->keycode_high) << 1);  /* check that 'xdo_t.keycode_high' is an integer */
  (void)((p->keycode_low) << 1);  /* check that 'xdo_t.keycode_low' is an integer */
  (void)((p->keysyms_per_keycode) << 1);  /* check that 'xdo_t.keysyms_per_keycode' is an integer */
  (void)((p->close_display_when_freed) << 1);  /* check that 'xdo_t.close_display_when_freed' is an integer */
  (void)((p->quiet) << 1);  /* check that 'xdo_t.quiet' is an integer */
  (void)((p->debug) << 1);  /* check that 'xdo_t.debug' is an integer */
  (void)((p->features_mask) << 1);  /* check that 'xdo_t.features_mask' is an integer */
}
struct _cffi_align__xdo_t { char x; xdo_t y; };

_CFFI_UNUSED_FN
static void _cffi_checkfld__xdo_search_t(xdo_search_t *p)
{
  /* only to generate compile-time warnings or errors */
  (void)p;
  { char const * *tmp = &p->title; (void)tmp; }
  { char const * *tmp = &p->winclass; (void)tmp; }
  { char const * *tmp = &p->winclassname; (void)tmp; }
  { char const * *tmp = &p->winname; (void)tmp; }
  (void)((p->pid) << 1);  /* check that 'xdo_search_t.pid' is an integer */
  (void)((p->max_depth) << 1);  /* check that 'xdo_search_t.max_depth' is an integer */
  (void)((p->only_visible) << 1);  /* check that 'xdo_search_t.only_visible' is an integer */
  (void)((p->screen) << 1);  /* check that 'xdo_search_t.screen' is an integer */
  /* cannot generate 'enum $1' in field 'require': unknown type name */
  (void)((p->searchmask) << 1);  /* check that 'xdo_search_t.searchmask' is an integer */
  (void)((p->desktop) << 1);  /* check that 'xdo_search_t.desktop' is an integer */
  (void)((p->limit) << 1);  /* check that 'xdo_search_t.limit' is an integer */
}
struct _cffi_align__xdo_search_t { char x; xdo_search_t y; };

static const struct _cffi_global_s _cffi_globals[] = {
  { "CURRENTWINDOW", (void *)_cffi_const_CURRENTWINDOW, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SEARCH_ALL", (void *)_cffi_const_SEARCH_ALL, _CFFI_OP(_CFFI_OP_ENUM, -1), (void *)0 },
  { "SEARCH_ANY", (void *)_cffi_const_SEARCH_ANY, _CFFI_OP(_CFFI_OP_ENUM, -1), (void *)0 },
  { "SEARCH_CLASS", (void *)_cffi_const_SEARCH_CLASS, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SEARCH_CLASSNAME", (void *)_cffi_const_SEARCH_CLASSNAME, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SEARCH_DESKTOP", (void *)_cffi_const_SEARCH_DESKTOP, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SEARCH_NAME", (void *)_cffi_const_SEARCH_NAME, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SEARCH_ONLYVISIBLE", (void *)_cffi_const_SEARCH_ONLYVISIBLE, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SEARCH_PID", (void *)_cffi_const_SEARCH_PID, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SEARCH_SCREEN", (void *)_cffi_const_SEARCH_SCREEN, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SEARCH_TITLE", (void *)_cffi_const_SEARCH_TITLE, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SIZE_FROM", (void *)_cffi_const_SIZE_FROM, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SIZE_TO", (void *)_cffi_const_SIZE_TO, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SIZE_USEHINTS", (void *)_cffi_const_SIZE_USEHINTS, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SIZE_USEHINTS_X", (void *)_cffi_const_SIZE_USEHINTS_X, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "SIZE_USEHINTS_Y", (void *)_cffi_const_SIZE_USEHINTS_Y, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "XDO_ERROR", (void *)_cffi_const_XDO_ERROR, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "XDO_FEATURE_XTEST", (void *)_cffi_const_XDO_FEATURE_XTEST, _CFFI_OP(_CFFI_OP_ENUM, -1), (void *)0 },
  { "XDO_FIND_CHILDREN", (void *)_cffi_const_XDO_FIND_CHILDREN, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "XDO_FIND_PARENTS", (void *)_cffi_const_XDO_FIND_PARENTS, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "XDO_SUCCESS", (void *)_cffi_const_XDO_SUCCESS, _CFFI_OP(_CFFI_OP_CONSTANT_INT, -1), (void *)0 },
  { "xdo_activate_window", (void *)_cffi_f_xdo_activate_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 18), (void *)_cffi_d_xdo_activate_window },
  { "xdo_clear_active_modifiers", (void *)_cffi_f_xdo_clear_active_modifiers, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 54), (void *)_cffi_d_xdo_clear_active_modifiers },
  { "xdo_click_window", (void *)_cffi_f_xdo_click_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 76), (void *)_cffi_d_xdo_click_window },
  { "xdo_click_window_multiple", (void *)_cffi_f_xdo_click_window_multiple, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 94), (void *)_cffi_d_xdo_click_window_multiple },
  { "xdo_disable_feature", (void *)_cffi_f_xdo_disable_feature, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 202), (void *)_cffi_d_xdo_disable_feature },
  { "xdo_enable_feature", (void *)_cffi_f_xdo_enable_feature, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 202), (void *)_cffi_d_xdo_enable_feature },
  { "xdo_enter_text_window", (void *)_cffi_f_xdo_enter_text_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 48), (void *)_cffi_d_xdo_enter_text_window },
  { "xdo_find_window_client", (void *)_cffi_f_xdo_find_window_client, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 22), (void *)_cffi_d_xdo_find_window_client },
  { "xdo_focus_window", (void *)_cffi_f_xdo_focus_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 18), (void *)_cffi_d_xdo_focus_window },
  { "xdo_free", (void *)_cffi_f_xdo_free, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 199), (void *)_cffi_d_xdo_free },
  { "xdo_get_active_modifiers", (void *)_cffi_f_xdo_get_active_modifiers, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 140), (void *)_cffi_d_xdo_get_active_modifiers },
  { "xdo_get_active_window", (void *)_cffi_f_xdo_get_active_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 14), (void *)_cffi_d_xdo_get_active_window },
  { "xdo_get_current_desktop", (void *)_cffi_f_xdo_get_current_desktop, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 174), (void *)_cffi_d_xdo_get_current_desktop },
  { "xdo_get_desktop_for_window", (void *)_cffi_f_xdo_get_desktop_for_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 101), (void *)_cffi_d_xdo_get_desktop_for_window },
  { "xdo_get_desktop_viewport", (void *)_cffi_f_xdo_get_desktop_viewport, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 145), (void *)_cffi_d_xdo_get_desktop_viewport },
  { "xdo_get_focused_window", (void *)_cffi_f_xdo_get_focused_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 14), (void *)_cffi_d_xdo_get_focused_window },
  { "xdo_get_focused_window_sane", (void *)_cffi_f_xdo_get_focused_window_sane, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 14), (void *)_cffi_d_xdo_get_focused_window_sane },
  { "xdo_get_input_state", (void *)_cffi_f_xdo_get_input_state, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 196), (void *)_cffi_d_xdo_get_input_state },
  { "xdo_get_mouse_location", (void *)_cffi_f_xdo_get_mouse_location, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 150), (void *)_cffi_d_xdo_get_mouse_location },
  { "xdo_get_mouse_location2", (void *)_cffi_f_xdo_get_mouse_location2, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 156), (void *)_cffi_d_xdo_get_mouse_location2 },
  { "xdo_get_number_of_desktops", (void *)_cffi_f_xdo_get_number_of_desktops, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 174), (void *)_cffi_d_xdo_get_number_of_desktops },
  { "xdo_get_pid_window", (void *)_cffi_f_xdo_get_pid_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 18), (void *)_cffi_d_xdo_get_pid_window },
  { "xdo_get_symbol_map", (void *)_cffi_f_xdo_get_symbol_map, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_N, 0), (void *)_cffi_d_xdo_get_symbol_map },
  { "xdo_get_viewport_dimensions", (void *)_cffi_f_xdo_get_viewport_dimensions, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 8), (void *)_cffi_d_xdo_get_viewport_dimensions },
  { "xdo_get_window_at_mouse", (void *)_cffi_f_xdo_get_window_at_mouse, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 14), (void *)_cffi_d_xdo_get_window_at_mouse },
  { "xdo_get_window_location", (void *)_cffi_f_xdo_get_window_location, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 69), (void *)_cffi_d_xdo_get_window_location },
  { "xdo_get_window_name", (void *)_cffi_f_xdo_get_window_name, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 111), (void *)_cffi_d_xdo_get_window_name },
  { "xdo_get_window_property", (void *)_cffi_f_xdo_get_window_property, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 39), (void *)_cffi_d_xdo_get_window_property },
  { "xdo_get_window_property_by_atom", (void *)_cffi_f_xdo_get_window_property_by_atom, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 188), (void *)_cffi_d_xdo_get_window_property_by_atom },
  { "xdo_get_window_size", (void *)_cffi_f_xdo_get_window_size, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 118), (void *)_cffi_d_xdo_get_window_size },
  { "xdo_has_feature", (void *)_cffi_f_xdo_has_feature, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 4), (void *)_cffi_d_xdo_has_feature },
  { "xdo_kill_window", (void *)_cffi_f_xdo_kill_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 18), (void *)_cffi_d_xdo_kill_window },
  { "xdo_map_window", (void *)_cffi_f_xdo_map_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 18), (void *)_cffi_d_xdo_map_window },
  { "xdo_minimize_window", (void *)_cffi_f_xdo_minimize_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 18), (void *)_cffi_d_xdo_minimize_window },
  { "xdo_mouse_down", (void *)_cffi_f_xdo_mouse_down, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 76), (void *)_cffi_d_xdo_mouse_down },
  { "xdo_mouse_up", (void *)_cffi_f_xdo_mouse_up, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 76), (void *)_cffi_d_xdo_mouse_up },
  { "xdo_move_mouse", (void *)_cffi_f_xdo_move_mouse, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 168), (void *)_cffi_d_xdo_move_mouse },
  { "xdo_move_mouse_relative", (void *)_cffi_f_xdo_move_mouse_relative, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 163), (void *)_cffi_d_xdo_move_mouse_relative },
  { "xdo_move_mouse_relative_to_window", (void *)_cffi_f_xdo_move_mouse_relative_to_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 81), (void *)_cffi_d_xdo_move_mouse_relative_to_window },
  { "xdo_move_window", (void *)_cffi_f_xdo_move_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 81), (void *)_cffi_d_xdo_move_window },
  { "xdo_new", (void *)_cffi_f_xdo_new, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_O, 211), (void *)_cffi_d_xdo_new },
  { "xdo_new_with_opened_display", (void *)_cffi_f_xdo_new_with_opened_display, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 206), (void *)_cffi_d_xdo_new_with_opened_display },
  { "xdo_raise_window", (void *)_cffi_f_xdo_raise_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 18), (void *)_cffi_d_xdo_raise_window },
  { "xdo_reparent_window", (void *)_cffi_f_xdo_reparent_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 28), (void *)_cffi_d_xdo_reparent_window },
  { "xdo_search_windows", (void *)_cffi_f_xdo_search_windows, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 182), (void *)_cffi_d_xdo_search_windows },
  { "xdo_select_window_with_click", (void *)_cffi_f_xdo_select_window_with_click, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 14), (void *)_cffi_d_xdo_select_window_with_click },
  { "xdo_send_keysequence_window", (void *)_cffi_f_xdo_send_keysequence_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 48), (void *)_cffi_d_xdo_send_keysequence_window },
  { "xdo_send_keysequence_window_down", (void *)_cffi_f_xdo_send_keysequence_window_down, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 48), (void *)_cffi_d_xdo_send_keysequence_window_down },
  { "xdo_send_keysequence_window_list_do", (void *)_cffi_f_xdo_send_keysequence_window_list_do, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 60), (void *)_cffi_d_xdo_send_keysequence_window_list_do },
  { "xdo_send_keysequence_window_up", (void *)_cffi_f_xdo_send_keysequence_window_up, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 48), (void *)_cffi_d_xdo_send_keysequence_window_up },
  { "xdo_set_active_modifiers", (void *)_cffi_f_xdo_set_active_modifiers, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 54), (void *)_cffi_d_xdo_set_active_modifiers },
  { "xdo_set_current_desktop", (void *)_cffi_f_xdo_set_current_desktop, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 178), (void *)_cffi_d_xdo_set_current_desktop },
  { "xdo_set_desktop_for_window", (void *)_cffi_f_xdo_set_desktop_for_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 106), (void *)_cffi_d_xdo_set_desktop_for_window },
  { "xdo_set_desktop_viewport", (void *)_cffi_f_xdo_set_desktop_viewport, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 163), (void *)_cffi_d_xdo_set_desktop_viewport },
  { "xdo_set_number_of_desktops", (void *)_cffi_f_xdo_set_number_of_desktops, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 178), (void *)_cffi_d_xdo_set_number_of_desktops },
  { "xdo_set_window_class", (void *)_cffi_f_xdo_set_window_class, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 33), (void *)_cffi_d_xdo_set_window_class },
  { "xdo_set_window_override_redirect", (void *)_cffi_f_xdo_set_window_override_redirect, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 76), (void *)_cffi_d_xdo_set_window_override_redirect },
  { "xdo_set_window_property", (void *)_cffi_f_xdo_set_window_property, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 33), (void *)_cffi_d_xdo_set_window_property },
  { "xdo_set_window_size", (void *)_cffi_f_xdo_set_window_size, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 87), (void *)_cffi_d_xdo_set_window_size },
  { "xdo_set_window_urgency", (void *)_cffi_f_xdo_set_window_urgency, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 76), (void *)_cffi_d_xdo_set_window_urgency },
  { "xdo_translate_window_with_sizehint", (void *)_cffi_f_xdo_translate_window_with_sizehint, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 132), (void *)_cffi_d_xdo_translate_window_with_sizehint },
  { "xdo_unmap_window", (void *)_cffi_f_xdo_unmap_window, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 18), (void *)_cffi_d_xdo_unmap_window },
  { "xdo_version", (void *)_cffi_f_xdo_version, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_N, 2), (void *)_cffi_d_xdo_version },
  { "xdo_wait_for_mouse_move_from", (void *)_cffi_f_xdo_wait_for_mouse_move_from, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 163), (void *)_cffi_d_xdo_wait_for_mouse_move_from },
  { "xdo_wait_for_mouse_move_to", (void *)_cffi_f_xdo_wait_for_mouse_move_to, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 163), (void *)_cffi_d_xdo_wait_for_mouse_move_to },
  { "xdo_wait_for_window_active", (void *)_cffi_f_xdo_wait_for_window_active, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 76), (void *)_cffi_d_xdo_wait_for_window_active },
  { "xdo_wait_for_window_focus", (void *)_cffi_f_xdo_wait_for_window_focus, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 76), (void *)_cffi_d_xdo_wait_for_window_focus },
  { "xdo_wait_for_window_map_state", (void *)_cffi_f_xdo_wait_for_window_map_state, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 76), (void *)_cffi_d_xdo_wait_for_window_map_state },
  { "xdo_wait_for_window_size", (void *)_cffi_f_xdo_wait_for_window_size, _CFFI_OP(_CFFI_OP_CPYTHON_BLTN_V, 124), (void *)_cffi_d_xdo_wait_for_window_size },
};

static const struct _cffi_field_s _cffi_fields[] = {
  { "key", offsetof(charcodemap_t, key),
           sizeof(((charcodemap_t *)0)->key),
           _CFFI_OP(_CFFI_OP_NOOP, 229) },
  { "code", offsetof(charcodemap_t, code),
            sizeof(((charcodemap_t *)0)->code),
            _CFFI_OP(_CFFI_OP_NOOP, 215) },
  { "symbol", offsetof(charcodemap_t, symbol),
              sizeof(((charcodemap_t *)0)->symbol),
              _CFFI_OP(_CFFI_OP_NOOP, 216) },
  { "group", offsetof(charcodemap_t, group),
             sizeof(((charcodemap_t *)0)->group),
             _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "modmask", offsetof(charcodemap_t, modmask),
               sizeof(((charcodemap_t *)0)->modmask),
               _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "needs_binding", offsetof(charcodemap_t, needs_binding),
                     sizeof(((charcodemap_t *)0)->needs_binding),
                     _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "xdpy", offsetof(xdo_t, xdpy),
            sizeof(((xdo_t *)0)->xdpy),
            _CFFI_OP(_CFFI_OP_NOOP, 207) },
  { "display_name", offsetof(xdo_t, display_name),
                    sizeof(((xdo_t *)0)->display_name),
                    _CFFI_OP(_CFFI_OP_NOOP, 221) },
  { "charcodes", offsetof(xdo_t, charcodes),
                 sizeof(((xdo_t *)0)->charcodes),
                 _CFFI_OP(_CFFI_OP_NOOP, 57) },
  { "charcodes_len", offsetof(xdo_t, charcodes_len),
                     sizeof(((xdo_t *)0)->charcodes_len),
                     _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "keycode_high", offsetof(xdo_t, keycode_high),
                    sizeof(((xdo_t *)0)->keycode_high),
                    _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "keycode_low", offsetof(xdo_t, keycode_low),
                   sizeof(((xdo_t *)0)->keycode_low),
                   _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "keysyms_per_keycode", offsetof(xdo_t, keysyms_per_keycode),
                           sizeof(((xdo_t *)0)->keysyms_per_keycode),
                           _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "close_display_when_freed", offsetof(xdo_t, close_display_when_freed),
                                sizeof(((xdo_t *)0)->close_display_when_freed),
                                _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "quiet", offsetof(xdo_t, quiet),
             sizeof(((xdo_t *)0)->quiet),
             _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "debug", offsetof(xdo_t, debug),
             sizeof(((xdo_t *)0)->debug),
             _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "features_mask", offsetof(xdo_t, features_mask),
                     sizeof(((xdo_t *)0)->features_mask),
                     _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "title", offsetof(xdo_search_t, title),
             sizeof(((xdo_search_t *)0)->title),
             _CFFI_OP(_CFFI_OP_NOOP, 36) },
  { "winclass", offsetof(xdo_search_t, winclass),
                sizeof(((xdo_search_t *)0)->winclass),
                _CFFI_OP(_CFFI_OP_NOOP, 36) },
  { "winclassname", offsetof(xdo_search_t, winclassname),
                    sizeof(((xdo_search_t *)0)->winclassname),
                    _CFFI_OP(_CFFI_OP_NOOP, 36) },
  { "winname", offsetof(xdo_search_t, winname),
               sizeof(((xdo_search_t *)0)->winname),
               _CFFI_OP(_CFFI_OP_NOOP, 36) },
  { "pid", offsetof(xdo_search_t, pid),
           sizeof(((xdo_search_t *)0)->pid),
           _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "max_depth", offsetof(xdo_search_t, max_depth),
                 sizeof(((xdo_search_t *)0)->max_depth),
                 _CFFI_OP(_CFFI_OP_NOOP, 109) },
  { "only_visible", offsetof(xdo_search_t, only_visible),
                    sizeof(((xdo_search_t *)0)->only_visible),
                    _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "screen", offsetof(xdo_search_t, screen),
              sizeof(((xdo_search_t *)0)->screen),
              _CFFI_OP(_CFFI_OP_NOOP, 6) },
  { "require", offsetof(xdo_search_t, require),
               sizeof(((xdo_search_t *)0)->require),
               _CFFI_OP(_CFFI_OP_NOOP, 225) },
  { "searchmask", offsetof(xdo_search_t, searchmask),
                  sizeof(((xdo_search_t *)0)->searchmask),
                  _CFFI_OP(_CFFI_OP_NOOP, 127) },
  { "desktop", offsetof(xdo_search_t, desktop),
               sizeof(((xdo_search_t *)0)->desktop),
               _CFFI_OP(_CFFI_OP_NOOP, 109) },
  { "limit", offsetof(xdo_search_t, limit),
             sizeof(((xdo_search_t *)0)->limit),
             _CFFI_OP(_CFFI_OP_NOOP, 127) },
};

static const struct _cffi_struct_union_s _cffi_struct_unions[] = {
  { "$Atom", 191, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "$Display", 214, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "$KeyCode", 215, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "$KeySym", 216, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "$Screen", 218, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "$Window", 20, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "$XID", 220, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "$useconds_t", 52, _CFFI_F_OPAQUE,
    (size_t)-1, -1, -1, 0 /* opaque */ },
  { "charcodemap", 224, _CFFI_F_CHECK_FIELDS,
    sizeof(charcodemap_t), offsetof(struct _cffi_align__charcodemap_t, y), 0, 6 },
  { "xdo", 231, _CFFI_F_CHECK_FIELDS,
    sizeof(xdo_t), offsetof(struct _cffi_align__xdo_t, y), 6, 11 },
  { "xdo_search", 230, _CFFI_F_CHECK_FIELDS,
    sizeof(xdo_search_t), offsetof(struct _cffi_align__xdo_search_t, y), 17, 12 },
};

static const struct _cffi_enum_s _cffi_enums[] = {
  { "$1", 225, _cffi_prim_int(4, 0),
    "SEARCH_ANY,SEARCH_ALL" },
  { "$XDO_FEATURES", 219, _cffi_prim_int(sizeof(XDO_FEATURES), ((XDO_FEATURES)-1) <= 0),
    "XDO_FEATURE_XTEST" },
};

static const struct _cffi_typename_s _cffi_typenames[] = {
  { "Atom", 191 },
  { "Display", 214 },
  { "KeyCode", 215 },
  { "KeySym", 216 },
  { "Screen", 218 },
  { "Window", 20 },
  { "XDO_FEATURES", 219 },
  { "XID", 220 },
  { "charcodemap_t", 224 },
  { "useconds_t", 52 },
  { "xdo_search_t", 230 },
  { "xdo_t", 231 },
};

static const struct _cffi_type_context_s _cffi_type_context = {
  _cffi_types,
  _cffi_globals,
  _cffi_fields,
  _cffi_struct_unions,
  _cffi_enums,
  _cffi_typenames,
  90,  /* num_globals */
  11,  /* num_struct_unions */
  2,  /* num_enums */
  12,  /* num_typenames */
  NULL,  /* no includes */
  232,  /* num_types */
  0,  /* flags */
};

#ifdef PYPY_VERSION
PyMODINIT_FUNC
_cffi_pypyinit__xdo(const void *p[])
{
    p[0] = (const void *)0x2601;
    p[1] = &_cffi_type_context;
}
#  ifdef _MSC_VER
     PyMODINIT_FUNC
#  if PY_MAJOR_VERSION >= 3
     PyInit__xdo(void) { return NULL; }
#  else
     init_xdo(void) { }
#  endif
#  endif
#elif PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit__xdo(void)
{
  return _cffi_init("_xdo", 0x2601, &_cffi_type_context);
}
#else
PyMODINIT_FUNC
init_xdo(void)
{
  _cffi_init("_xdo", 0x2601, &_cffi_type_context);
}
#endif
