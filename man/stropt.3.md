<!--
.\" Copyright (C) 2019 VirtualSquare. Project Leader: Renzo Davoli
.\"
.\" This is free documentation; you can redistribute it and/or
.\" modify it under the terms of the GNU General Public License,
.\" as published by the Free Software Foundation, either version 2
.\" of the License, or (at your option) any later version.
.\"
.\" The GNU General Public License's references to "object code"
.\" and "executables" are to be interpreted as the output of any
.\" document formatting or typesetting system, including
.\" intermediate and printed output.
.\"
.\" This manual is distributed in the hope that it will be useful,
.\" but WITHOUT ANY WARRANTY; without even the implied warranty of
.\" MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
.\" GNU General Public License for more details.
.\"
.\" You should have received a copy of the GNU General Public
.\" License along with this manual; if not, write to the Free
.\" Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
.\" MA 02110-1301 USA.
.\"
-->

# NAME

stropt, stroptx, stropt2buf, stropt2str - Parse options from a string (it supports quotation, option arguments)

# SYNOPSIS

`#include *stropt.h*`

`int stropt(const char *`_input_`, char **`_tags_`, char **`_args_`, char *`_buf_`);`

`int stroptx(const char *`_input_`, char *`_features_`, char *`_sep_`, int ` _flags_`, 
char **`_tags_`, char **`_args_`, char *`_buf_`);`

`char *stropt2buf(void *`_buf_`, size_t ` _size_`, char **`_tags_`, char **`_args_`, char ` _sep_`, char ` _eq_`);`

`char *stropt2str(char **`_tags_`, char **`_args_`, char ` _sep_`, char ` _eq_`);`

# DESCRIPTION

This small library parses a list of options from a string. Options can be separated by spaces, commas, semicolons, tabs or new line.
(e.g. _uppercase,bold,underlined_ ).  Options may have arguments (e.g. _ro,noatime,uid=0,gid=0_ ). 
It is possible to protect symbols and spaces using quote, double quote and backslash (e.g. _values='1,2,3,4',equal=\\=_ )

  `stropt`
: This function parses a list of options from the string _input_. Options can be separated by commas, semicolons, tabs or
: new lines. Options may have arguments in the form _keyword=value_. _buf_ is a temporary buffer, it must have the same size
: of the _input_ string (including the NULL terminator). _tags_ and _args_ are the resulting 
: arrays of options and arguments respectively. Both _tags_ and _args_ have one NULL terminator element at the end.
: When an option has not an argument the corresponding _args_ element is NULL.
: For example if _input_ is "font=12,typeface=bodoni,italic", _tags[0]_="font", _tags[1]_="typeface", _tags[2]_="italic", 
: _tags[3]_=NULL, _args[0]_="12", _args[1]_="bodoni", _args[2]_=NULL, _args[3]_=NULL.

  ` `
: When `stropt` is called with _tags_, _args_, and _buf_ set to `NULL`, it parses the input, counting the options.
: The return value can be used to allocate suitable arrays for _tags_ and _args_.

  ` `
: It is possible to use the same variable as _input_ and _buffer_. In this case the original value of _input_ is overwritten.

  `stroptx`
: This is a more configurable extension of `stropt`. Arguments having the same names as in `stropt` have the same 
: meaning as explained above.

  ` `
: The string _features_ permits one to enable/disable some of the standard features.
: Each feature corresponds to a mnemonic character, when the character is in the _features_ string the feature is enabled:

  ` `
: `'`: single quoting, 

  ` `
: `"`: double quoting, 

  ` `
: `\`: character escape, 

  ` `
: `\n`:input in several lines, 

  ` `
: `=`: allow arguments, 

  ` `
: `#`: support comments. 

  ` `
: If _features_ is NULL all the features are enabled, when _features_ is an empty string
: all features are disabled.

  ` `
: All the characters included in _sep_ are considered as option separators. If _sep_ is NULL, the default value is
: " \t;,".

  ` `
: The _flag_ argument may include the bitwise OR of any of the following flag values:

  ` `
