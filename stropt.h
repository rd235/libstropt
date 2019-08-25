#ifndef STROPT_H
#define STROPT_H

/* stropt parses a list of options from a string.
	 options can be separated by spaces, commas, semicolons, tabs or new line.
	 e.g. "uppercase,bold,underlined"
	 options may have arguments: "font=12,typeface=bodoni,italic"
	 it is possible to protect symbols and spaces using quote, double quote and backslash.
	 e.g. "values='1,2,3,4',equal=\="

	 stropt always returns the number of options in the input string parameters plus one.
	 actually stropt has been designed to be called twice, the first call counts the
	 options in order to allocate the arrays for option-tags and arguments for the second call.
 */

/* The following function lists the option tags and arguments 
	 (without modyfying the input string).
 */
#if 0
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
#endif
/* it is possible to use the same input string as the buffer for parsing 
	 (the value of the input string gets lost in this way).*/
#if 0
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
#endif
/* when options to parse have no arguments, args can be set to NULL. The examples above become: */
#if 0
void parse_args(char *input) {
	int tagc = stropt(input, NULL, NULL, 0);
	if(tagc > 0) {
		char buf[strlen(input)+1];
		char *tags[tagc];
		stropt(input, tags, NULL, buf);
		for (int i=0; i<tagc; i++)
			printf("%s\n",tags[i]);
	}
}
#endif
#if 0
void parse_args(char *input) {
  int tagc = stropt(input, NULL, NULL, 0);
  if(tagc > 0) {
    char *tags[tagc];
    stropt(input, tags, NULL, input);
    for (int i=0; i<tagc; i++)
      printf("%s\n",tags[i]);
  }
}
#endif

int stropt(const char *input, char **tags, char **args, char *buf);

/* stroptx is a more flexible and extended interface to stropt:
	 stropt supports several features:
	 single quoting ('), double quoting ("), character escape (\), input on several lines (\n), arguments (=)
	 comment("#")

	 the string argument named "features" of stroptx allows to enable/disable some of the standard features.
	 when it is NULL, all features are enabled, otherwise each character of features represent a feature to enable.
	 so features == NULL is the same as features == "#=\n\'\"\\".
	 For example to disable comment management feature must be set to "=\n\'\"\\".

	 the string argument sep of stroptx is the list of separators of tags (the default value is " \t;,").
 */

int stroptx(const char *input, char *features, char *sep, int flags, char **tags, char **args, char *buf);

/* flags: 
	 usually strops use quotation to provide the value without quotation marks (' " \).
	 when the followings flags are set the quotation marks are preserved in tags, args or both */
#define STROPTX_KEEP_QUOTATION_MARKS_IN_TAGS 0x1
#define STROPTX_KEEP_QUOTATION_MARKS_IN_ARGS 0x2
#define STROPTX_KEEP_QUOTATION_MARKS 0x3
/* when STROPTX_ALLOW_MULTIPLE_SEP is not set multiple separators one after the other appear as one,
	 viceversa when it is set each sequence of two separators means an empty field in between */
#define STROPTX_ALLOW_MULTIPLE_SEP   0x4
/* when STROPTX_NEWLINE_TAGS is set each new line is encoded as a tag "\n" */
#define STROPTX_NEWLINE_TAGS         0x8

/* rebuild an option string from the tags/args arrays.
	 tags whose value is STROPTX_DELETED_TAG are skipped in the conversion */
/* stropt2buf uses a buffer while the output of stropt2str is dynamically allocated */
/* args == NULL means that options have no arguments */

char *stropt2buf(void *buf, size_t size, char **tags, char **args, char sep, char eq);

static inline char *stropt2str(char **tags, char **args, char sep, char eq) {
	return stropt2buf(NULL, 0, tags, args, sep, eq);
}

#define STROPTX_DELETED_TAG ((char *) -1)
#endif
