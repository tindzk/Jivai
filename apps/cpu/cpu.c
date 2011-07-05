#import <CPU.h>
#import <Main.h>
#import <String.h>

#define self Application

def(bool, Run) {
	printf("Number of cores: %i\n", CPU_getCores());
	return true;
}
