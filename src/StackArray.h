typedef struct {
	size_t len;
	size_t size;
	void *buf[0];
} StackArray;

/* Calculates the number of `StackArray' instances being necessary
 * to hold one item.
 */
#define StackArray_OneItem(item)                        \
	((sizeof(item) <= sizeof(StackArray))               \
	 ? sizeof(StackArray)                               \
	 : (((sizeof(item) % sizeof(StackArray)) == 0)      \
		 ?   (sizeof(item) / sizeof(StackArray))        \
		 :  ((sizeof(item) / sizeof(StackArray)) + 1)))

/* Same as StackArray_OneItem() but the result fit for `len' items. */
#define StackArray_ManyItems(item, len)                \
	(((sizeof(item) * len) / sizeof(StackArray)) + (   \
	(((sizeof(item) * len) % sizeof(StackArray)) != 0) \
		? StackArray_OneItem(item)                     \
		: 0))

/* Calculates the final size ensuring that sufficient space for the
 * StackArray headers `len' and `size' is available.
 */
#define StackArray_CalcSize(item, len)                                  \
	(StackArray_ManyItems(item, len) +                                  \
		((((sizeof(StackArray) + sizeof(item) * len) -                  \
		   (StackArray_ManyItems(item, len)) * sizeof(StackArray)) > 0) \
			? 1                                                         \
			: 0))

#define StackArray(name, item, len) \
	StackArray name[StackArray_CalcSize(item, len)]

#define StackArray_GetSize(this, item) \
	(sizeof(this) / sizeof(item))

#define StackArray_Init(this, item)                    \
	do {                                               \
		(this)->len  = 0;                              \
		(this)->size = StackArray_GetSize(this, item); \
	} while(0)

#define StackArray_GetBuffer(this, item) \
	((item *) &(this)->buf[0])

#define StackArray_At(this, i, item) \
	StackArray_GetBuffer(this, item)[i]

#define StackArray_Foreach(this, cb, item)         \
	do {                                           \
		for (size_t i = 0; i < (this)->len; i++) { \
			cb(&StackArray_At(this, i, item));     \
		}                                          \
	} while(0)

#define StackArray_CanPush(this) \
	(((this)->size != 0 && ((this)->len + 1) <= (this)->size))

#define StackArray_Push(this, data, item)              \
	do {                                               \
		StackArray_At(this, (this)->len, item) = data; \
		(this)->len++;                                 \
	} while(0)
