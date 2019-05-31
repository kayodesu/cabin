/*
 * Author: Jia Yang
 */

#include "ClassObject.h"

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

using namespace std;

ClassObject *ClassObject::newInst(Class *entityClass)
{
    auto classClass = g_bootstrap_loader->jlClass;
    size_t size = sizeof(ClassObject) + classClass->instFieldsCount * sizeof(slot_t);
    return new(g_heap_mgr.get(size)) ClassObject(classClass, entityClass);
}

void ClassObject::operator delete(void *rawMemory,std::size_t size) throw()
{
    Object::operator delete(rawMemory, size);
}

string ClassObject::toString() const
{
    // todo
    string s;
    return s;
}