#include "gui_g9.h"

#include "gui_g9.h"

I_EXIT_STATE Load_InterFace(InterFaceObj *interFacePtr) {
    if (interFacePtr->init == NULL) return I_LOAD_FAIL;
    if (interFacePtr->exit == NULL) return I_EXIT_FAIL;
    if (interFacePtr->exit_judge == NULL) return I_JUDGE_FAIL;

    interFacePtr->init(interFacePtr);
    while (1) {
        if (interFacePtr->looping != NULL) {
			/* looping the interface */
            interFacePtr->looping(interFacePtr);
        }
        if (interFacePtr->exit_judge(interFacePtr)) break;
    }
    interFacePtr->exit(interFacePtr);
    return I_EXIT_WELL;
}

