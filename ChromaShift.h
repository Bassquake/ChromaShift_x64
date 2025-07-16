#include <windows.h>
//#include <string.h>
//#include <stdio.h>
#include "avisynth.h"


class ChromaShift : public GenericVideoFilter {
	int CShift, UShift, VShift, LShift, RShift, GShift, BShift;
	
public:
	ChromaShift(PClip _child, int _CShift, int _UShift, int _VShift, int _LShift,
		int _RShift, int _GShift, int _BShift, IScriptEnvironment* env);

	~ChromaShift();

	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
};