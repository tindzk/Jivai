#ifndef VAR_ARG_H
#define VAR_ARG_H

typedef __builtin_va_list VarArg;

#define VarArg_Start(v, l) \
	__builtin_va_start((v), (l))

#define VarArg_End \
	__builtin_va_end

#define VarArg_Get \
	__builtin_va_arg

#define VarArg_Copy(d, s) \
	__builtin_va_copy((d), (s))

#endif
