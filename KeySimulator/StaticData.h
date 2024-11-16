#ifndef STATICDATA_H
#define STATICDATA_H

#include <atlstr.h>

struct StaticData {
	static inline bool checkBox_Loop_Clicked = false;
	static inline bool btn_Start_Clicked = false;
	static inline bool key_detected = false;
	static inline CString* textBox_Input_p = nullptr;
	static inline CString activationKey = _T("");
	static inline float delay = 0;
};

#endif // !STATICDATA_H