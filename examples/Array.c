#import <Array.h>
#import <String.h>
#import <Integer.h>

typedef struct {
	String field1;
	String field2;
} Item;

Array_Define(Item, ItemArray);

int main(void) {
	/* Initialize an array allocating memory for 5 elements. You
	 * can add more than 5 elements, but this involves resizing
	 * the array (which is done automatically).
	 */
	ItemArray *arr = ItemArray_New(5);

	/* Fill in the array with 50 elements. If you already know the
	 * amount of elements you are going to add in advance,
	 * use Array_Align() for better performance. This prevents
	 * the situation that every time Array_Push() is called,
	 * more memory is requested from the kernel.
	 */
	for (size_t i = 0; i < 50; i++) {
		Item item = {
			String_Clone(Int32_ToString(i)),
			String_Clone(Int32_ToString(i + 5))
		};

		ItemArray_Push(&arr, item);
	}

	for (size_t i = 0; i < arr->len; i++) {
		String_FmtPrint(String("%:%\n"),
			arr->buf[i].field1,
			arr->buf[i].field2);
	}

	foreach (item, arr) {
		String_Destroy(&item->field1);
		String_Destroy(&item->field2);
	}

	ItemArray_Free(arr);

	return ExitStatus_Success;
}
