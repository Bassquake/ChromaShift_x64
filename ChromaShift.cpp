/*
	ChromaShift plugin for Avisynth -- lets users shift the chroma information
	around indenpendatly of the lunimance information

	Copyright (C) 2002 Simon Walters
  
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation.
	 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
		
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	
	The author can be contacted at:
	Simon Walters
	siwalters@hotmail.com
			 
	V1.0 10th Sep 2002 - 1st release
	V1.1 Allow variable amount of shift and independant U V shifts
	V1.2 Allow negative values and a global vertical shift value L
	V2.5 Amended to work in Avisynth2.5
	V2.6 Allowed horizontal shifting of R, G and B separately (if source is RGB32) (Wilbert Dijkhof)
	V2.6a minor syntax error corrected in YUY2 colourspace checking (D replaced with C) SW 3rdNov2003
	V2.7 add in YV12 handling.
*/

#include "ChromaShift.h"

ChromaShift::ChromaShift(PClip _child, int _CShift, int _UShift, int _VShift, int _LShift,
						 int _RShift, int _GShift, int _BShift, IScriptEnvironment* env) : 
	GenericVideoFilter(_child), CShift(_CShift), UShift(_UShift), VShift(_VShift), LShift(_LShift),
	RShift(_RShift), GShift(_GShift), BShift(_BShift) {
	// parameter checking goes in here
	if (!(vi.IsYUY2()||vi.IsRGB32()||vi.IsYV12()))
		env->ThrowError("ChromaShift: input to filter must be in YUY2, YV12 or RGB32");
	if ((CShift % 2)||(UShift % 2)||(VShift % 2))
		env->ThrowError("ChromaShift: C. U and V values must be even");
	if (!(vi.IsRGB32()) && ((RShift!=0)||(GShift!=0)||(BShift!=0)))
		env->ThrowError("Your source is not RGB32: you are only allowed to change the C, U, V or L values");
	if (vi.IsRGB32() && ((LShift!=0)||(UShift!=0)||(VShift!=0)||(CShift!=0)))
		env->ThrowError("Your source is RGB32: you are only allowed to change the R, G or B values");
	if (((vi.IsYV12())&&(LShift % 2)))
		env->ThrowError("Your source is YV12: L value must be even");
	}

ChromaShift::~ChromaShift() {
}
	
