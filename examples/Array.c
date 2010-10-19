#import <Array.h>
#import <String.h>
#import <Integer.h>

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

	Items *arr;

	/* Initialize the array allocating memory for 5 elements. You
	 * can add more than 5 elements, but this involves resizing
	 * the array (which is done automatically).
	 */
	Array_Init(arr, 5);

	/* Fill in the array with 50 elements. If you already know the
	 * amount of elements you are going to add in advance,
	 * use Array_Align() for better performance. This prevents
	 * the situation that every time Array_Push() is called,
	 * more memory is requested from the kernel.
	 */
	for (size_t i = 0; i < 50; i++) {
		Item item = {
			String_Clone(Integer_ToString(i)),
			String_Clone(Integer_ToString(i + 5))
		};

		Array_Push(arr, item);
	}

	for (size_t i = 0; i < arr->len; i++) {
		String_FmtPrint(String("%:%\n"),
			arr->buf[i].field1,
			arr->buf[i].field2);
	}

	Array_Foreach(arr, DestroyItem);

	Array_Destroy(arr);

	return EXIT_SUCCESS;
}
