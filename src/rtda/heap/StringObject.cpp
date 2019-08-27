/*
 * Author: kayo
 */

#include <wchar.h>
#include "StringObject.h"
#include "../../util/encoding.h"
#include "../ma/Access.h"
#include "../ma/Field.h"
#include "ArrayObject.h"
#include "Object.h"
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

using namespace std;

StringObject *StringObject::newInst(const char *str)
{
    size_t size = sizeof(StringObject) + java_lang_String_class->instFieldsCount*sizeof(slot_t);
    return new(g_heap_mgr.get(size)) StringObject(str);
}

void StringObject::operator delete(void *rawMemory,std::size_t size) throw()
{
    Object::operator delete(rawMemory, size);
}

StringObject::StringObject(const char *str): Object(java_lang_String_class)
{
    assert(str != nullptr);

    data = reinterpret_cast<slot_t *>(this + 1); // todo data怎么办,这样不对

    jchar *wstr = utf8_to_unicode(str);
    len = wcslen(reinterpret_cast<const wchar_t *>(wstr)); // todo
    ArrayObject *jchars = ArrayObject::newInst(char_array_class, len);
    // 不要使用 wcscpy 直接字符串拷贝，
    // 因为 wcscpy 函数会自动添加字符串结尾 L'\0'，
    // 但 jchars 没有空间容纳字符串结尾符，因为 jchar 是字符数组，不是字符串
    memcpy(jchars->data, wstr, sizeof(jchar) * len);

    raw = reinterpret_cast<const jchar *>(jchars->data);

    clazz->clinit(); // todo

    // todo 要不要调用<init>方法。
    // 给 java/lang/String 类的 value 变量赋值
    setInstFieldValue(S(value), S(array_C), (const slot_t *) &jchars);
}

const char *StringObject::getUtf8Value()
{
    if (utf8Value == nullptr) {
        ArrayObject *char_arr = getInstFieldValue<ArrayObject *>(S(value), S(array_C));
        utf8Value = unicode_to_utf8(reinterpret_cast<const jchar *>(char_arr->data), char_arr->len);
    }
    return utf8Value;
}

string StringObject::toString() const
{
    // todo
    string s;
    return s;
}
