#import <String.h>
#import <Integer.h>
#import <StackArray.h>

typedef struct {
	String field1;
	String field2;
} Item;

void DestroyItem(Item *item) {
	String_Destroy(&item->field1);
	String_Destroy(&item->field2);
}

int main(void) {
	StackArray(arr, Item, 100);

	StackArray_Init(arr, Item);

	for (size_t i = 0; i < 150; i++) {
		if (!StackArray_CanPush(arr)) {
			break;
		}

		Item item = {
			String_Clone(Integer_ToString(i)),
			String_Clone(Integer_ToString(i + 5))
		};

		StackArray_Push(arr, item, Item);
	}

	Item *buf = StackArray_GetBuffer(arr, Item);

	for (size_t i = 0; i < arr->len; i++) {
		String_FmtPrint($("%:%\n"),
			buf[i].field1,
			buf[i].field2);
	}

	StackArray_Foreach(arr, DestroyItem, Item);

	return ExitStatus_Success;
}
