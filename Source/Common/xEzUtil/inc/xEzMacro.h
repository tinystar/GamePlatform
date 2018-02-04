/************************************************************************/
/* All rights reserved.
/* You can't use, copy or modify the file without the permission of the
/* author. Otherwise I will be held accountable under the law.
/*
/* Author: xiaoxin
/* Contact: 765392700(QQ)
/* Date: 2016/12/2
/************************************************************************/
#ifndef __XEZ_MACRO_H__
#define __XEZ_MACRO_H__

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else	// __cplusplus
#define NULL ((void*)0)
#endif	// __cplusplus
#endif

#ifndef MAX_PATH
#define MAX_PATH			260
#endif


#define EZMIN(a, b)			(((a) < (b)) ? (a) : (b))
#define EZMAX(a, b)			(((a) > (b)) ? (a) : (b))


#define EZ_NAMESPACE_BEGIN(name)	namespace name {
#define EZ_NAMESPACE_END	}


#define EZ_FORBID_CLASS_COPY(CLASS_NAME)			\
	private:										\
		CLASS_NAME(const CLASS_NAME&);				\
		CLASS_NAME& operator= (const CLASS_NAME&);


#ifndef EZ_SAFE_DELETE
#define EZ_SAFE_DELETE(pointer)			\
	do{									\
		if(pointer)						\
		{								\
			delete pointer;				\
			pointer = NULL;				\
		}								\
	}while(0)
#endif

#ifndef EZ_SAFE_DELETE_ARRAY
#define EZ_SAFE_DELETE_ARRAY(pointer)	\
	do{									\
		if(pointer)						\
		{								\
			delete[] pointer;			\
			pointer = NULL;				\
		}								\
	}while(0)
#endif

#ifndef EZ_SAFE_FREE
#define EZ_SAFE_FREE(pointer)			\
	do{									\
		if(pointer)						\
		{								\
			free(pointer);				\
			pointer = NULL;				\
		}								\
	}while(0)
#endif


#define EzCountOf(array)	(sizeof(array)/sizeof(array[0]))

#endif // __XEZ_MACRO_H__