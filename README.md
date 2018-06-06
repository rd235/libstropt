# stropt
Parse options from a string (supports quotation, option arguments, no malloc needed for parsing)

This small library parses a list of options from a string.
options can be separated by spaces, commas, semicolons, tabs or new line.

*uppercase,bold,underlined*

options may have arguments:

*ro,noatime,uid=0,gid=0*

it is possible to protect symbols and spaces using quote, double quote and backslash.

*values='1,2,3,4',equal=\=*

## stropt function

```C
int stropt(const char *input, char **tags, char **args, char *buf);
```

The following function lists the option tags and arguments
(without modifying the input string).

```C
#include <stropt.h>
void parse_args(char *input) {
  int tagc = stropt(input, NULL, NULL, 0);
  if(tagc > 0) {
    char buf[strlen(input)+1];
    char *tags[tagc];
    char *args[tagc];
    stropt(input, tags, args, buf);
    for (int i=0; i<tagc; i++)
      printf("%s = %s\n",tags[i], args[i]);
  }
}
```

it is possible to use the same input string as the buffer for parsing
(the original value of the input string gets lost in this way).

```C
#include <stropt.h>
void parse_args(char *input) {
  int tagc = stropt(input, NULL, NULL, 0);
  if(tagc > 0) {
    char *tags[tagc];
    char *args[tagc];
    stropt(input, tags, args, input);
    for (int i=0; i<tagc; i++)
      printf("%s = %s\n",tags[i], args[i]);
  }
}
```

A typical use case is in combination with strcase:

```C
#include <strcase.h>

void parse_args(char *input) {
  int tagc = stropt(input, NULL, NULL, 0);
  if(tagc > 0) {
    char buf[strlen(input)+1];
    char *tags[tagc];
    char *args[tagc];
    stropt(input, tags, args, buf);
    for (int i=0; i<tagc; i++) {
			switch(strcase(tags[i])) {
				case STRCASE(r,o): global_flags |= READONLY; break;
				case STRCASE(n,o,a,t,i,m,e): global_flags |= NOATIME; break;
				case STRCASE(uid): global_uid = atoi(args[i]); break;
				case STRCASE(gid): global_gid = atoi(args[i]); break;
				default: fprintf(stderr, "Unknown option "%s\n", tags[i]);
			}
		}
  }
}
```

## stroptx function

```C
int stroptx(const char *input, char *features, char *sep, int flags, char **tags, char **args, char *buf);
```

stroptx is a more flexible and extended interface to stropt:
stropt supports several features:
single quoting ('), double quoting ("), character escape (\), input on several lines (\n), arguments (=)
comment("#")

the string argument named "features" of stroptx allows to enable/disable some of the standard features.
when it is NULL, all features are enabled, otherwise each character of features represent a feature to enable.
so features == NULL is the same as features == "#=\n\'\"\\".
For example to disable comment management feature must be set to "=\n\'\"\\".

the string argument sep of stroptx is the list of separators of tags (the default value is "\t;,").

Flags:
* ```STROPTX_KEEP_QUOTATION_MARKS_IN_TAGS STROPTX_KEEP_QUOTATION_MARKS_IN_ARGS STROPTX_KEEP_QUOTATION_MARKS```
usually strops provides the output values without quotation marks (' " \).
when the followings flags are set the quotation marks are preserved in tags, args or both
* ```STROPTX_ALLOW_MULTIPLE_SEP```
when STROPTX_ALLOW_MULTIPLE_SEP is not set multiple separators one after the other appear as one,
viceversa when it is set each sequence of two separators means an empty field in between
* ```STROPTX_NEWLINE_TAGS```
when STROPTX_NEWLINE_TAGS is set each new line is encoded as a tag "\n"
 
## stropt2str

```C
char *stropt2str(char **tags, char **args, char sep, char eq);
```

stropt2str rebuilds an option string from the tags/args arrays, sep and eq are the characters to use
as separator and assignment marks, repectively.
Tags whose value is STROPTX_DELETED_TAG are skipped in the conversion

## Install

stropt uses cmake. A standard installation procedure is:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```

Use ```-l stropt``` as a gcc/ld option to link the library.
