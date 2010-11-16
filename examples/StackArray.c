#import <String.h>
#import <StackArray.h>

ExceptionManager exc;

typedef struct {
	String field1;
	String field2;
} Item;

void DestroyItem(Item *item) {
	String_Destroy(&item->field1);
	String_Destroy(&item->field2);
}

int main(void) {
	ExceptionManager_Init(&exc);

	String0(&exc);

	StackArray(arr, Item, 100);

	StackArray_Init(arr, Item);

	for (size_t i = 0; i < 150; i++) {
		if (!StackArray_CanPush(arr)) {
			break;
		}

		Item item = {
			String_Clone(Int16_ToString(i)),
			String_Clone(Int16_ToString(i + 5))
		};

		StackArray_Push(arr, item, Item);
	}

	Item *buf = StackArray_GetBuffer(arr, Item);

	for (size_t i = 0; i < arr->len; i++) {
		String_FmtPrint(String("%:%\n"),
			buf[i].field1,
			buf[i].field2);
	}

	StackArray_Foreach(arr, DestroyItem, Item);

	return ExitStatus_Success;
}
