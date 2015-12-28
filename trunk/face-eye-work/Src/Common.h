#ifndef __Common_h__
#define __Common_h__

#define SAFE_RELEASE(p)\
	{if (p){\
		p->Release();\
		p = NULL;}}

#define GET_SET_FUNC(varType, varName, funcName) \
protected:\
	varType varName;\
public:\
	inline varType get##funcName()const{return varName;}\
	inline void set##funcName(varType var){varName = var;}

#define GET_FUNC(varType, varName, funcName) \
protected:\
	varType varName;\
public:\
	inline varType get##funcName()const{return varName;}

#define SET_FUNC(varType, varName, funcName) \
protected:\
	varType varName;\
public:\
	inline void set##funcName(varType var){varName = var;}
#endif