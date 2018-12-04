/*
 * Author: Jia Yang
 */

#include "jobject.h"

/*
 * jdk8下的一些测试：
   1.
        public class Test {
            class T {

            }
            public static void main(String[] args) {
                Class c1 = String.class;
                Class c2 = int.class;
                Class c3 = Integer.class;
                Class c4 = T.class;
            }
        }
        生成如下字节码：
        ldc #2 <java/lang/String>
        astore_1
        getstatic #3 <java/lang/Integer.TYPE>
        astore_2
        ldc #4 <java/lang/Integer>
        astore_3
        ldc #5 <Test$T>
        astore_4
        return
 */

struct jobject* jclsobj_create(struct jclass *entity_class)
{
    assert(entity_class != NULL);

    struct jobject *o = jobject_create(classloader_get_jlclass(bootstrap_loader));
    o->extra = entity_class;
    return o;
}

struct jclass* jclsobj_entity_class(const struct jobject *o)
{
    assert(o != NULL);
    assert(o->extra != NULL);
    return o->extra;
}
