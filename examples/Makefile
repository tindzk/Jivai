# Compile with Clang using the `config.h' configuration file and
# enable blocks.
CC = clang -include config.h

# Enable warnings.
CFLAGS = -W -Wall -pipe

# Include debugging symbols.
# This will add file names and line numbers to your backtraces.
CFLAGS += -g

# Disable optimizations.
CFLAGS += -O0

# The library uses a special C variant. Therefore these parameters
# are necessary.
CFLAGS += -std=gnu99 -fblocks

# Define the library path.
LIBPATH = ../src

# Only build these modules.
LIBFILES += $(LIBPATH)/{Kernel,Block,Typography,Path,Module,HTML,HTML/{Tokenizer,Entities,Tree},Unicode,Terminal/{Selection,Buffer,InputLine,Prompt},Terminal,Char,String,Backtrace,Integer,Memory,Socket,NetworkAddress,SocketConnection,Exception{,Manager},YAML,Tree,File,BufferedStream,FileStream,Server,Signal,Poll,Client,HTTP/{Header,Server,Query,Status,Method,Version},Hex,Logger,ClientListener}.c

# Enable human-readable backtraces (requires BFD).
LIBFILES += $(LIBPATH)/BFD.c -lbfd

# Set the library path.
LIBFILES += -I$(LIBPATH)

# Define all sample applications.
httpd:
	$(CC) -o build/httpd.bin $(CFLAGS) httpd.c $(LIBFILES)

BufferedStream:
	$(CC) -o build/BufferedStream.bin $(CFLAGS) BufferedStream.c $(LIBFILES)

SocketClient:
	$(CC) -o build/SocketClient.bin $(CFLAGS) SocketClient.c $(LIBFILES)

Unicode:
	$(CC) -o build/Unicode.bin $(CFLAGS) Unicode.c $(LIBFILES)

HTTPClient:
	$(CC) -o build/HTTPClient.bin $(CFLAGS) HTTPClient.c $(LIBFILES)

SocketServer:
	$(CC) -o build/SocketServer.bin $(CFLAGS) SocketServer.c $(LIBFILES)

YAML:
	$(CC) -o build/YAML.bin $(CFLAGS) YAML.c $(LIBFILES)

Integer:
	$(CC) -o build/Integer.bin $(CFLAGS) Integer.c $(LIBFILES)

Logger:
	$(CC) -o build/Logger.bin $(CFLAGS) Logger.c $(LIBFILES)

Terminal:
	$(CC) -o build/Terminal.bin $(CFLAGS) Terminal.c $(LIBFILES)

TerminalPrompt:
	$(CC) -o build/TerminalPrompt.bin $(CFLAGS) TerminalPrompt.c $(LIBFILES)

TerminalBuffer:
	$(CC) -o build/TerminalBuffer.bin $(CFLAGS) TerminalBuffer.c $(LIBFILES)

TerminalSelection:
	$(CC) -o build/TerminalSelection.bin $(CFLAGS) TerminalSelection.c $(LIBFILES)

TerminalInputLine:
	$(CC) -o build/TerminalInputLine.bin $(CFLAGS) TerminalInputLine.c $(LIBFILES)

HTMLTokenizer:
	$(CC) -o build/HTMLTokenizer.bin $(CFLAGS) HTMLTokenizer.c $(LIBFILES)

HTMLTree:
	$(CC) -o build/HTMLTree.bin $(CFLAGS) HTMLTree.c $(LIBFILES)

File:
	$(CC) -o build/File.bin $(CFLAGS) File.c $(LIBFILES)

Array:
	$(CC) -o build/Array.bin $(CFLAGS) Array.c $(LIBFILES)

StackArray:
	$(CC) -o build/StackArray.bin $(CFLAGS) StackArray.c $(LIBFILES)

Exception:
	$(CC) -o build/Exception.bin $(CFLAGS) Exception.c $(LIBFILES)

String:
	$(CC) -o build/String.bin $(CFLAGS) String.c $(LIBFILES)

PHP:
	$(CC) -o build/PHP.bin $(CFLAGS) PHP.c $(LIBFILES)

Typography:
	$(CC) -o build/Typography.bin $(CFLAGS) Typography.c $(LIBFILES)

# Delete all compiled binaries with a `make clean'.
clean:
	rm build/*.bin
