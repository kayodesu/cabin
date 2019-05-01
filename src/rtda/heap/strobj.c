/*
 * Author: Jia Yang
 */

#include <wchar.h>
#include "strobj.h"
#include "../../util/encoding.h"
#include "../ma/access.h"
#include "../ma/field.h"
#include "arrobj.h"
#include "object.h"
#include "../../symbol.h"

/*
 * jdk8下的一些测试：
    1.
        public static void main(String[] args) {
            new String("123");
        }
        生成如下字节码：
        new #2<java/lang/String>
        dup
        ldc #3<123>
        invokespecial #4<java/lang/String.<init>>
        pop
        return
    2.
        public static void main(String[] args) {
            String s = "123";
        }
        生成如下字节码：
        ldc #2<123>
        astore_1
        return
 */

Object* strobj_create(const char *str)
{
    assert(str != NULL);
    Object *o = object_create(classloader_get_jlstring(g_bootstrap_loader));

    jchar *wstr = utf8_to_unicode(str);
    jint len = wcslen(wstr);
    Object *jchars = arrobj_create(load_sys_class("[C"), len);
    // 不要使用 wcscpy 直接字符串拷贝，
    // 因为 wcscpy 函数会自动添加字符串结尾 L'\0'，
    // 但 jchars 没有空间容纳字符串结尾符，因为 jchar 是字符数组，不是字符串
    memcpy(arrobj_data(jchars), wstr, sizeof(jchar) * len);

    class_clinit(o->clazz);

    // todo 要不要调用<init>方法。
    // 给 java/lang/String 类的 value 变量赋值
    Field *field = class_lookup_instance_field(o->clazz, S(value), S(array_C));
    set_instance_field_value(o, field, (const slot_t *) &jchars);
    return o;
}

const char* strobj_value(Object *o)
{
    assert(o != NULL);
    if (o->u.str == NULL) {
        Object *char_arr = RSLOT(get_instance_field_value_by_nt(o, S(value), S(array_C)));
        o->u.str = unicode_to_utf8(arrobj_data(char_arr), arrobj_len(char_arr));
    }
    return o->u.str;
}
