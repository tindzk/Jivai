#import "../String.h"
#import "../Folder.h"

#define self Folder_Expander

Callback(ref(OnMatch), void, RdString path, RdString name);

class {
	ref(OnMatch) onMatch;
};

rsdef(self, new, ref(OnMatch) onMatch);
def(bool, scan, RdString pattern);

#undef self
