#import "../YAML.h"
#import "../String.h"
#import "../StringReader.h"

#define self YAML_Parser

exc(ColonMissing)
exc(InvalidIndention)
exc(SpaceIndention)

class {
	size_t depth;
	size_t prevDepth;

	StringReader reader;
	YAML_OnToken onToken;
};

rsdef(self, New, YAML_OnToken onToken);
def(void, Destroy);
def(void, Process, RdString s);

#undef self
