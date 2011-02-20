#import <Array.h>
#import <String.h>
#import <Integer.h>
#import <Terminal.h>

record(Item) {
	String field1;
	String field2;
};

Array(Item, ItemArray);

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
	forward (i, 50) {
		Item item = {
			Integer_ToString(i),
			Integer_ToString(i + 5)
		};

		ItemArray_Push(&arr, item);
	}

	forward (i, arr->len) {
		String_Print(arr->buf[i].field1);
		String_Print($(":"));
		String_Print(arr->buf[i].field2);
		String_Print($("\n"));
	}

	foreach (item, arr) {
		String_Destroy(&item->field1);
		String_Destroy(&item->field2);
	}

	ItemArray_Free(arr);

	return ExitStatus_Success;
}
