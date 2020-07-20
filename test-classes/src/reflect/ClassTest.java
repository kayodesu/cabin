package reflect;

import java.lang.reflect.Method;
import java.util.Arrays;

/**
 * Status: Fail
 */
public class ClassTest implements Runnable {
    private int a;
    public double b;
    static boolean z;

    public static void main(String[] args) throws Exception {
        _package();
        _class();
        method();
    }

    public static void _package() {
        System.out.println("--- package");
        System.out.println("reflect".equals(ClassTest.class.getPackage().getName()));
    }

    public static void _class() {
        System.out.println("--- class");
        Class<?> c = ClassTest.class;
        System.out.println("reflect.ClassTest".equals(c.getName()));
        System.out.println(Object.class == c.getSuperclass());
        System.out.println(Arrays.deepEquals(new Class<?>[]{Runnable.class}, c.getInterfaces()));
        System.out.println(1 == c.getFields().length);
        System.out.println(3 == c.getDeclaredFields().length);
        System.out.println(14 == c.getMethods().length);
        System.out.println(5 == c.getDeclaredMethods().length);
    }

    public static void method() throws Exception {
        System.out.println("--- method");
        Method main = ClassTest.class.getMethod("main", String[].class);
        System.out.println(Arrays.deepEquals(new Class<?>[]{Exception.class}, main.getExceptionTypes()));
        System.out.println(Arrays.deepEquals(new Class<?>[]{String[].class}, main.getParameterTypes()));
        System.out.println(0 == main.getDeclaredAnnotations().length);

        Method run = ClassTest.class.getMethod("run");
        System.out.println(0 == run.getDeclaredAnnotations().length);
    }

    @Override
    public void run() { }
}
