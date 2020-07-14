package reflect;

import sun.reflect.Reflection;

/**
 * Status: Fail
 */
public class CallerClassTest {
    
    public static void main(String[] args) {
        Foo.test();
    }
    
    static class Foo {
        static void test() {
            Bar.test();
        }
    }
    
    static class Bar {
        static void test() {
            System.out.println(Reflection.getCallerClass(0).getName()); // sun.reflect.Reflection
            System.out.println(Reflection.getCallerClass(1).getName()); // reflect.CallerClassTest$Bar
            System.out.println(Reflection.getCallerClass(2).getName()); // reflect.CallerClassTest$Foo
            System.out.println(Reflection.getCallerClass(3).getName()); // reflect.CallerClassTest
        }
    }
}
