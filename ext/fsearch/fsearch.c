#include <string.h>
#include <fts.h>
#include <fnmatch.h>
#include <ruby.h>

VALUE fsModule = NULL;
VALUE sClass = NULL;

static int find_match(const char *name, const char **lst, int trace) {
    while (*lst != NULL) {
        if (fnmatch(*lst, name, FNM_PATHNAME) == 0) {
            return 1;
        } else {
        }
        ++lst;
    }
    return 0;
}

static int ignore_entry(FTSENT *node, const char **exclude_dp, const char **exclude_fp, const char **only_fp, const char **patterns) {
    if (node->fts_info & FTS_D) {
        return find_match(node->fts_name, exclude_dp, 0);
    } else if (node->fts_info & FTS_F) {
        if (*exclude_fp != NULL && find_match(node->fts_name, exclude_fp, 0)) {
            return 1;
        }
        if (find_match(node->fts_name, patterns, 0)) {
            if (*only_fp != NULL && !find_match(node->fts_name, only_fp, 0)) {
                return 1;
            }
            return 0;
        }
    }
    return 1;
}

static VALUE fsearch_search(VALUE self, VALUE root, VALUE pattern) {
    VALUE dir = rb_ivar_get(self, rb_intern("@dir"));
    VALUE ary = Qnil;
    VALUE result = rb_ary_new();
    Check_Type(dir, T_STRING);
    Check_Type(pattern, T_STRING);
    Check_Type(root, T_STRING);

    const char *rootv = StringValuePtr(root);
    const char *pt = StringValuePtr(pattern);
    const char *patterns[2] = { pt, 0 };

    const char *path = StringValuePtr(dir);
    const char *paths[2] = { path, 0 };

    ary = rb_ivar_get(self, rb_intern("@exclude_dp"));
    const char *exclude_dp[RARRAY(ary)->len + 1];
    for (long i = 0; i < RARRAY(ary)->len; ++i) {
        exclude_dp[i] = StringValuePtr(RARRAY(ary)->ptr[i]);
    }
    exclude_dp[RARRAY(ary)->len] = NULL;

    ary = rb_ivar_get(self, rb_intern("@exclude_fp"));
    const char *exclude_fp[RARRAY(ary)->len+1];
    for (long i = 0; i < RARRAY(ary)->len; ++i) {
        exclude_fp[i] = StringValuePtr(RARRAY(ary)->ptr[i]);
    }
    exclude_fp[RARRAY(ary)->len] = NULL;

    ary = rb_ivar_get(self, rb_intern("@only_fp"));
    const char *only_fp[RARRAY(ary)->len+1];
    for (long i = 0; i < RARRAY(ary)->len; ++i) {
        only_fp[i] = StringValuePtr(RARRAY(ary)->ptr[i]);
    }
    only_fp[RARRAY(ary)->len] = NULL;

    FTS *tree = fts_open(paths, FTS_NOCHDIR, 0);
    if (!tree) {
        rb_raise(rb_eIOError, "Failed to open directory: %s", path);
    }

    FTSENT *node = NULL;
    size_t rootvlen = strlen(rootv);
    while ( (node = fts_read(tree)) != NULL) {
        if (ignore_entry(node, exclude_dp, exclude_fp, only_fp, patterns)) {
            fts_set(tree, node, FTS_SKIP);
        } else if (node->fts_info & FTS_F) {
            if (strncmp(rootv, node->fts_path, rootvlen) == 0) {
                if (node->fts_path[rootvlen] == '/') {
                    rb_ary_push(result, rb_str_new2(node->fts_path + rootvlen + 1));
                } else {
                    rb_ary_push(result, rb_str_new2(node->fts_path + rootvlen));
                }
            } else {
                rb_ary_push(result, rb_str_new2(node->fts_path));
            }
        }
    }

    fts_close(tree);
    return result;
}

static VALUE fsearch_exclude_fp(VALUE self, VALUE lst) {
    return Qtrue;
}

static VALUE fsearch_exclude_dp(VALUE self, VALUE lst) {
    return Qtrue;
}

static VALUE fsearch_only_fp(VALUE self, VALUE lst) {
    return lst;
}

void Init_fsearch(void) {
    fsModule = rb_define_module("FSearch");

    sClass = rb_define_class_under(fsModule, "Search", rb_cObject);
    rb_define_private_method(sClass, "int_search", fsearch_search, 2);
    rb_define_private_method(sClass, "exclude_file_patterns", fsearch_exclude_fp, 1);
    rb_define_private_method(sClass, "exclude_dir_patterns", fsearch_exclude_dp, 1);
    rb_define_private_method(sClass, "only_file_patterns", fsearch_only_fp, 1);
}
