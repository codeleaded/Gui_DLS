/*

OR 2 -> 1:
- NOT \ 
       AND - NOT
- NOT /


2 => f => 1:
f:= %0 = !(!$0 & !$1)

Functiontree:
n -> function(*ins,*outs)
t -> functionwrapper{ins,outs,functionwrapper}:

STD:
    AND(2,1),NOT(1,1)

FUNCS:
    AND,NOT,NOT,NOT

INPUTS:
    LIST: (Pins itself)
        VALUE,
        LIST: (FUNCS index,PIN index) (Pin IN to TARGET)

Counting how many inputs were already set:
    if ins set == ins there -> on execute STACK
    else                    -> on waiting STACK

*/

#include "/home/codeleaded/System/Static/Library/WindowEngine.h"
#include "/home/codeleaded/System/Static/Library/DLS.h"


DLS dls;
Chip_Pin_Iter selected;

void Setup(AlxWindow* w){
    ResizeAlxFont(16,16);
    dls = DLS_New_Std();
    selected = Chip_Pin_Iter_Null();
}
void Update(AlxWindow* w){
    TransformedView_HandlePanZoom(&dls.tv,w->Strokes,GetMouse());
    const Vec2 m = TransformedView_ScreenWorldPos(&dls.tv,GetMouse());
    
    if(Stroke(ALX_KEY_ENTER).PRESSED){
        DLS_Execute(&dls);
    }

    if(Stroke(ALX_KEY_1).PRESSED){
        selected = Chip_Pin_Iter_Null();
        DLS_AddChip(&dls,m,"IN");
    }
    if(Stroke(ALX_KEY_2).PRESSED){
        selected = Chip_Pin_Iter_Null();
        DLS_AddChip(&dls,m,"OUT");
    }
    if(Stroke(ALX_KEY_3).PRESSED){
        selected = Chip_Pin_Iter_Null();
        DLS_AddChip(&dls,m,"AND");
    }
    if(Stroke(ALX_KEY_4).PRESSED){
        selected = Chip_Pin_Iter_Null();
        DLS_AddChip(&dls,m,"NOT");
    }
    if(Stroke(ALX_KEY_5).PRESSED){
        selected = Chip_Pin_Iter_Null();
        DLS_AddChip(&dls,m,"NOT8");
    }

    
    if(Stroke(ALX_MOUSE_L).PRESSED){
        if(selected.cit != CHIP_ITER_NONE && selected.pit != PIN_ITER_NONE){
            Chip_Pin_Iter other = DLS_FindPin(&dls,m);
            
            if(other.cit != CHIP_ITER_NONE && other.pit != PIN_ITER_NONE){
                Chip* c0 = (Chip*)Vector_Get(&dls.cd.chips,selected.cit);
                Chip* c1 = (Chip*)Vector_Get(&dls.cd.chips,other.cit);
                Chip_Def_Connect(&dls.cd,selected.cit,selected.pit,other.cit,other.pit);
            }
            selected = Chip_Pin_Iter_Null();
        }else{
            selected.cit = DLS_FindChip(&dls,m);
            selected.pit = PIN_ITER_NONE;

            if(selected.cit == CHIP_ITER_NONE){
                selected = DLS_FindPin(&dls,m);
            }
        }
        

        // if(focusedPin>=0){
        //     Pin* fp = (Pin*)PVector_Get(&dls.pins,focusedPin);
        //     int itp = DLS_Pin_FindI(&dls,GetMouse());
        //     if(itp>=0){
        //         Pin* p = (Pin*)PVector_Get(&dls.pins,itp);
        //         if(fp->t==CIRCUIT_SIGNALTYPE_INPUT && p->t==CIRCUIT_SIGNALTYPE_OUTPUT)
        //             Vector_Push(&dls.wires,(Wire[]){ Wire_New(focusedPin,itp) });
        //         if(fp->t==CIRCUIT_SIGNALTYPE_OUTPUT && p->t==CIRCUIT_SIGNALTYPE_INPUT)
        //             Vector_Push(&dls.wires,(Wire[]){ Wire_New(itp,focusedPin) });
        //     }
        //     focusedPin = -1;
        // }else{
        //     focusedChip = DLS_GChip_Find(&dls,GetMouse());
        //     focusedPin = DLS_Pin_FindI(&dls,GetMouse());
        // }

        
    }
    if(Stroke(ALX_MOUSE_L).DOWN){
        if(selected.cit != CHIP_ITER_NONE && selected.pit == PIN_ITER_NONE){
            Chip* c = (Chip*)Vector_Get(&dls.cd.chips,selected.cit);
            c->p = m;
        }
    }
    if(Stroke(ALX_KEY_REMOVE).PRESSED){
        if(selected.cit != CHIP_ITER_NONE && selected.pit == PIN_ITER_NONE){
            DLS_RemoveChip(&dls,selected.cit);
            selected = Chip_Pin_Iter_Null();
        }
    }

    // if(Stroke(ALX_MOUSE_R).PRESSED){
    //     GChip* fc = DLS_GChip_Find(&dls,GetMouse());
    //     if(fc){
    //         if(CStr_Cmp(fc->cw->name,"IN")){
    //             Pin* p = *(Pin**)Vector_Get(&fc->input,0);
    //             if(p->s == CIRCUIT_SIGNAL_HIGH) p->s = CIRCUIT_SIGNAL_LOW;
    //             else                            p->s = CIRCUIT_SIGNAL_HIGH;
    //         }
    //     }
    // }
    // if(Stroke(ALX_KEY_ENTER).PRESSED){
    //     DLS_Execute(&dls);
    // }

    Clear(BLACK);

    DLS_Render(&dls,WINDOW_STD_ARGS);

    if(selected.cit != CHIP_ITER_NONE && selected.pit != PIN_ITER_NONE){
        Chip* c = (Chip*)Vector_Get(&dls.cd.chips,selected.cit);
        //Pin* p = selected.pit < c->ins.size ? (Pin*)Vector_Get(&c->ins,selected.pit) : (Pin*)Vector_Get(&c->outs,selected.pit - c->ins.size);
        
        const Pin_Iter pit = selected.pit < c->ins.size ? selected.pit : selected.pit - c->ins.size;
        const float poff = selected.pit < c->ins.size ? -0.33f : 0.33f + 2.0f;

        const Vec2 psp = TransformedView_WorldScreenPos(&dls.tv,(Vec2){ c->p.x + poff,c->p.y + 0.5f + pit });
        RenderLine(psp,GetMouse(),LIGHT_RED,1.0f);
    }
}
void Delete(AlxWindow* w){
    DLS_Free(&dls);
}

