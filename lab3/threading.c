#include <threading.h>

void t_init()
{
        for(int i = 0;i<NUM_CTX;i++){
                contexts[i].state = INVALID;
        }
        current_context_idx = NUM_CTX;
}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2)
{
        uint8_t index = 255;
        for(uint8_t i = 0;i<NUM_CTX;i++){
                if(contexts[i].state == INVALID){
                       index = i;
                       break;
                }
        }

        if(index == 255)
                return 1; // couldnt find a free context
        else{
                getcontext(&contexts[index].context);
                contexts[index].context.uc_stack.ss_sp = malloc(STK_SZ);
                contexts[index].context.uc_stack.ss_size = STK_SZ;
                contexts[index].context.uc_link = NULL;
                contexts[index].context.uc_stack.ss_flags = 0;

                makecontext(&contexts[index].context, (ctx_ptr)foo, 2, arg1, arg2);

                contexts[index].state = VALID;
                return 0;
        }
        return 1; // fail
}

int32_t t_yield()
{
        uint8_t next_context = current_context_idx;
        uint8_t next_valid;
        int valid = 0;
        int found = 0;
        for(uint8_t i = 0;i<NUM_CTX;i++){
                next_context = (current_context_idx+i) % NUM_CTX;
                if(contexts[next_context].state == VALID){
                        if(found == 0){
                                found = 1;
                                next_valid = next_context;
                        }
                        valid++;
                }
        }

        if(next_context==current_context_idx){
                return 1; // no valid context found
        }
        int last_context = current_context_idx;
        current_context_idx = next_valid;
        swapcontext(&contexts[last_context].context, &contexts[next_valid].context);
        return valid;
}

void t_finish()
{
        free(contexts[current_context_idx].context.uc_stack.ss_sp);
        memset(&contexts[current_context_idx].context, 0, sizeof(ucontext_t));
        contexts[current_context_idx].state = DONE;
        t_yield();
}
