/*
 * File: primitives.c
 * ------------------
 * Presents the implementation of the lisp primitives.
 */

#include <interpreter.h>
#include <primitives.h>
#include <evaluator.h>
#include <environment.h>
#include <stack-trace.h>
#include <lisp-objects.h>
#include <list.h>
#include <closure.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <parser.h>

// forward declarations of primitives
static def_primitive(quote);
static def_primitive(atom);
static def_primitive(eq);
static def_primitive(car);
static def_primitive(cdr);
static def_primitive(cons);
static def_primitive(cond);
static def_primitive(set);
static def_primitive(defmacro_form);
static def_primitive(env);
static def_primitive(lambda);
static def_primitive(print_val);
static def_primitive(concat);
static def_primitive(string_length);
static def_primitive(make_vector);
static def_primitive(vector_ref);
static def_primitive(vector_set);
static def_primitive(vector_length);
static def_primitive(hash_val);
static def_primitive(input_line);
static def_primitive(read_file);
static def_primitive(write_file);

static atom_t primitive_reserved_names[] = { "quote", "atom", "eq", "car", "cdr", "cons",
                                             "cond", "set", "defmacro",
                                             "env", "lambda",
                                             "print", "strcat", "strlen",
                                             "mkvec", "vref", "vset", "vlen", "hash",
                                             "input", "read", "write", NULL };

static const primitive_t primitive_functions[] = { &quote, &atom, &eq, &car, &cdr, &cons,
                                                   &cond, &set, &defmacro_form,
                                                   &env, &lambda,
                                                   &print_val, &concat, &string_length,
                                                   &make_vector, &vector_ref, &vector_set,
                                                   &vector_length, &hash_val,
                                                   &input_line, &read_file, &write_file, NULL };

// Helper: after binding a closure, update CAPTURED so it can reference itself (for recursion)
static void enable_self_recursion(obj *value, obj *env_before, obj *env_after) {
  if ((is_closure(value) || is_macro(value)) && CAPTURED(value) == env_before)
    CAPTURED(value) = env_after;
}

obj* get_primitive_library(void) {
  return create_environment(primitive_reserved_names, primitive_functions);
}

obj* new_primitive(primitive_t primitive) {
  obj* o = malloc(sizeof(obj));
  MALLOC_CHECK(o);
  o->objtype = primitive_obj;
  o->reachable = false;
  o->primitive = primitive;
  return o;
}

obj *t(GarbageCollector *gc) { return gc->t_cached; }
obj *nil(GarbageCollector *gc) { return gc->nil_cached; }

static def_primitive(quote) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  return CAR(args);
}

static def_primitive(atom) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* result = eval(CAR(args), interpreter);
  if (is_list(result)) return is_nil(result) ? t(&interpreter->gc) : nil(&interpreter->gc);
  if (is_atom(result)) return t(&interpreter->gc);
  return is_number(result) ? t(&interpreter->gc) : nil(&interpreter->gc);
}

static def_primitive(eq) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* first = eval(ith(args, 0), interpreter);
  if (first == NULL) return NULL;
  obj* second = eval(ith(args, 1), interpreter);
  if (second == NULL) return NULL;

  bool same = compare(first, second);
  return same ? t(&interpreter->gc) : nil(&interpreter->gc);
}

static def_primitive(car) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* arg_value = eval(CAR(args), interpreter);
  if (arg_value == NULL) {
    LOG_ERROR("Error evaluating argument");
    return NULL;
  }

  if (!is_list(arg_value)) {
    LOG_ERROR("Argument is not a list");
    return NULL;
  }
  if (is_nil(arg_value)) return nil(&interpreter->gc);
  return CAR(arg_value);
}

static def_primitive(cdr) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj* arg_value = eval(CAR(args), interpreter);
  if (arg_value == NULL) {
    LOG_ERROR("Error evaluating argument");
    return NULL;
  }

  if (!is_list(arg_value)) {
    LOG_ERROR("Argument is not a list");
    return NULL;
  }

  if (is_nil(arg_value)) return nil(&interpreter->gc);
  if (CDR(arg_value) == NULL) return nil(&interpreter->gc);
  return CDR(arg_value);
}

