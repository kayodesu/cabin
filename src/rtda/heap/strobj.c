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

/*
 * todo
 * 在class文件中，字符串是以MUTF8格式保存的，这一点在3.3.7
节讨论过。在Java虚拟机运行期间，字符串以java.lang.String（后面
简称String）对象的形式存在，而在String对象内部，字符串又是以
UTF16格式保存的。字符串相关功能大部分都是由String（和
StringBuilder等）类提供的，本节只实现一些辅助功能即可。

 注意，这里其实是跳过了String的构造函数，
直接用hack的方式创建实例。在前面分析过String类的代码，这样做
虽然有点投机取巧，但确实是没有问题的。

 todo 为什么要这么做， 而不是像其他类一样正常初始化
 */
struct object* strobj_create(const char *str)
{
    assert(str != NULL);
    struct object *o = object_create(classloader_get_jlstring(g_bootstrap_loader));

    jchar *wstr = utf8_to_unicode(str);
    jint len = wcslen(wstr);
    struct object *jchars = arrobj_create(load_sys_class("[C"), len);
    // 不要使用 wcscpy 直接字符串拷贝，
    // 因为 wcscpy 函数会自动添加字符串结尾 L'\0'，
    // 但 jchars 没有空间容纳字符串结尾符，因为 jchar 是字符数组，不是字符串
    memcpy(arrobj_data(jchars), wstr, sizeof(jchar) * len);

// todo 要不要调用 <clinit>, <init>方法。

    // 给 java/lang/String 类的 value 变量赋值  todo
    struct field *field = class_lookup_instance_field(o->clazz, "value", "[C");
    set_instance_field_value(o, field, (const slot_t *) &jchars);
//    for (int i = 0; i < o->clazz->fields_count; i++) {
//        struct field *field = o->clazz->fields + i;
//        if (!IS_STATIC(field->access_flags)
//            && strcmp(field->descriptor, "[C") == 0
//            && strcmp(field->name, "value") == 0) {
//            struct slot s = rslot(jchars);
//            set_instance_field_value_by_id(o, field->id, &s);
//            break;
//        }
//    }

    return o;
}

const char* strobj_value(struct object *o)
{
    assert(o != NULL);
    if (o->u.str == NULL) {
        struct object *char_arr = RSLOT(get_instance_field_value_by_nt(o, "value", "[C"));
        o->u.str = unicode_to_utf8(arrobj_data(char_arr), arrobj_len(char_arr));
    }
    return o->u.str;
}
