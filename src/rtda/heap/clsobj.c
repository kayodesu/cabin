/*
 * Author: Jia Yang
 */

#include "clsobj.h"
#include "object.h"

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

   2.
        Object obj = new Object();
        Class c1 = obj.getClass();
        Class c2 = c1.getClass();
        Class c3 = c2.getClass();
        System.out.println(c1);
        System.out.println(c2);
        System.out.println(c3);
        System.out.println(c2 == c1);
        System.out.println(c2 == c3);
        输出：
        class java.lang.Object
        class java.lang.Class
        class java.lang.Class
        false
        true
 */

Object* clsobj_create(Class *entity_class)
{
    assert(entity_class != NULL);

    Class *c = classloader_get_jlclass(g_bootstrap_loader);
    Object *o = object_create(c);
    o->u.entity_class = entity_class;

    return o;
}