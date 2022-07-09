#define SET_EXPR(lhs, expr)                   *(uint32_t*)&lhs = (uint32_t)(expr);
//#define SET_VAR(lhs, rhs)                     *(int8_t**)&lhs = (int8_t*)&rhs;
#define SET_VAR(lhs, rhs)                     *(int8_t**)&lhs = (int8_t*)rhs;
//#define SET_VAR_WITH_OFFSET(lhs, rhs, offset) *(int8_t**)&lhs = (int8_t*)&rhs + (offset);
#define SET_VAR_WITH_OFFSET(lhs, rhs, offset) *(int8_t**)&lhs = (int8_t*)rhs + (offset);

//С помощью этой вставки (работает аналогично вызовам из делфи) вызываем методы TList,
//поскольку C++ напрямую вызывать функцию из делфи не может в силу различий регистра передачи аргументов
#include <cstdint>

inline uint32_t ASM_CALL(auto funcptr, auto arg0, auto arg1) {
    uint32_t register result;
    asm volatile ("call *%3" : "=a"(result) : "d"((void*)arg1), "a"((void*)arg0), "b"((void*)funcptr));
    return result;
}



VMT_TList *vmt_TList = nullptr;

void*     (__fastcall *TObject_Create)    (__cls* cls, int8_t flag);

TList*    (__fastcall *TList_Create)      (VMT_TList* cls, int8_t flag);
int       (__fastcall *TList_Destroy)     (TList* list, int8_t flag);
void      (__fastcall *TList_Add)         (TList* list, uint32_t value);
void      (__fastcall *TList_Clear)       (TList* list);
void      (__fastcall *TList_Delete)      (TList* list, int32_t index);
void      (__fastcall *TList_Error)       (TList* list, int32_t, int32_t);
void      (__fastcall *TList_Error_0)     (TList* list, int32_t, int32_t);
void      (__fastcall *TList_Exchange)    (TList* list, int32_t index1, int32_t index2);
void      (__fastcall *TList_Expand)      (TList* list);
uint32_t  (__fastcall *TList_First)       (TList* list);
uint32_t  (__fastcall *TList_Get)         (TList* list, int32_t index);
void      (__fastcall *TList_Grow)        (TList* list);
int32_t   (__fastcall *TList_IndexOf)     (TList* list, uint32_t value);
void      (__fastcall *TList_Insert)      (TList* list, int32_t index, uint32_t value);
uint32_t  (__fastcall *TList_Last)        (TList* list);
void      (__fastcall *TList_Put)         (TList* list, int32_t index, uint32_t value);
void      (__fastcall *TList_Remove)      (TList* list, uint32_t value);
void      (__fastcall *TList_SetCapacity) (TList* list, int32_t new_capacity);
void      (__fastcall *TList_SetCount)    (TList* list, int32_t new_count);
void      (__fastcall *TList_Notify)      (TList* list);


void TList_methods_init(TList* list) {
    vmt_TList = (VMT_TList*)list->cls;

    SET_EXPR           (TObject_Create   , 0x403FA8);

    *(void**)&TList_Destroy = *((void**)vmt_TList - 1);
    SET_VAR            (TList_Create     , TObject_Create);               // у TList нет своего конструктора, он дефолтный
    SET_VAR_WITH_OFFSET(TList_Add        , TList_Destroy, 0xA9C - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Clear      , TList_Destroy, 0xAD0 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Delete     , TList_Destroy, 0xAE8 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Error      , TList_Destroy, 0xB4C - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Error_0    , TList_Destroy, 0xB84 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Exchange   , TList_Destroy, 0xBD8 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Expand     , TList_Destroy, 0xC28 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_First      , TList_Destroy, 0xC40 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Get        , TList_Destroy, 0xC48 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Grow       , TList_Destroy, 0xC70 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_IndexOf    , TList_Destroy, 0xCA4 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Insert     , TList_Destroy, 0xCC4 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Last       , TList_Destroy, 0xD30 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Put        , TList_Destroy, 0xD3C - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Remove     , TList_Destroy, 0xD8C - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_SetCapacity, TList_Destroy, 0xDAC - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_SetCount   , TList_Destroy, 0xDE8 - 0xA7C);
    SET_VAR_WITH_OFFSET(TList_Notify     , TList_Destroy, 0xE58 - 0xA7C);
}