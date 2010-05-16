#include <Array.h>
#include <String.h>

ExceptionManager exc;

typedef struct {
	String field1;
	String field2;
} Item;

typedef Array(Item, Items);

void DestroyItem(Item *item) {
	String_Destroy(&item->field1);
	String_Destroy(&item->field2);
}

int main(void) {
	ExceptionManager_Init(&exc);

	String0(&exc);

	Items arr;

	/* Initialize the array allocating memory for 5 elements. You
	 * can add more than 5 elements, but this involves resizing
	 * the array (which is done automatically).
	 */
	Array_Init(&arr, 5);

	/* Fill in the array with 50 elements. If you already know the
	 * amount of elements you are going to add in advance,
	 * use Array_Align() for better performance. This prevents
	 * the situation that every time Array_Push() is called,
	 * more memory is requested from the kernel.
	 */
	for (size_t i = 0; i < 50; i++) {
		Item item;

		item.field1 = String_Clone(Integer_ToString(i));
		item.field2 = String_Clone(Integer_ToString(i + 5));

		Array_Push(&arr, item);
	}

	/* Shrink the array to 40 elements. */
	Array_Resize(&arr, 40, DestroyItem);

	for (size_t i = 0; i < arr.len; i++) {
		String tmp;

		String_Print(tmp = String_Format(
			String("%:%\n"),
			arr.buf[i].field1,
			arr.buf[i].field2));

		String_Destroy(&tmp);
	}

	/* Limit the array to 30 elements, but do not reduce the
	 * allocated memory like Array_Resize() does. */
	Array_Reset(&arr, 30, DestroyItem);

	Array_Destroy(&arr, DestroyItem);

	return EXIT_SUCCESS;
}
