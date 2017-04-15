define(MAKE_PROTO_SAFE, `echo "$1" | sed 'y%./+-%__p_%'`)
define(MAKE_PROTO_NAME, [dnl
changequote({, })dnl
{NEED_`echo $1 | sed 's/[^a-zA-Z0-9_]/_/g' | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`_PROTO}
changequote([, ])dnl
])

ifdef([AC_AUTOHEADER],
[
  define(AC_CHECK_PROTO, [@@@protos="$protos $1"@@@])
],
[
AC_DEFUN(AC_CHECK_PROTO,
[
dnl Do the transliteration at runtime so arg 1 can be a shell variable.
ac_safe=MAKE_PROTO_SAFE($1)
ac_proto=MAKE_PROTO_NAME($1)

AC_MSG_CHECKING([if $1 is prototyped])
AC_CACHE_VAL(ac_cv_prototype_$ac_safe, [#
AC_TRY_COMPILE([
#define NO_LIBRARY_PROTOTYPES
#define __COMM_C__
#define __ACT_OTHER_C__
#include "src/sysdep.h"
#ifdef $1
  error - already defined!
#endif
void $1(int a, char b, int c, char d, int e, char f, int g, char h);
],dnl
,
eval "ac_cv_prototype_$ac_safe=no",eval "ac_cv_prototype_$ac_safe=yes")
])

if eval "test \"`echo '$ac_cv_prototype_'$ac_safe`\" = yes"; then
  AC_MSG_RESULT(yes)
else
  AC_DEFINE_UNQUOTED($ac_proto)
  AC_MSG_RESULT(no)
fi
])
])

dnl @@@t1="MAKE_PROTO_SAFE($1)"; t2="MAKE_PROTO_NAME($t1)"; literals="$literals $t2"@@@])
