#if 0
<?php

abstract class Type {
	public $value;

	public function __construct($value) {
		$this->value = $value;
	}

	public function __toString() {
		return (string) $this->value;
	}
}

class Integer extends Type { }
class String  extends Type { }

function String($value) {
	return new String($value);
}

function Integer($value) {
	return new Integer($value);
}

exit(main());
#endif

#import <String.h>

#define echo          String_Print
#define substr        String_Slice
#define String_Create String

/* inverse() */
#define function float // return type
#define Integer    int // first parameter
#define string   float // cast is needed for float division
#undef  throw
#define throw
#define new
#define Exception(x) printf("%s", x); Runtime_Exit(ExitStatus_Failure);
function inverse(Integer $x) {
	if (!$x) {
		throw new Exception("Division by zero.");
	} else {
		return 1 / (string) $x;
	}
}

/* PrintOffset */
#undef  function
#define function bool
#define Integer  int
#undef  string
#define string   int
function PrintSubString(String $s, Integer $offset, Integer $length) {
	echo(substr($s, (string) $offset, (string) $length));

	return true;
}

/* main() */
#undef  function
#define function int
#undef  Integer
#define Integer(x) x
function main() {
	PrintSubString(
		String("Hello\n"),
		Integer(2),
		Integer(3));

	printf("%f", inverse(Integer(12)));
	printf("%f", inverse(Integer(0)));

	return 0;
}
