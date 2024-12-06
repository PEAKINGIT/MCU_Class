/**
 * @brief drawing function extend
 */
#include "gui_g9.h"

// x^n
int32_t my_pow(int32_t x,int32_t n){
	int32_t i,ret=1;
	for(i=0;i<n;i++){
		ret*=x;
	}
	return ret;
}