static def_primitive(cons) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* y = ith(args, 1);
  if (y == NULL) {
    LOG_ERROR("Could not get second argument");
    return NULL;
  }

  obj *car = eval(CAR(args), interpreter);
  if (car == NULL) {
    LOG_ERROR("Error evaluating first argument");
    return NULL;
  }

  obj* cdr = eval(y, interpreter);
  if (cdr == NULL) {
    LOG_ERROR("Error evaluating second argument");
    return NULL;
  }

  if (!is_list(cdr)) {
    LOG_ERROR("Second argument is not list");
    return NULL;
  }

  obj *new_obj = new_list();
  if (new_obj == NULL) {
    LOG_ERROR("could not allocate list element");
    return NULL;
  }
  gc_add(&interpreter->gc, new_obj);

  CAR(new_obj) = car;
  CDR(new_obj) = is_nil(cdr) ? NULL : cdr;

  return new_obj;
}

static def_primitive(cond) {
  if (args == NULL) return nil(&interpreter->gc);

  if (!is_list(args)) {
    LOG_ERROR("Arguments are not a list of pairs");
    return NULL;
  }

  obj *pair = CAR(args);

  if (!is_list(pair)) {
    LOG_ERROR("Conditional pair clause is not a list");
    return NULL;
  }

  if (is_nil(pair)) {
    LOG_ERROR("Empty Conditional pair.");
    return NULL;
  }

  if (list_length(pair) != 2) {
    LOG_ERROR("Conditional pair length was %d, not 2.", list_length(pair));
    return NULL;
  }

  obj *predicate = eval(CAR(pair), interpreter);
  if (is_primitive(predicate)) {
    LOG_ERROR("Cannot cast primitive function as bool.");
    return NULL;
  }

  if (!is_nil(predicate)) {
    obj* e = ith(pair, 1);
    if (e == NULL) {
      LOG_ERROR("Predicate has no associated value");
      return NULL;
    }
    obj *value = eval(e, interpreter);
    if (value == NULL)
      LOG_ERROR("Error evaluating value for predicate");
    return value;
  }

  return cond(CDR(args), interpreter);
}

/**
 * Primitive: set
 * --------------
 * Bind a value to a name in the environment.
 * Usage: (set 'foo 42)
 */
static def_primitive(set) {
  if (!CHECK_NARGS(args, 2)) return NULL;

  obj* var_name = eval(ith(args, 0), interpreter);
  if (is_nil(var_name)) {
    LOG_ERROR("Cannot set empty list");
    return NULL;
  }
  if (is_t(var_name)) {
    LOG_ERROR("Cannot set truth atom");
    return NULL;
  }
  if (!is_atom(var_name)) {
    LOG_ERROR("Can only set atom types");
    return NULL;
  }
  obj* value = eval(ith(args, 1), interpreter);
  if (value == NULL) {
    LOG_ERROR("Error evaluating right-hand-side");
    return NULL;
  }

  obj* env_before = interpreter->env;
  obj** prev_value_p = lookup_entry(var_name, interpreter->env);
  if (prev_value_p == NULL) {
    obj* pair = make_pair(var_name, value);
    obj* new_link = new_list_set(pair, interpreter->env);
    gc_add(&interpreter->gc, CDR(pair));
    gc_add(&interpreter->gc, pair);
    gc_add(&interpreter->gc, new_link);
    interpreter->env = new_link;
  } else {
    *prev_value_p = value;
  }
  enable_self_recursion(value, env_before, interpreter->env);
  return nil(&interpreter->gc);
}

static def_primitive(defmacro_form) {
  if (!CHECK_NARGS(args, 3)) return NULL;

  obj* name = CAR(args);
  if (!is_atom(name) || is_t(name)) {
    LOG_ERROR("defmacro: first argument must be a name");
    return NULL;
  }

  obj* params = ith(args, 1);
  if (!is_list(params) && !is_atom(params)) {
    LOG_ERROR("defmacro: parameters must be a list or symbol");
    return NULL;
  }

  obj* body = ith(args, 2);

  obj* o = new_closure_set(params, body, interpreter->env);
  if (o == NULL) return NULL;
  o->objtype = macro_obj;
  gc_add(&interpreter->gc, o);

  obj** prev_value_p = lookup_entry(name, interpreter->env);
  if (prev_value_p == NULL) {
    obj* pair = make_pair(name, o);
    obj* new_link = new_list_set(pair, interpreter->env);
    gc_add(&interpreter->gc, CDR(pair));
    gc_add(&interpreter->gc, pair);
    gc_add(&interpreter->gc, new_link);
    interpreter->env = new_link;
  } else {
    *prev_value_p = o;
  }
  CAPTURED(o) = interpreter->env;
  return o;
}