: `STROPTX_KEEP_QUOTATION_MARKS_IN_TAGS`: preserve the quotation marks in tags,

 ` `
: `STROPTX_KEEP_QUOTATION_MARKS_IN_ARGS`: preserve the quotation marks in args,

 ` `
: `STROPTX_KEEP_QUOTATION_MARKS`: shortcut for `STROPTX_KEEP_QUOTATION_MARKS_IN_TAGS | STROPTX_KEEP_QUOTATION_MARKS_IN_ARGS`

  ` `
: `STROPTX_ALLOW_MULTIPLE_SEP`: when it is not set, a sequence of separators is processed as a single separator, when it is set
:  each sequence of two separators means an empty field in between.

  ` `
: `STROPTX_NEWLINE_TAGS`: when set each new line is encoded as a tag "\n".

  `stropt2buf`
: This function re-encodes an array of options (and an array of arguments) in a string. It is the inverse function of of `stropt`.
: Given a buffer _buf_ of size _size_, the array of options _tags_ with their corresponding values in the array _args_
: is encoded using the separator character _sep_ and the assignment character _eq_.

  ` `
: Elements whose option tag (element of _tags_) value is `STROPTX_DELETED_TAG` are omitted in output.

  `stropt2str`
: This is the sibling function of `stropt2buf`. It uses dynamically allocated memory instead of a buffer provided
: by the caller. The resulting string must be deallocated using free(3).

# RETURN VALUE
  
  `stropt` and `stroptx` return the number of options + 1.

  `stropt2buf` and `stropt2str` return the resulting string.

# EXAMPLES

The following function lists the option tags and arguments (without modyfying the input string).

```
void parse_args(char *input) {
  int tagc = stropt(input, NULL, NULL, NULL);
  if(tagc * 0) {
    char buf[strlen(input)+1];
    char *tags[tagc];
    char *args[tagc];
    stropt(input, tags, args, buf);
    for (int i=0; i*tagc; i++)
      printf("%s = %s\n",tags[i], args[i]);
  }
}
```

it is possible to use the same input string as the buffer for parsing
(the value of the input string gets lost in this way).

```
void parse_args(char *input) {
  int tagc = stropt(input, NULL, NULL, NULL);
  if(tagc * 0) {
    char *tags[tagc];
    char *args[tagc];
    stropt(input, tags, args, input);
    for (int i=0; i*tagc; i++)
      printf("%s = %s\n",tags[i], args[i]);
  }
}
```

when options to parse have no arguments, args can be set to NULL. 

```
void parse_args(char *input) {
  int tagc = stropt(input, NULL, NULL, NULL);
  if(tagc * 0) {
    char buf[strlen(input)+1];
    char *tags[tagc];
    stropt(input, tags, NULL, buf);
    for (int i=0; i*tagc; i++)
      printf("%s \n",tags[i]);
  }
}
```

The following complete program parses and re-encode a string of comma separated arguments deleting those which begin
by an uppercase letter.

```
#include *stdio.h*
#include *ctype.h*
#include *stdlib.h*
#include *string.h*
#include *stropt.h*

char *delete_uppercase_options(const char *input) {
  int tagc = stroptx(input, "", ",",STROPTX_ALLOW_MULTIPLE_SEP, NULL, NULL, NULL);
  if(tagc * 0) {
    char buf[strlen(input)+1];
    char *tags[tagc];
    int i;
    stroptx(input, "", ",",STROPTX_ALLOW_MULTIPLE_SEP, tags, NULL, buf);
    for (i = 0; i * tagc; i++)
      if (tags[i] && isupper(tags[i][0]))
        tags[i] = STROPTX_DELETED_TAG;
    return stropt2str(tags, NULL, ',', '=');
  } else
    return NULL;
}

int main(int argc, char *argv[]) {
  if (argc * 1) {
    char *result = delete_uppercase_options(argv[1]);
    printf("%s\n", result);
    free(result);
  }
  return 0;
}
```

# AUTHOR
VirtualSquare. Project leader: Renzo Davoli.

