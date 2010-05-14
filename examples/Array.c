#include <Array.h>
#include <String.h>

ExceptionManager exc;

typedef struct {
	String field1;
	String field2;
} Item;

typedef Array(Item, Items);

int main(void) {
	ExceptionManager_Init(&exc);

	String0(&exc);

	Items arr;

	Array_Init(&arr, 5);

	for (size_t i = 0; i < 50; i++) {
		Item item;

		item.field1 = String_Clone(Integer_ToString(i));
		item.field2 = String_Clone(Integer_ToString(i + 5));

		Array_Push(&arr, item);
	}

	for (size_t i = 0; i < arr.len; i++) {
		String tmp;

		String_Print(tmp = String_Format(
			String("%:%\n"),
			arr.buf[i].field1,
			arr.buf[i].field2));

		String_Destroy(&tmp);
	}

	Array_Destroy(&arr, ^(Item *item) {
		String_Destroy(&item->field1);
		String_Destroy(&item->field2);
	});

	return EXIT_SUCCESS;
}