static def_primitive(env) {
  if (!check_nargs(__func__, args, 0)) return NULL;
  return interpreter->env;
}

/**
 * Primitive: lambda
 * -----------------
 * Define a non-primitive procedure. Captures the entire lexical environment.
 */
static def_primitive(lambda) {
  if (!CHECK_NARGS_MIN(args, 1)) return NULL;
  if (!CHECK_NARGS_MAX(args, 2)) return NULL;

  obj* params = ith(args, 0);
  if (is_list(params)) {
    FOR_LIST(params, var) {
      if (var == NULL) continue;
      if (is_t(var)) {
        LOG_ERROR("Truth atom can't be parameter");
        return NULL;
      }
      if (is_nil(var)) {
        LOG_ERROR("Empty list can't be a parameter");
        return NULL;
      }
      if (!is_atom(var)) {
        LOG_ERROR("Parameter was not an atom");
        return NULL;
      }
    }
  } else if (!is_atom(params) || is_t(params)) {
    LOG_ERROR("Lambda parameters must be a list or symbol");
    return NULL;
  }

  obj* procedure = ith(args, 1);
  obj* o = new_closure_set(params, procedure, interpreter->env);
  if (o == NULL) {
    LOG_ERROR("Error allocating closure object");
    return NULL;
  }

  gc_add(&interpreter->gc, o);
  return o;
}

static def_primitive(print_val) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj *value = eval(CAR(args), interpreter);
  if (value == NULL) return nil(&interpreter->gc);
  if (is_string(value)) {
    fwrite(STRING(value), 1, STRING_LEN(value), stdout);
  } else {
    expression str = unparse(value);
    if (str) { fputs(str, stdout); free(str); }
  }
  putchar('\n');
  fflush(stdout);
  return nil(&interpreter->gc);
}

static def_primitive(concat) {
  if (!CHECK_NARGS(args, 2)) return NULL;
  obj *a = eval(CAR(args), interpreter);
  obj *b = eval(ith(args, 1), interpreter);
  if (a == NULL || b == NULL) return NULL;

  // Convert both to string representations
  char *sa, *sb;
  bool free_a = false, free_b = false;
  if (is_string(a)) { sa = STRING(a); }
  else { sa = unparse(a); free_a = true; }
  if (is_string(b)) { sb = STRING(b); }
  else { sb = unparse(b); free_b = true; }
  if (sa == NULL || sb == NULL) {
    if (free_a && sa) free(sa);
    if (free_b && sb) free(sb);
    return NULL;
  }

  size_t la = free_a ? strlen(sa) : STRING_LEN(a);
  size_t lb = free_b ? strlen(sb) : STRING_LEN(b);
  char *buf = malloc(la + lb + 1);
  MALLOC_CHECK(buf);
  memcpy(buf, sa, la);
  memcpy(buf + la, sb, lb);
  buf[la + lb] = '\0';

  obj *o = new_string(buf);
  gc_add(&interpreter->gc, o);

  if (free_a) free(sa);
  if (free_b) free(sb);
  free(buf);
  return o;
}

static def_primitive(string_length) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj *value = eval(CAR(args), interpreter);
  if (value == NULL) return NULL;
  if (!is_string(value)) {
    LOG_ERROR("string-length requires a string argument");
    return NULL;
  }
  obj *o = new_int((int)STRING_LEN(value));
  gc_add(&interpreter->gc, o);
  return o;
}

static def_primitive(make_vector) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj *size = eval(CAR(args), interpreter);
  if (size == NULL || !is_int(size)) {
    LOG_ERROR("mkvec requires an integer size");
    return NULL;
  }
  int n = get_int(size);
  if (n < 0) { LOG_ERROR("mkvec: size must be non-negative"); return NULL; }
  obj *v = new_vector(n);
  // Initialize all slots to nil
  for (int i = 0; i < n; i++)
    VECTOR(v)[i] = nil(&interpreter->gc);
  gc_add(&interpreter->gc, v);
  return v;
}

static def_primitive(vector_ref) {
  if (!CHECK_NARGS(args, 2)) return NULL;
  obj *v = eval(CAR(args), interpreter);
  obj *idx = eval(ith(args, 1), interpreter);
  if (v == NULL || idx == NULL) return NULL;
  if (!is_vector(v)) { LOG_ERROR("vref: first argument must be a vector"); return NULL; }
  if (!is_int(idx)) { LOG_ERROR("vref: index must be an integer"); return NULL; }
  int i = get_int(idx);
  if (i < 0 || i >= VECTOR_LEN(v)) {
    LOG_ERROR("vref: index %d out of bounds (length %d)", i, VECTOR_LEN(v));
    return NULL;
  }
  return VECTOR(v)[i];
}

