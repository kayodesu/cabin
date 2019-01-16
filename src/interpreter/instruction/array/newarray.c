/*
 * Author: Jia Yang
 */


#include "../../../util/bytecode_reader.h"
#include "../../../loader/classloader.h"
#include "../../../rtda/thread/frame.h"
#include "../../../rtda/heap/arrobj.h"

/*
 * 创建一维基本类型数组。包括 boolean[], byte[], char[], short[], int[], long[], float[] 和 double[] 8种。
 * 显然基本类型数组肯定都是一维数组，
 * 如果引用类型数组的元素也是数组，那么它就是多维数组。
 */
void newarray(struct frame *frame)
{
    jint arr_len = frame_stack_popi(frame);  //os_popi(frame->operand_stack);
    if (arr_len < 0) {
        thread_throw_negative_array_size_exception(frame->thread, arr_len);
        return;
    }

    // todo arrLen == 0 的情况

    /*
     * AT_BOOLEAN = 4
     * AT_CHAR    = 5
     * AT_FLOAT   = 6
     * AT_DOUBLE  = 7
     * AT_BYTE    = 8
     * AT_SHORT   = 9
     * AT_INT     = 10
     * AT_LONG    = 11
     */
    int arr_type = bcr_readu1(&frame->reader);
    char *arr_name;
    switch (arr_type) {
        case 4: arr_name = "[Z"; break;
        case 5: arr_name = "[C"; break;
        case 6: arr_name = "[F"; break;
        case 7: arr_name = "[D"; break;
        case 8: arr_name = "[B"; break;
        case 9: arr_name = "[S"; break;
        case 10: arr_name = "[I"; break;
        case 11: arr_name = "[J"; break;
        default:
            VM_UNKNOWN_ERROR("error. Invalid array type: %d", arr_type);
            return;
    }

    struct class *c = classloader_load_class(frame->method->clazz->loader, arr_name);
    frame_stack_pushr(frame, arrobj_create(c, (size_t) arr_len));
    //os_pushr(frame->operand_stack, (jref) jarrobj_create(c, (size_t) arr_len));
}