int main() {
    if(Create("Digital Logic Sim",1900,1000,1,1,Setup,Update,Delete)){
        Start();
    }
    return 0;
}

/*
#include "/home/codeleaded/System/Static/Library/WindowEngine.h"
#include "/home/codeleaded/System/Static/Library/DLS.h"

DLS dls;
GChip* focusedChip = NULL;
int focusedPin = -1;

void Setup(AlxWindow* w){
    ResizeAlxFont(16,16);
    dls = DLS_New_Std();
}
void Update(AlxWindow* w){
    TransformedView_HandlePanZoom(&dls.tv,w->Strokes,GetMouse());
    
    if(Stroke(ALX_KEY_ENTER).PRESSED){
        DLS_Execute(&dls);
    }

    if(Stroke(ALX_KEY_1).PRESSED){
        focusedChip = NULL;
        focusedPin = -1;
        DLS_AddChip(&dls,GetMouse(),"IN");
    }
    if(Stroke(ALX_KEY_2).PRESSED){
        focusedChip = NULL;
        focusedPin = -1;
        DLS_AddChip(&dls,GetMouse(),"OUT");
    }
    if(Stroke(ALX_KEY_3).PRESSED){
        focusedChip = NULL;
        focusedPin = -1;
        DLS_AddChip(&dls,GetMouse(),"AND");
    }
    if(Stroke(ALX_KEY_4).PRESSED){
        focusedChip = NULL;
        focusedPin = -1;
        DLS_AddChip(&dls,GetMouse(),"NOT");
    }

    if(Stroke(ALX_MOUSE_L).PRESSED){
        if(focusedPin>=0){
            Pin* fp = (Pin*)PVector_Get(&dls.pins,focusedPin);
            int itp = DLS_Pin_FindI(&dls,GetMouse());
            if(itp>=0){
                Pin* p = (Pin*)PVector_Get(&dls.pins,itp);
                if(fp->t==CIRCUIT_SIGNALTYPE_INPUT && p->t==CIRCUIT_SIGNALTYPE_OUTPUT)
                    Vector_Push(&dls.wires,(Wire[]){ Wire_New(focusedPin,itp) });
                if(fp->t==CIRCUIT_SIGNALTYPE_OUTPUT && p->t==CIRCUIT_SIGNALTYPE_INPUT)
                    Vector_Push(&dls.wires,(Wire[]){ Wire_New(itp,focusedPin) });
            }
            focusedPin = -1;
        }else{
            focusedChip = DLS_GChip_Find(&dls,GetMouse());
            focusedPin = DLS_Pin_FindI(&dls,GetMouse());
        }
    }
    if(Stroke(ALX_MOUSE_L).DOWN){
        if(focusedChip){
            const Vec2 wm = TransformedView_ScreenWorldPos(&dls.tv,GetMouse());
            focusedChip->p = wm;
        }
    }
    if(Stroke(ALX_KEY_REMOVE).PRESSED){
        if(focusedChip){
            const int iter = DLS_GChip_Iter(&dls,focusedChip);
            DLS_RemoveChip(&dls,iter);
        }
    }

    if(Stroke(ALX_MOUSE_L).PRESSED){
        GChip* fc = DLS_GChip_Find(&dls,GetMouse());
        if(fc){
            if(CStr_Cmp(fc->cw->name,"IN")){
                Pin* p = (Pin*)Vector_Get(&fc->input,0);
                if(p->s == CIRCUIT_SIGNAL_HIGH) p->s = CIRCUIT_SIGNAL_LOW;
                else                            p->s = CIRCUIT_SIGNAL_HIGH;
            }
        }
    }
    if(Stroke(ALX_KEY_ENTER).PRESSED){
        DLS_Execute(&dls);
    }
    
    Clear(DARK_GRAY);

    DLS_Render(WINDOW_STD_ARGS,&dls);

    if(focusedPin>=0){
        Pin* p = (Pin*)PVector_Get(&dls.pins,focusedPin);
        const Vec2 pos = Vec2_Add(p->p,(Vec2){ PIN_SIZE*0.5f,PIN_SIZE*0.5f });
        const Vec2 sp = TransformedView_WorldScreenPos(&dls.tv,pos);
        RenderLine(sp,GetMouse(),RED,1.0f);
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

*/