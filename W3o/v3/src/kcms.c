#include "kcms.h"

int KCMS_Enabled       = 0;
int KCMS_Return_Format = JYCC;

void CheckKCMS() {

	/*replace this with the real lib call when we get it*/
	KCMS_Enabled = 0;

	/*need a callback which will eventually decide this, but base the
	  default on whether KCMS is present or not*/
	if (KCMS_Enabled) {
		KCMS_Return_Format=JYCC;
	}
	else {
		KCMS_Return_Format=JPEG;
	}

	return;
}
