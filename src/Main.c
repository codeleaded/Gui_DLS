#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/DLS.h"

DLS dls;
GChip* focusedChip = NULL;
int focusedPin = -1;

void Setup(AlxWindow* w){
    ResizeAlxFont(16,16);

    dls = DLS_Make_Std();
}
void Update(double ElapsedTime){
    if(Stroke(ALX_KEY_ENTER).PRESSED){
        DLS_Execute(&dls);
    }
    if(Stroke(ALX_KEY_A).PRESSED){
        focusedChip = NULL;
        focusedPin = -1;
        DLS_AddChip(&dls,Vec2_Divf(GetMouse(),GetWidth()),"AND");
    }
    if(Stroke(ALX_KEY_N).PRESSED){
        focusedChip = NULL;
        focusedPin = -1;
        DLS_AddChip(&dls,Vec2_Divf(GetMouse(),GetWidth()),"NOT");
    }

    if(Stroke(ALX_MOUSE_L).PRESSED){
        if(focusedPin>=0){
            Pin* fp = (Pin*)Vector_Get(&dls.pins,focusedPin);
            int itp = DLS_Pin_Find(&dls,Vec2_Divf(GetMouse(),GetWidth()));
            if(itp>=0){
                Pin* p = (Pin*)Vector_Get(&dls.pins,itp);
                if(fp->t==SIGNALTYPE_INPUT && p->t==SIGNALTYPE_OUTPUT) Vector_Push(&dls.wires,(Wire[]){ Wire_New(focusedPin,itp) });
                if(fp->t==SIGNALTYPE_OUTPUT && p->t==SIGNALTYPE_INPUT) Vector_Push(&dls.wires,(Wire[]){ Wire_New(itp,focusedPin) });
            }
            focusedPin = -1;
        }else{
            focusedChip = DLS_GChip_Find(&dls,Vec2_Divf(GetMouse(),GetWidth()));
            focusedPin = DLS_Pin_Find(&dls,Vec2_Divf(GetMouse(),GetWidth()));
        }
    }
    if(Stroke(ALX_MOUSE_L).DOWN){
        if(focusedChip){
            focusedChip->p = Vec2_Divf(GetMouse(),GetWidth());
        }
    }
    
    Clear(DARK_GRAY);

    DLS_Render(WINDOW_STD_ARGS,&dls);

    if(focusedPin>=0){
        Pin* p = (Pin*)Vector_Get(&dls.pins,focusedPin);
        Vec2 pos = Vec2_Mulf(Vec2_Add(p->p,(Vec2){ PIN_SIZE*0.5f,PIN_SIZE*0.5f }),GetWidth());
        RenderLine(pos,GetMouse(),RED,1.0f);
    }
}
void Delete(AlxWindow* w){
    focusedChip = NULL;
    focusedPin = -1;
    DLS_Free(&dls);
}

int main() {
    if(Create("Digital Logic Sim",1900,1000,1,1,Setup,Update,Delete)){
        Start();
    }
    return 0;
}