PVideoFrame __stdcall ChromaShift::GetFrame(int n, IScriptEnvironment* env) {

	PVideoFrame frame = child->GetFrame(n, env);

	env->MakeWritable(&frame);

	unsigned char* framep = frame->GetWritePtr();
	const int frame_pitch = frame->GetPitch();
	const int frame_row_size = frame->GetRowSize();
	const int frame_height = frame->GetHeight();

	if (vi.IsYV12()) {
		const int frame_pitchUV = frame->GetPitch(PLANAR_U);	// The pitch,height and width information
		const int frame_row_sizeUV = frame->GetRowSize(PLANAR_U);	// is guaranted to be the same for both
		const int frame_heightUV = frame->GetHeight(PLANAR_U);	// the U and V planes so we only the U

		// CShift (Shift both U & V)
		framep = frame->GetWritePtr(PLANAR_U);
		if (CShift > 0) {
			for (int y = 0; y < frame_heightUV; y++) {
				for (int x = (frame_row_sizeUV - 1); x >= (CShift>>1); x-=1) {
					framep[x] = framep[x-(CShift>>1)];
				}
				framep += frame_pitchUV;
			}
		}
		framep = frame->GetWritePtr(PLANAR_V);
		if (CShift > 0) {
			for (int y = 0; y < frame_heightUV; y++) {
				for (int x = (frame_row_sizeUV - 1); x >= (CShift>>1); x-=1) {
					framep[x] = framep[x-(CShift>>1)];
				}
				framep += frame_pitchUV;
			}
		}

		framep = frame->GetWritePtr(PLANAR_U);
		if (CShift < 0) {
			for (int y = 0; y < frame_heightUV; y++) {
				for (int x = 0; x < (frame_row_sizeUV + (CShift>>1)); x+=1) {
					framep[x] = framep[x-(CShift>>1)];
				}
				framep += frame_pitchUV;
			}
		}
		framep = frame->GetWritePtr(PLANAR_V);
		if (CShift < 0) {
			for (int y = 0; y < frame_heightUV; y++) {
				for (int x = 0; x < (frame_row_sizeUV + (CShift>>1)); x+=1) {
					framep[x] = framep[x-(CShift>>1)];
				}
				framep += frame_pitchUV;
			}
		}

		// UShift
		framep = frame->GetWritePtr(PLANAR_U);
		if (UShift > 0) {
			for (int y = 0; y < frame_heightUV; y++) {
				for (int x = (frame_row_sizeUV - 1); x >= (UShift>>1); x-=1) {
					framep[x] = framep[x-(UShift>>1)];
				}
				framep += frame_pitchUV;
			}
		}
		framep = frame->GetWritePtr(PLANAR_U);
		if (UShift < 0) {
			for (int y = 0; y < frame_heightUV; y++) {
				for (int x = 0; x < (frame_row_sizeUV + (UShift>>1)); x+=1) {
					framep[x] = framep[x-(UShift>>1)];
				}
				framep += frame_pitchUV;
			}
		}

		// VShift

		framep = frame->GetWritePtr(PLANAR_V);
		if (VShift > 0) {
			for (int y = 0; y < frame_heightUV; y++) {
				for (int x = (frame_row_sizeUV - 1); x >= (VShift>>1); x-=1) {
					framep[x] = framep[x-(VShift>>1)];
				}
				framep += frame_pitchUV;
			}
		}


		framep = frame->GetWritePtr(PLANAR_V);
		if (VShift < 0) {
			for (int y = 0; y < frame_heightUV; y++) {
				for (int x = 0; x < (frame_row_sizeUV + (VShift>>1)); x+=1) {
					framep[x] = framep[x-(VShift>>1)];
				}
				framep += frame_pitchUV;
			}
		}

		// LShift
		if (LShift > 0) {
			framep = frame->GetWritePtr(PLANAR_U);
			framep = framep + (frame_heightUV*frame_pitchUV) - frame_pitchUV;
			for (int y = (frame_heightUV - 1); y >= (LShift>>1); y--) {
				for (int x = 0; x < frame_row_sizeUV; x+=1) {
					framep[x] = framep[x-(frame_pitchUV*(LShift>>1))];
				}
				framep -= frame_pitchUV;
			}
			framep = frame->GetWritePtr(PLANAR_V);
			framep = framep + (frame_heightUV*frame_pitchUV) - frame_pitchUV;
			for (int y = (frame_heightUV - 1); y >= (LShift>>1); y--) {
				for (int x = 0; x < frame_row_sizeUV; x+=1) {
					framep[x] = framep[x-(frame_pitchUV*(LShift>>1))];
				}
				framep -= frame_pitchUV;
			}
		}

		if (LShift < 0) {
			framep = frame->GetWritePtr(PLANAR_U);
			for (int y = 0; y < (frame_heightUV+ (LShift>>1)); y++) {
				for (int x = 0; x < frame_row_sizeUV; x+=1) {
					framep[x] = framep[x-(frame_pitchUV*(LShift>>1))];
				}
				framep += frame_pitchUV;
			}
			framep = frame->GetWritePtr(PLANAR_V);
			for (int y = 0; y < (frame_heightUV+ (LShift>>1)); y++) {
				for (int x = 0; x < frame_row_sizeUV; x+=1) {
					framep[x] = framep[x-(frame_pitchUV*(LShift>>1))];
				}
				framep += frame_pitchUV;
			}
		}
	}


	if (vi.IsYUY2()) {

		// CShift (Shift both U & V)
		framep = frame->GetWritePtr();
		if (CShift > 0) {
			for (int y = 0; y < frame_height; y++) {
				for (int x = (frame_row_size - 4); x >= (CShift*2); x-=4) {
					framep[x+1] = framep[x+1-(CShift*2)];
					framep[x+3] = framep[x+3-(CShift*2)];
				}
				framep += frame_pitch;
			}
		}
		if (CShift < 0) {
			for (int y = 0; y < frame_height; y++) {
				for (int x = 0; x < (frame_row_size + (CShift*2)); x+=4) {
					framep[x+1] = framep[x+1-(CShift*2)];
					framep[x+3] = framep[x+3-(CShift*2)];
				}
				framep += frame_pitch;
			}
		}
	
		// UShift
		framep = frame->GetWritePtr();
		if (UShift > 0) {
			for (int y = 0; y < frame_height; y++) {
				for (int x = (frame_row_size - 4); x >= (UShift*2); x-=4) {
					framep[x+1] = framep[x+1-(UShift*2)];
				}
				framep += frame_pitch;
			}
		}
		if (UShift < 0) {
			for (int y = 0; y < frame_height; y++) {
				for (int x = 0; x < (frame_row_size + (UShift*2)); x+=4) {
					framep[x+1] = framep[x+1-(UShift*2)];
				}
				framep += frame_pitch;
			}
		}

		// VShift
		framep = frame->GetWritePtr();
		if (VShift > 0) {
			for (int y = 0; y < frame_height; y++) {
				for (int x = (frame_row_size - 4); x >= (VShift*2); x-=4) {
					framep[x+3] = framep[x+3-(VShift*2)];
				}
				framep += frame_pitch;;
			}
		}
		if (VShift < 0) {
			for (int y = 0; y < frame_height; y++) {
				for (int x = 0; x < (frame_row_size + (VShift*2)); x+=4) {
					framep[x+3] = framep[x+3-(VShift*2)];
				}
				framep += frame_pitch;
			}
		}

		// LShift
		if (LShift > 0) {
			framep = frame->GetWritePtr();
			framep = framep + (frame_height*frame_pitch) - frame_pitch;
			for (int y = (frame_height - 1); y >= LShift; y--) {
				for (int x = 0; x < frame_row_size; x+=4) {
					framep[x+1] = framep[x+1-(frame_pitch*LShift)];
					framep[x+3] = framep[x+3-(frame_pitch*LShift)];
				}
				framep -= frame_pitch;
			}
		}
		if (LShift < 0) {
			framep = frame->GetWritePtr();
			for (int y = 0; y < (frame_height+ LShift); y++) {
				for (int x = 0; x < frame_row_size; x+=4) {
					framep[x+1] = framep[x+1-(frame_pitch*LShift)];
					framep[x+3] = framep[x+3-(frame_pitch*LShift)];
				}
				framep += frame_pitch;
			}
		}
	}

	if (vi.IsRGB32()) {
		
		// BShift
		if (BShift > 0) {
			framep = frame->GetWritePtr();
			for (int y = 0; y < frame_height; y++) {	// Loop from bottom line to top line.
				for (int x = (frame_row_size - 4); x >= 0; x-=4) {	// 1 pixel = 4 bytes (fourth byte = red)
					if (x >= (BShift*4)) framep[x] = framep[x-(BShift*4)]; else framep[x] = 0;
				}
				framep += frame_pitch;
			}
		}
		if (BShift < 0) {
			framep = frame->GetWritePtr();
			for (int y = 0; y < frame_height; y++) {
				for (int x = 0; x <= frame_row_size; x+=4) {
					if (x < (frame_row_size + BShift*4)) framep[x] = framep[x-(BShift*4)]; else framep[x] = 0;
				}
				framep += frame_pitch;
			}
		}

		// GShift
		if (GShift > 0) {
			framep = frame->GetWritePtr();
			for (int y = 0; y < frame_height; y++) {	// Loop from bottom line to top line.
				for (int x = (frame_row_size - 4); x >= 0; x-=4) {	// 1 pixel = 4 bytes (fourth byte = red)
					if (x >= GShift*4) framep[x+1] = framep[x+1-(GShift*4)]; else framep[x+1] = 0;
				}
				framep += frame_pitch;
			}
		}
		if (GShift < 0) {
			framep = frame->GetWritePtr();
			for (int y = 0; y < frame_height; y++) {
				for (int x = 0; x <= frame_row_size; x+=4) {
					if (x < (frame_row_size + GShift*4)) framep[x+1] = framep[x+1-(GShift*4)]; else framep[x+1] = 0;
				}
				framep += frame_pitch;
			}
		}

		if (RShift > 0) {
			framep = frame->GetWritePtr();
			for (int y = 0; y < frame_height; y++) {	// Loop from bottom line to top line.
				for (int x = (frame_row_size - 4); x >= 0; x-=4) {	// 1 pixel = 4 bytes (fourth byte = red)
					if (x >= RShift*4) framep[x+2] = framep[x+2-(RShift*4)]; else framep[x+2] = 0;
				}
				framep += frame_pitch;
			}
		}

		if (RShift < 0) {
			framep = frame->GetWritePtr();
			for (int y = 0; y < frame_height; y++) {
				for (int x = 0; x <= frame_row_size; x+=4) {
					if (x <= (frame_row_size + RShift*4)) framep[x+2] = framep[x+2-(RShift*4)]; else framep[x+2] = 0;
				}
				framep += frame_pitch;
			}
		}
	};

	return frame;
}


AVSValue __cdecl Create_ChromaShift(AVSValue args, void* user_data, IScriptEnvironment* env) {
    return new ChromaShift(args[0].AsClip(),
			args[1].AsInt(0),
			args[2].AsInt(0),
			args[3].AsInt(0),
			args[4].AsInt(0),
			args[5].AsInt(0),
			args[6].AsInt(0),
			args[7].AsInt(0),
			env);
}


extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit2(IScriptEnvironment* env) {
	env->AddFunction("ChromaShift", "c[C]i[U]i[V]i[L]i[R]i[G]i[B]i", Create_ChromaShift, 0);
    return "`ChromaShift' ChromaShift plugin";
}