static def_primitive(vector_set) {
  if (!CHECK_NARGS(args, 3)) return NULL;
  obj *v = eval(CAR(args), interpreter);
  obj *idx = eval(ith(args, 1), interpreter);
  obj *val = eval(ith(args, 2), interpreter);
  if (v == NULL || idx == NULL || val == NULL) return NULL;
  if (!is_vector(v)) { LOG_ERROR("vset: first argument must be a vector"); return NULL; }
  if (!is_int(idx)) { LOG_ERROR("vset: index must be an integer"); return NULL; }
  int i = get_int(idx);
  if (i < 0 || i >= VECTOR_LEN(v)) {
    LOG_ERROR("vset: index %d out of bounds (length %d)", i, VECTOR_LEN(v));
    return NULL;
  }
  VECTOR(v)[i] = val;
  return v;
}

static def_primitive(vector_length) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj *v = eval(CAR(args), interpreter);
  if (v == NULL) return NULL;
  if (!is_vector(v)) { LOG_ERROR("vlen: argument must be a vector"); return NULL; }
  obj *o = new_int(VECTOR_LEN(v));
  gc_add(&interpreter->gc, o);
  return o;
}

// FNV-1a hash
static unsigned int fnv1a(const char *data, size_t len) {
  unsigned int h = 2166136261u;
  for (size_t i = 0; i < len; i++) {
    h ^= (unsigned char)data[i];
    h *= 16777619u;
  }
  return h;
}

static def_primitive(hash_val) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj *value = eval(CAR(args), interpreter);
  if (value == NULL) return NULL;
  unsigned int h;
  if (is_atom(value))
    h = fnv1a(ATOM(value), strlen(ATOM(value)));
  else if (is_string(value))
    h = fnv1a(STRING(value), STRING_LEN(value));
  else if (is_int(value)) {
    int v = get_int(value);
    h = fnv1a((const char*)&v, sizeof(v));
  } else if (is_float(value)) {
    float v = get_float(value);
    h = fnv1a((const char*)&v, sizeof(v));
  } else {
    LOG_ERROR("hash: unsupported type");
    return NULL;
  }
  obj *o = new_int((int)(h & 0x7FFFFFFF));  // keep positive
  gc_add(&interpreter->gc, o);
  return o;
}

static def_primitive(input_line) {
  if (!CHECK_NARGS(args, 0)) return NULL;
  char buf[4096];
  if (fgets(buf, sizeof(buf), stdin) == NULL)
    return nil(&interpreter->gc);
  size_t len = strlen(buf);
  if (len > 0 && buf[len - 1] == '\n') buf[--len] = '\0';
  obj *o = new_string(buf);
  gc_add(&interpreter->gc, o);
  return o;
}

static def_primitive(read_file) {
  if (!CHECK_NARGS(args, 1)) return NULL;
  obj *path = eval(CAR(args), interpreter);
  if (path == NULL) return NULL;
  if (!is_string(path)) { LOG_ERROR("read: argument must be a string"); return NULL; }
  FILE *f = fopen(STRING(path), "r");
  if (!f) { LOG_ERROR("read: cannot open \"%s\"", STRING(path)); return NULL; }
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buf = malloc(size + 1);
  MALLOC_CHECK(buf);
  size_t nread = fread(buf, 1, size, f);
  buf[nread] = '\0';
  fclose(f);
  obj *o = new_string(buf);
  free(buf);
  gc_add(&interpreter->gc, o);
  return o;
}

static def_primitive(write_file) {
  if (!CHECK_NARGS(args, 2)) return NULL;
  obj *path = eval(CAR(args), interpreter);
  obj *content = eval(ith(args, 1), interpreter);
  if (path == NULL || content == NULL) return NULL;
  if (!is_string(path)) { LOG_ERROR("write: first argument must be a string"); return NULL; }
  if (!is_string(content)) { LOG_ERROR("write: second argument must be a string"); return NULL; }
  FILE *f = fopen(STRING(path), "w");
  if (!f) { LOG_ERROR("write: cannot open \"%s\"", STRING(path)); return NULL; }
  fwrite(STRING(content), 1, STRING_LEN(content), f);
  fclose(f);
  return content;
}
