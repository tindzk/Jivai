#import <Main.h>
#import <System.h>
#import <Memory.h>
#import <Exception.h>

#define self Application

def(bool, Run) {
	unsigned int *ptr[2048];

	for (size_t i = 1; i < nElems(ptr); i++) {
		ptr[i] = Memory_New(i * sizeof(unsigned int));

		fwd(j, i) {
			ptr[i][j] = i + j;
		}
	}

	for (size_t i = 1; i < nElems(ptr); i++) {
		fwd(j, i) {
			assert(ptr[i][j] == i + j);
		}
	}

	for (size_t i = 1; i < nElems(ptr); i++) {
		Memory_Destroy(ptr[i]);
	}

	return true;
}
